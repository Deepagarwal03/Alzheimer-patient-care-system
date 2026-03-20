#include <Wire.h>
#include <LiquidCrystal.h>
#include <MFRC522.h>
#include <DHT.h>
#include "MAX30105.h"
#include "web_pages.h"
#include <ESPAsyncWebServer.h>
#include "heartRate.h"
// Include your two separate model files
#include "model.h"       // Gyro Model (Stationary, Walking, Fall)
#include "modeldht11.h"  // DHT Model (Ideal, Cold State)
#include <WiFi.h>
const char* ssid = "PatientMonitor_AP";
const char* password = "password123";
// LCD Pins
LiquidCrystal lcd(13, 12, 14, 27, 26, 25); 
AsyncWebServer server(80);
// RFID Pins
#define SS_PIN 5
#define RST_PIN 0 
MFRC522 rfid(SS_PIN, RST_PIN);

// Sensor Pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define TRIG_PIN 17
#define ECHO_PIN 16
#define PIR_PIN 34
#define BUZZER_PIN 2 

Eloquent::ML::Port::MotionClassifier motionClf; 
Eloquent::ML::Port::DHTClassifier dhtClf; 

// Motion AI Buffers
const int WINDOW_SIZE = 10;
double magBuffer[WINDOW_SIZE];
int bufIdx = 0;
int sampleCount = 0;

// Global Data
DHT dht(DHTPIN, DHTTYPE);
MAX30105 heartSensor;
float currentTemp, currentHumid, currentDist;
long currentIR;
String currentUID = "None";
String pirStatus = "No Motion";
String patientStatus = "Calibrating";
String envStatus = "Analysing"; 
unsigned long lastRFIDScanTime = 0;

int mode = 0;
unsigned long lastSwitchTime = 0;
const unsigned long displayInterval = 3000; 

const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;

// CSV output to PC via Serial (use a serial logger on the computer)
const bool CSV_MODE = true;
bool csvHeaderPrinted = false;

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // 2. Send the CSS when the HTML asks for it
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", style_css);
  });

  // 3. Send the JS when the HTML asks for it
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "application/javascript", script_js);
  });

  // 4. Send the Dynamic JSON Data for the dashboard
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"motion\":\"" + patientStatus + "\",";
    json += "\"env\":\"" + envStatus + "\",";
    json += "\"temp\":" + String(currentTemp) + ",";
    json += "\"hum\":" + String(currentHumid) + ",";
    json += "\"heart\":" + String(beatAvg) + ","; // beatAvg is the stabilized average
    json += "\"dist\":" + String(currentDist) + ",";
    json += "\"rfid\":\"" + currentUID + "\",";
    json += "\"rfid2\":\"None\",";
    json += "\"near\":\"" + pirStatus + "\",";
    json += "\"ir\":" + String(currentIR);
    json += "}";
    request->send(200, "application/json", json);
  });

  server.begin();
  Serial.begin(115200);
  lcd.begin(16, 2);
  Wire.begin(21, 22);
  
  // Initialize MPU6050 for Motion AI
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  dht.begin();
  SPI.begin();
  rfid.PCD_Init();
  heartSensor.begin(Wire, I2C_SPEED_FAST);
  heartSensor.setup(); 

  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.print("Dual AI System");
  delay(2000);
}

void loop() {
  // 1. RUN MOTION AI CONSTANTLY (High Priority)
  runMotionInference();
  updateHeartRate();
  // 2. RFID MANAGEMENT
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String rawID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      rawID += String(rfid.uid.uidByte[i], HEX);
    }
    
    if (rawID == "739c88fc") currentUID = "Home Gate";
    else if (rawID == "1381f4f4") currentUID = "Bedroom";
    else currentUID = rawID;

    lastRFIDScanTime = millis();
    rfid.PICC_HaltA();
  }
  // 3. CYCLIC SENSOR UPDATES & SERIAL OUTPUT
  if (millis() - lastSwitchTime >= displayInterval) {
    updateEnvironment(); // Refresh DHT, Distance, Heart
    runEnvironmentAI();  // Predict Ideal vs Cold State
    if (CSV_MODE) {
      emitCsv();
    } else {
      printFullDashboard();
    }
    
    mode = (mode + 1) % 6;
    lcd.clear();
    lastSwitchTime = millis();
  }

  refreshLCD();
  delay(50);
}

void runMotionInference() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  
  if (Wire.requestFrom(0x68, 6, true) == 6) {
    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();
    double accel_mag = sqrt((double)ax*ax + (double)ay*ay + (double)az*az);

    magBuffer[bufIdx] = accel_mag;
    bufIdx = (bufIdx + 1) % WINDOW_SIZE;
    if (sampleCount < WINDOW_SIZE) sampleCount++;

    if (sampleCount >= WINDOW_SIZE) {
      double sum = 0, sqDiffSum = 0;
      for(int i=0; i<WINDOW_SIZE; i++) sum += magBuffer[i];
      double mean = sum / WINDOW_SIZE;
      for(int i=0; i<WINDOW_SIZE; i++) sqDiffSum += pow(magBuffer[i] - mean, 2);
      double stdDev = sqrt(sqDiffSum / WINDOW_SIZE);

      // Update PIR status for the Web Dashboard
      int pirRead = digitalRead(PIR_PIN);
      pirStatus = (pirRead == HIGH) ? "Motion Detected" : "No Motion";

      // Features: [0,0,0,0, PIR, Mag, StdDev]
      float features[7] = {0, 0, 0, 0, (float)pirRead, (float)accel_mag, (float)stdDev};
      int pred = motionClf.predict(features);
      
      if (pred == 0) { patientStatus = "Stationary"; noTone(BUZZER_PIN); }
      else if (pred == 1) { patientStatus = "Walking"; noTone(BUZZER_PIN); }
      else if (pred == 2) { patientStatus = "!!! FALL !!!"; tone(BUZZER_PIN, 2000); }
    }
  }
}
void runEnvironmentAI() {
  // Features: [Temp, Humidity]
  float dhtFeatures[2] = {currentTemp, currentHumid};
  int envPred = dhtClf.predict(dhtFeatures);
  
  if (envPred == 0) envStatus = "Ideal State";
  else if (envPred == 1) envStatus = "Cold State";
}

void updateEnvironment() {
  currentTemp = dht.readTemperature();
  currentHumid = dht.readHumidity();
  currentIR = heartSensor.getIR();
  
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH, 30000);
  currentDist = (dur == 0) ? 0 : dur * 0.034 / 2;
}

void updateHeartRate() {
  long irValue = heartSensor.getIR();

  if (checkForBeat(irValue) == true) { 
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++) beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // If no finger is detected (IR < 50000), set average to 0 for the UI
  if (irValue < 50000) {
    beatAvg = 0;
  }
}

void printFullDashboard() {
  Serial.println("\n--- [ DUAL-AI MONITORING ] ---");
  Serial.print("MOTION:  "); Serial.println(patientStatus);
  Serial.print("ENVIRON: "); Serial.println(envStatus);
  Serial.print("HEART:    "); Serial.print(beatAvg); Serial.println(" BPM");
  Serial.print("TEMP:    "); Serial.print(currentTemp); Serial.print("C | HUM: "); Serial.println(currentHumid);
  Serial.print("RFID:    "); Serial.println(currentUID);
  Serial.print("DISTANCE:"); Serial.print(currentDist); Serial.println(" cm");
  Serial.println("------------------------------");
}

void emitCsv() {
  if (!csvHeaderPrinted) {
    Serial.println("timestamp_ms,motion,env_status,temp_c,humidity,heart_bpm,distance_cm,rfid,pir,ir");
    csvHeaderPrinted = true;
  }
  Serial.print(millis());
  Serial.print(",");
  Serial.print(patientStatus);
  Serial.print(",");
  Serial.print(envStatus);
  Serial.print(",");
  Serial.print(currentTemp);
  Serial.print(",");
  Serial.print(currentHumid);
  Serial.print(",");
  Serial.print(beatAvg);
  Serial.print(",");
  Serial.print(currentDist);
  Serial.print(",");
  Serial.print(currentUID);
  Serial.print(",");
  Serial.print(pirStatus);
  Serial.print(",");
  Serial.println(currentIR);
}

void refreshLCD() {
  lcd.setCursor(0,0);
  switch (mode) {
    case 0: lcd.print("Patient: "); lcd.print(patientStatus); 
            lcd.setCursor(0,1); lcd.print("Env: "); lcd.print(envStatus); break;
    case 1: lcd.print("T: "); lcd.print(currentTemp); lcd.print("C"); 
            lcd.setCursor(0,1); lcd.print("H: "); lcd.print(currentHumid); lcd.print("%"); break;
    case 2: lcd.print("Distance:"); lcd.setCursor(0,1); lcd.print(currentDist); lcd.print(" cm"); break;
    case 3: lcd.print("RFID Tag:"); lcd.setCursor(0,1); lcd.print(currentUID); break;
    case 4: lcd.print("Heart IR:"); lcd.setCursor(0,1); lcd.print(currentIR); break;
    case 5: lcd.print("PIR Motion:"); lcd.setCursor(0,1); lcd.print(digitalRead(PIR_PIN) ? "DETECTED" : "NONE"); break;
  }
}

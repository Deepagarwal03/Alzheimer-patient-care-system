# Alzheimer Patient Care System

An ESP32-based patient monitoring prototype designed to support caregivers of Alzheimer’s patients through live sensing, alerts, and AI-assisted insights.

The system combines:

- an ESP32 hardware monitor with multiple sensors
- a browser dashboard served directly from the ESP32
- an optional local Python analytics service for CSV logging and AI inference
- an optional Android app for mobile monitoring and notifications

## Overview

This project monitors patient movement, heart-rate trends, environmental conditions, nearby motion, RFID checkpoints, and possible fall events. It displays the data in a real-time dashboard and adds higher-level insights such as sleep tracking, mobility monitoring, agitation detection, patient-state prediction, and comfort recommendations for caregivers.

## Key Features

- Real-time patient dashboard served from the ESP32
- Motion classification with states such as stationary, walking, and fall
- On-device fall buzzer alert
- Heart rate and IR monitoring using MAX30105
- Temperature and humidity tracking using DHT11
- PIR-based nearby motion sensing
- Ultrasonic distance measurement
- RFID checkpoint or location identification
- LCD readout on the hardware unit
- Local CSV logging for raw and processed sensor data
- CNN-based patient-state prediction in the analytics service
- Random Forest-based comfort and alert recommendations
- Android app wrapper with native notifications

## System Architecture

### 1. ESP32 Device

The ESP32 runs the main sensing and alert loop. It reads sensor values, performs embedded inference for motion and environment classification, and hosts the dashboard over its own Wi-Fi access point.

### 2. Browser Dashboard

The dashboard polls the ESP32 `/data` endpoint every few seconds and updates caregiver-facing panels in real time.

### 3. Local Analytics Service

A Python companion service can poll the ESP32, save raw data to CSV, train lightweight models, and expose processed AI output to the dashboard through a local API.

### 4. Android App

The Android app loads the same dashboard in a `WebView` and raises native notifications for critical situations such as falls and abnormal heart-rate patterns.

## Dashboard Tabs

- **Live Vitals**: heart rate, motion, fall status, temperature, humidity, distance, PIR, RFID, and IR
- **Circadian Rhythm & Sleep**: resting versus active time and sleep guidance
- **Mobility Decay Index**: walking versus stationary trend and weekly decline warnings
- **Environmental Stress Correlation**: cold, ideal, and hot room-state risk analysis
- **Agitation Detection**: sustained heart and IR change while stationary
- **Heart Insight**: spike, drop, recovery, and signal-quality guidance
- **AI Patient State**: processed state prediction from the local analytics service
- **Comfort & Alerts**: action recommendations and risk factors

## Hardware Used

- ESP32
- MAX30105 heart-rate and IR sensor
- DHT11 temperature and humidity sensor
- MPU6050-style motion input over I2C for movement classification
- PIR motion sensor
- Ultrasonic distance sensor
- MFRC522 RFID reader
- 16x2 LCD
- Buzzer

## Repository Structure

```text
.
├── project.ino                     # Main ESP32 firmware
├── model.h                         # Embedded motion classifier
├── modeldht11.h                    # Embedded environment classifier
├── index.html                      # Dashboard HTML source
├── style.css                       # Dashboard styling
├── script.js                       # Dashboard logic
├── web_pages.h                     # Generated embedded web assets
├── analytics/
│   ├── service.py                  # Local logging + inference service
│   ├── requirements.txt
│   └── README.md
├── android-app/
│   └── PatientMonitorApp/          # Android Studio project
└── tools/
    └── generate_web_pages.py       # Regenerates web_pages.h from HTML/CSS/JS
```

## Tech Stack

### Embedded

- Arduino / ESP32
- ESPAsyncWebServer
- WiFi
- Wire
- SPI
- LiquidCrystal
- MFRC522
- DHT
- MAX30105

### Dashboard

- HTML
- CSS
- Vanilla JavaScript

### Analytics Service

- Python
- Flask
- NumPy
- scikit-learn

### Mobile

- Kotlin
- Android Studio
- OkHttp

## How It Works

1. The ESP32 boots and creates a Wi-Fi access point.
2. Sensors are initialized and sampled continuously.
3. Embedded models classify patient motion and environment state.
4. The ESP32 serves:
   - `/` for the dashboard
   - `/style.css`
   - `/script.js`
   - `/data` for live JSON sensor output
5. The browser dashboard polls `/data` and updates the UI.
6. Optionally, the Python analytics service polls the ESP32, stores raw CSV logs, trains models, and serves processed results at `http://127.0.0.1:5001/api/latest`.
7. Optionally, the Android app polls the same ESP32 endpoint and raises native alerts.

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/Deepagarwal03/Alzheimer-patient-care-system.git
cd Alzheimer-patient-care-system
```

### 2. Prepare the ESP32 Firmware

Open `project.ino` in the Arduino IDE or PlatformIO and make sure the required libraries are installed.

Important values currently set in the firmware:

- Wi-Fi SSID: `PatientMonitor_AP`
- Wi-Fi password: `password123`

The firmware serves the dashboard directly from the ESP32 and exposes live values at:

- `http://192.168.4.1/`
- `http://192.168.4.1/data`

### 3. Flash the ESP32

Upload `project.ino` to the ESP32 with your sensor wiring connected.

After flashing:

1. Power the device
2. Connect a phone or laptop to the ESP32 access point
3. Open `http://192.168.4.1/` in a browser

## Running the Optional Analytics Service

The dashboard can run without this service, but the **AI Patient State** and **Comfort & Alerts** tabs depend on it.

### Install Dependencies

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r analytics/requirements.txt
```

On Windows, activate the virtual environment with:

```powershell
.venv\Scripts\Activate.ps1
```

### Start the Analytics Service

```bash
python analytics/service.py --esp32-url http://192.168.4.1
```

This service will:

- poll the ESP32 `/data` endpoint
- save raw sensor data to `analytics/data/raw_sensor_log.csv`
- create or reuse a seed dataset at `analytics/data/synthetic_seed_dataset.csv`
- train a lightweight CNN and Random Forest
- save processed predictions to `analytics/data/processed_predictions.csv`
- expose processed results at `http://127.0.0.1:5001/api/latest`

If you run the analytics API on a different machine or port, set the dashboard override in the browser:

```javascript
localStorage.setItem("analyticsApiBase", "http://YOUR-PC-IP:5001/api");
```

## Running the Android App

The Android app is located in:

```text
android-app/PatientMonitorApp
```

To run it:

1. Open the folder in Android Studio
2. Let Gradle sync
3. Connect an Android device or start an emulator
4. Run the app

The app:

- loads the ESP32 dashboard in a `WebView`
- polls `http://192.168.4.1/data`
- sends notifications for:
  - falls
  - heart-rate spikes
  - heart-rate drops
  - weak IR signal
  - cold-stress with elevated BPM

## Regenerating Embedded Web Files

If you edit `index.html`, `style.css`, or `script.js`, regenerate `web_pages.h` before reflashing the ESP32:

```bash
python tools/generate_web_pages.py
```

## Data Files Produced by the Analytics Service

- `analytics/data/raw_sensor_log.csv`
- `analytics/data/processed_predictions.csv`
- `analytics/data/synthetic_seed_dataset.csv`

## Notes and Limitations

- The dashboard supports both body temperature and environment temperature fields, but the current ESP32 payload falls back to the same temperature source unless extended.
- The analytics service starts with a synthetic seed dataset and becomes more useful as real sensor data is collected.
- The Android app currently provides notifications while the app is active in the foreground.
- This project is a prototype for academic, demonstration, or hobby use and is not a certified medical device.

## Future Improvements

- Cloud-backed storage and remote caregiver access
- Background Android alert service
- Better model training using real patient datasets
- Multi-patient support
- Stronger authentication and secure networking
- Additional health sensors and anomaly detection

## License

See `LICENSE.txt`.

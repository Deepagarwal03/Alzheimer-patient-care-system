# Patient Monitor Android App

This Android app loads the same ESP32 dashboard inside a phone app and adds native alert notifications.

## What it does

- Loads the ESP32 dashboard in a WebView
- Shows the same readings and AI outputs already visible on the webpage
- Polls `http://192.168.4.1/data` in the app
- Sends native Android notifications for:
  - fall alerts
  - heart-rate spikes
  - heart-rate drops
  - weak IR signal
  - cold-stress with elevated BPM

## Open in Android Studio

1. Open `android-app/PatientMonitorApp`
2. Let Gradle sync
3. Connect your Android phone or start an emulator
4. Run the app

## Notes

- The app currently assumes the ESP32 base URL is `http://192.168.4.1`
- If your ESP32 IP changes, update `BASE_URL` in `MainActivity.kt`
- Notifications work while the app is running in the foreground
- For full background notifications even when the app is closed, the next step would be a foreground service or push backend

# Sensor Analytics Companion Service

This folder adds the missing PC-side pipeline that the ESP32 alone cannot do reliably:

- Poll live sensor values from the ESP32 `/data` endpoint
- Save every raw reading to `analytics/data/raw_sensor_log.csv`
- Bootstrap a sample dataset in `analytics/data/synthetic_seed_dataset.csv`
- Train a lightweight 1D CNN and a Random Forest on the seed + captured sensor data
- Save every processed prediction to `analytics/data/processed_predictions.csv`
- Expose the latest processed result to the dashboard at `http://127.0.0.1:5001/api/latest`

## Why a local companion service is needed

The browser cannot automatically write CSV files to your disk by itself, and a heavier CNN/Random Forest pipeline is better kept off the ESP32. The clean split is:

- ESP32: sensing + live webpage + raw `/data`
- PC/Laptop: CSV logging + model training + processed predictions

## Setup

1. Create a Python virtual environment:

```powershell
python -m venv .venv
```

2. Activate it:

```powershell
.venv\Scripts\Activate.ps1
```

3. Install the companion dependencies:

```powershell
pip install -r analytics\requirements.txt
```

4. Start the analytics service while connected to the ESP32 access point:

```powershell
python analytics\service.py --esp32-url http://192.168.4.1
```

5. Open the ESP32 dashboard in the browser as you do now. The new `AI Patient State` and `Comfort & Alerts` tabs will pull processed results from the local service.

## Output files

- Raw sensor log: `analytics/data/raw_sensor_log.csv`
- Processed predictions: `analytics/data/processed_predictions.csv`
- Seed training dataset: `analytics/data/synthetic_seed_dataset.csv`
- Saved models: `analytics/models/`

## Notes

- The current code falls back to the same temperature value for both `body_temp_c` and `env_temp_c` if your `/data` payload only exposes one temperature field.
- Retraining happens automatically after enough new raw rows are collected.
- If you run the analytics API on a different machine or port, set the browser value:

```javascript
localStorage.setItem("analyticsApiBase", "http://YOUR-PC-IP:5001/api");
```


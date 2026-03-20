const char index_html[] PROGMEM = R"rawliteral( 
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Alzheimer Patient Care Dashboard</title>
  <link rel="preconnect" href="https://fonts.googleapis.com" />
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
  <link href="https://fonts.googleapis.com/css2?family=Manrope:wght@400;600;700;800&display=swap" rel="stylesheet" />
  <link rel="stylesheet" href="style.css" />
</head>
<body>
  <div class="bg-orb orb-1"></div>
  <div class="bg-orb orb-2"></div>
  <main class="container">
    <header class="topbar">
      <div class="title-block">
        <p class="kicker">Care Monitor</p>
        <h1>Alzheimer Patient Care System</h1>
        <p class="subtitle">Real-time patient safety and environment dashboard</p>
      </div>
      <div class="topbar-right">
        <div class="live-dot"></div>
        <div class="status-chip" id="lastUpdated">Live data</div>
      </div>
    </header>

    <section class="tabs">
      <button class="tab-btn active" data-tab="vitalsTab">Live Vitals</button>
      <button class="tab-btn" data-tab="circadianTab">Circadian Rhythm & Sleep</button>
      <button class="tab-btn" data-tab="mobilityTab">Mobility Decay Index</button>
      <button class="tab-btn" data-tab="envStressTab">Environmental Stress Correlation</button>
      <button class="tab-btn" data-tab="agitationTab">Agitation Detection</button>
    </section>

    <section class="tab-panel active" id="vitalsTab">
      <section class="grid">
      <article class="card heart-card">
        <p class="label">Heart Rate</p>
        <p class="value"><span id="heartRate">76</span> <span class="unit">BPM</span></p>
        <div class="viz wave-viz" id="heartViz"><span></span></div>
      </article>

      <article class="card motion-card">
        <p class="label">Patient Motion</p>
        <p class="value" id="patientMotion">Resting</p>
        <div class="viz track-viz" id="patientMotionViz"><span></span></div>
      </article>

      <article class="card env-status-card">
        <p class="label">Environment Status</p>
        <p class="value" id="envStatus">Ideal State</p>
        <div class="viz meter-viz" id="envStatusViz"><span></span></div>
      </article>

      <article class="card alert-card" id="fallCard">
        <p class="label">Fall Alert</p>
        <p class="value" id="fallAlert">No Fall Detected</p>
        <div class="viz pulse-viz" id="fallViz"><span></span></div>
      </article>

      <article class="card body-temp-card">
        <p class="label">Body Temperature</p>
        <p class="value"><span id="bodyTemp">36.7</span> <span class="unit">deg C</span></p>
        <div class="viz meter-viz" id="bodyTempViz"><span></span></div>
      </article>

      <article class="card env-temp-card">
        <p class="label">Surrounding Temperature</p>
        <p class="value"><span id="envTemp">24.8</span> <span class="unit">deg C</span></p>
        <div class="viz meter-viz" id="envTempViz"><span></span></div>
      </article>

      <article class="card humidity-card">
        <p class="label">Humidity</p>
        <p class="value"><span id="humidity">54</span> <span class="unit">%</span></p>
        <div class="viz meter-viz" id="humidityViz"><span></span></div>
      </article>

      <article class="card pir-card">
        <p class="label">PIR Motion</p>
        <p class="value" id="pirMotion">No Motion</p>
        <div class="viz track-viz" id="pirViz"><span></span></div>
      </article>

      <article class="card rfid-card">
        <p class="label">RFID IDs</p>
        <div class="rfid-wrap">
          <p class="rfid">Primary: <span id="rfid1">A7-91-2C-FF</span></p>
          <p class="rfid">Secondary: <span id="rfid2">B4-20-9D-61</span></p>
        </div>
        <div class="viz scan-viz" id="rfidViz"><span></span></div>
      </article>

      <article class="card object-card">
        <p class="label">Nearest Object</p>
        <p class="value" id="nearestObject">Water Bottle (0.7 m)</p>
        <div class="viz meter-viz" id="objectViz"><span></span></div>
      </article>

      <article class="card near-motion-card">
        <p class="label">Motion Near Patient</p>
        <p class="value" id="nearMotion">Moving</p>
        <div class="viz track-viz" id="nearMotionViz"><span></span></div>
      </article>

      <article class="card ir-card">
        <p class="label">Heart Sensor IR</p>
        <p class="value"><span id="heartIr">0</span></p>
        <div class="viz wave-viz" id="irViz"><span></span></div>
      </article>
      </section>
    </section>

    <section class="tab-panel" id="circadianTab">
      <article class="card sleep-card">
        <p class="label">Circadian Rhythm & Sleep Pattern Tracking</p>
        <div class="chart-wrap">
          <canvas id="sleepChart" width="1100" height="360"></canvas>
        </div>
        <div class="sleep-hours">
          <p class="sleep-title">Total Hours Slept</p>
          <p class="sleep-value" id="totalSleepHours">0.00 hrs</p>
          <p class="sleep-formula" id="sleepFormula">Resting hours - Active hours</p>
        </div>
      </article>
    </section>

    <section class="tab-panel" id="mobilityTab">
      <article class="card mobility-card">
        <p class="label">Mobility Decay Index</p>
        <div class="mobility-grid">
          <div class="mobility-stat">
            <p class="mobility-stat-label">Today's Walking %</p>
            <p class="mobility-stat-value" id="walkingPct">0.0%</p>
          </div>
          <div class="mobility-stat">
            <p class="mobility-stat-label">Today's Stationary %</p>
            <p class="mobility-stat-value" id="stationaryPct">0.0%</p>
          </div>
          <div class="mobility-stat">
            <p class="mobility-stat-label">Walking / Stationary Ratio</p>
            <p class="mobility-stat-value" id="walkStationaryRatio">0.00</p>
          </div>
        </div>

        <div class="week-strip" id="mobilityWeek"></div>

        <div class="mobility-warning" id="mobilityWarning">
          Mobility stable. No weekly decay warning.
        </div>
      </article>
    </section>

    <section class="tab-panel" id="envStressTab">
      <article class="card env-stress-card">
        <p class="label">Environmental Stress Correlation</p>
        <div class="env-grid">
          <div class="env-stat">
            <p class="env-stat-label">Cold State Risk</p>
            <p class="env-stat-value" id="coldRisk">0.0%</p>
            <p class="env-stat-sub" id="coldEvents">0 / 0 events</p>
          </div>
          <div class="env-stat">
            <p class="env-stat-label">Ideal State Risk</p>
            <p class="env-stat-value" id="idealRisk">0.0%</p>
            <p class="env-stat-sub" id="idealEvents">0 / 0 events</p>
          </div>
          <div class="env-stat">
            <p class="env-stat-label">Hot State Risk</p>
            <p class="env-stat-value" id="hotRisk">0.0%</p>
            <p class="env-stat-sub" id="hotEvents">0 / 0 events</p>
          </div>
        </div>
        <div class="env-summary" id="envSummary">
          Collecting data to correlate falls or high heart rate with room temperature states.
        </div>
      </article>
    </section>

    <section class="tab-panel" id="agitationTab">
      <article class="card agitation-card">
        <p class="label">Agitation Detection (Heart Rate Variability)</p>
        <div class="agitation-grid">
          <div class="agitation-stat">
            <p class="agitation-stat-label">Current Status</p>
            <p class="agitation-stat-value" id="agitationStatus">Stable</p>
          </div>
          <div class="agitation-stat">
            <p class="agitation-stat-label">Baseline BPM (Stationary)</p>
            <p class="agitation-stat-value" id="baselineBpm">--</p>
          </div>
          <div class="agitation-stat">
            <p class="agitation-stat-label">Current Delta</p>
            <p class="agitation-stat-value" id="agitationDelta">0 BPM</p>
          </div>
          <div class="agitation-stat">
            <p class="agitation-stat-label">Sustained Increase</p>
            <p class="agitation-stat-value" id="agitationSustain">0s</p>
          </div>
          <div class="agitation-stat">
            <p class="agitation-stat-label">Events Today</p>
            <p class="agitation-stat-value" id="agitationEvents">0</p>
          </div>
          <div class="agitation-stat">
            <p class="agitation-stat-label">Last Event</p>
            <p class="agitation-stat-value" id="agitationLast">--</p>
          </div>
        </div>
        <div class="agitation-summary" id="agitationSummary">
          Tracking sustained heart rate increases while stationary. No agitation detected.
        </div>
      </article>
    </section>
  </main>

  <script src="script.js"></script>
</body>
</html>
)rawliteral";

const char style_css[] PROGMEM = R"rawliteral(
:root {
  --bg-1: #e6f3f3;
  --bg-2: #f9ecd8;
  --ink: #142025;
  --muted: #4b626b;
  --line: rgba(20, 32, 37, 0.14);
  --glass: rgba(255, 255, 255, 0.72);
  --card-strong: rgba(255, 255, 255, 0.92);
  --teal: #0b8a83;
  --teal-dark: #0a6f6a;
  --orange: #de8d38;
  --danger: #d34242;
  --danger-bg: rgba(255, 226, 226, 0.92);
  --radius-xl: 24px;
  --radius-lg: 18px;
  --shadow: 0 14px 36px rgba(20, 32, 37, 0.11);
}

* {
  box-sizing: border-box;
}

body {
  margin: 0;
  min-height: 100vh;
  font-family: "Manrope", sans-serif;
  color: var(--ink);
  background:
    radial-gradient(circle at 8% 8%, #d0ece9 0%, transparent 37%),
    radial-gradient(circle at 94% 92%, #ffe1be 0%, transparent 36%),
    linear-gradient(155deg, var(--bg-1), var(--bg-2));
  overflow-x: hidden;
}

.bg-orb {
  position: fixed;
  border-radius: 50%;
  filter: blur(60px);
  opacity: 0.48;
  z-index: -1;
}

.orb-1 {
  width: 360px;
  height: 360px;
  background: rgba(90, 191, 182, 0.6);
  top: -120px;
  right: -100px;
}

.orb-2 {
  width: 300px;
  height: 300px;
  background: rgba(255, 189, 114, 0.55);
  bottom: -120px;
  left: -90px;
}

.container {
  max-width: 1180px;
  margin: 0 auto;
  padding: 32px 20px 42px;
}

.topbar {
  display: flex;
  align-items: flex-start;
  justify-content: space-between;
  gap: 14px;
  margin-bottom: 22px;
  padding: 18px;
  border: 1px solid var(--line);
  border-radius: var(--radius-xl);
  background: linear-gradient(140deg, rgba(255, 255, 255, 0.84), rgba(255, 255, 255, 0.58));
  backdrop-filter: blur(14px);
  box-shadow: var(--shadow);
}

.kicker {
  margin: 0;
  text-transform: uppercase;
  letter-spacing: 0.16em;
  font-weight: 800;
  color: var(--teal-dark);
  font-size: 11px;
}

h1 {
  margin: 7px 0 0;
  font-size: clamp(1.5rem, 2.9vw, 2.5rem);
  line-height: 1.12;
}

.subtitle {
  margin: 8px 0 0;
  font-size: 0.94rem;
  color: var(--muted);
  font-weight: 600;
}

.topbar-right {
  display: inline-flex;
  align-items: center;
  gap: 9px;
}

.live-dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
  background: #16b074;
  box-shadow: 0 0 0 0 rgba(22, 176, 116, 0.55);
  animation: pulse 1.5s infinite;
}

.status-chip {
  border: 1px solid var(--line);
  border-radius: 999px;
  background: var(--card-strong);
  padding: 9px 13px;
  font-size: 0.84rem;
  color: var(--muted);
  font-weight: 700;
  white-space: nowrap;
}

.tabs {
  display: flex;
  gap: 10px;
  margin: 6px 0 18px;
}

.tab-btn {
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.72);
  color: var(--muted);
  border-radius: 999px;
  padding: 9px 15px;
  font-family: inherit;
  font-size: 0.88rem;
  font-weight: 800;
  cursor: pointer;
  transition: all 0.2s ease;
}

.tab-btn.active {
  background: linear-gradient(120deg, rgba(11, 138, 131, 0.9), rgba(11, 138, 131, 0.74));
  border-color: rgba(11, 138, 131, 0.5);
  color: #ffffff;
}

.tab-panel {
  display: none;
}

.tab-panel.active {
  display: block;
}

.grid {
  display: grid;
  grid-template-columns: repeat(12, minmax(0, 1fr));
  gap: 14px;
}

.card {
  grid-column: span 4;
  background: var(--glass);
  border: 1px solid var(--line);
  border-radius: var(--radius-lg);
  padding: 18px;
  backdrop-filter: blur(10px);
  box-shadow: 0 8px 24px rgba(20, 32, 37, 0.08);
  animation: rise 0.45s ease both;
}

.heart-card,
.motion-card,
.alert-card {
  grid-column: span 4;
  background: var(--card-strong);
}

.heart-card {
  background: linear-gradient(165deg, rgba(235, 255, 252, 0.95), rgba(221, 245, 242, 0.92));
}

.motion-card {
  background: linear-gradient(165deg, rgba(255, 245, 227, 0.95), rgba(255, 237, 211, 0.92));
}

.alert-card {
  background: linear-gradient(165deg, rgba(233, 255, 247, 0.95), rgba(219, 245, 237, 0.92));
}

.label {
  margin: 0 0 11px;
  font-size: 0.84rem;
  color: var(--muted);
  font-weight: 800;
  letter-spacing: 0.04em;
  text-transform: uppercase;
}

.value {
  margin: 0;
  font-size: clamp(1.28rem, 2vw, 1.82rem);
  font-weight: 800;
  line-height: 1.2;
}

.unit {
  font-size: 0.9rem;
  font-weight: 800;
  color: var(--muted);
}

.rfid-wrap {
  display: grid;
  gap: 8px;
}

.rfid {
  margin: 0;
  font-weight: 700;
  font-size: 0.95rem;
  padding: 8px 10px;
  border-radius: 10px;
  border: 1px dashed rgba(20, 32, 37, 0.18);
  background: rgba(255, 255, 255, 0.7);
}

.viz {
  position: relative;
  margin-top: 14px;
  height: 18px;
  border-radius: 999px;
  overflow: hidden;
  border: 1px solid rgba(20, 32, 37, 0.12);
  background: rgba(255, 255, 255, 0.7);
}

.viz span {
  display: block;
  height: 100%;
}

.meter-viz span {
  width: calc(var(--level, 40) * 1%);
  border-radius: 999px;
  background: linear-gradient(90deg, rgba(11, 138, 131, 0.72), rgba(11, 138, 131, 0.95));
  animation: meterSlide var(--flow, 1.8s) linear infinite;
}

.wave-viz span {
  width: 200%;
  background:
    radial-gradient(circle at 15px 50%, rgba(11, 138, 131, 0.95) 0 5px, transparent 6px),
    radial-gradient(circle at 45px 50%, rgba(11, 138, 131, 0.5) 0 4px, transparent 5px),
    linear-gradient(90deg, rgba(11, 138, 131, 0.22), rgba(11, 138, 131, 0.58));
  background-size: 60px 100%, 60px 100%, 100% 100%;
  animation: waveMove var(--speed, 1.1s) linear infinite;
}

.track-viz span {
  width: 22%;
  border-radius: 999px;
  background: linear-gradient(90deg, rgba(222, 141, 56, 0.95), rgba(245, 184, 123, 0.85));
  animation: runner var(--speed, 2.6s) ease-in-out infinite;
}

.pulse-viz span {
  width: 100%;
  background: linear-gradient(90deg, rgba(39, 166, 118, 0.45), rgba(77, 197, 153, 0.5));
}

.pulse-viz.alert {
  border-color: rgba(211, 66, 66, 0.4);
  background: rgba(255, 236, 236, 0.92);
}

.pulse-viz.alert span {
  background: linear-gradient(90deg, rgba(211, 66, 66, 0.45), rgba(255, 120, 120, 0.68));
  animation: alertPulse var(--speed, 0.75s) ease-in-out infinite;
}

.scan-viz span {
  width: 24%;
  border-radius: 999px;
  background: linear-gradient(90deg, rgba(11, 138, 131, 0.08), rgba(11, 138, 131, 0.92), rgba(11, 138, 131, 0.08));
  box-shadow: 0 0 16px rgba(11, 138, 131, 0.4);
  animation: scanner var(--speed, 1.7s) linear infinite;
}

.sleep-card {
  background: linear-gradient(165deg, rgba(243, 254, 255, 0.95), rgba(236, 250, 246, 0.92));
}

.chart-wrap {
  width: 100%;
  border-radius: 14px;
  border: 1px solid rgba(20, 32, 37, 0.14);
  background: rgba(255, 255, 255, 0.8);
  padding: 10px;
}

#sleepChart {
  width: 100%;
  height: auto;
  display: block;
}

.sleep-hours {
  margin-top: 14px;
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.78);
  padding: 12px;
}

.sleep-title {
  margin: 0;
  color: var(--muted);
  font-size: 0.85rem;
  font-weight: 700;
}

.sleep-value {
  margin: 4px 0 2px;
  font-size: 1.9rem;
  font-weight: 800;
  line-height: 1;
}

.sleep-formula {
  margin: 0;
  color: var(--muted);
  font-size: 0.84rem;
  font-weight: 700;
}

.mobility-card {
  background: linear-gradient(165deg, rgba(255, 248, 236, 0.95), rgba(243, 252, 255, 0.92));
}

.mobility-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
  margin-bottom: 14px;
}

.mobility-stat {
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.8);
  padding: 10px;
}

.mobility-stat-label {
  margin: 0;
  font-size: 0.8rem;
  color: var(--muted);
  font-weight: 700;
}

.mobility-stat-value {
  margin: 4px 0 0;
  font-size: 1.5rem;
  font-weight: 800;
  line-height: 1.1;
}

.week-strip {
  display: grid;
  grid-template-columns: repeat(7, minmax(0, 1fr));
  gap: 8px;
}

.week-day {
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 10px;
  background: rgba(255, 255, 255, 0.72);
  padding: 8px 6px;
}

.week-day-name {
  margin: 0 0 6px;
  font-size: 0.72rem;
  color: var(--muted);
  font-weight: 700;
  text-align: center;
}

.week-bar {
  height: 56px;
  border-radius: 8px;
  border: 1px solid rgba(20, 32, 37, 0.12);
  background: rgba(255, 255, 255, 0.9);
  overflow: hidden;
  display: flex;
  align-items: flex-end;
}

.week-bar-fill {
  width: 100%;
  background: linear-gradient(180deg, rgba(11, 138, 131, 0.95), rgba(11, 138, 131, 0.55));
  transition: height 0.3s ease;
}

.week-day-pct {
  margin: 6px 0 0;
  text-align: center;
  font-size: 0.72rem;
  font-weight: 800;
}

.mobility-warning {
  margin-top: 14px;
  border: 1px solid rgba(22, 176, 116, 0.35);
  border-radius: 12px;
  background: rgba(227, 255, 242, 0.88);
  color: #1c6f4f;
  font-size: 0.9rem;
  font-weight: 800;
  padding: 10px 12px;
}

.mobility-warning.alert {
  border-color: rgba(211, 66, 66, 0.45);
  background: rgba(255, 230, 230, 0.9);
  color: #a52e2e;
}

.env-stress-card {
  background: linear-gradient(165deg, rgba(240, 247, 255, 0.96), rgba(235, 254, 245, 0.92));
}

.env-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
}

.env-stat {
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.8);
  padding: 10px;
}

.env-stat-label {
  margin: 0;
  color: var(--muted);
  font-size: 0.8rem;
  font-weight: 700;
}

.env-stat-value {
  margin: 6px 0 4px;
  font-size: 1.5rem;
  font-weight: 800;
  line-height: 1.1;
}

.env-stat-sub {
  margin: 0;
  color: var(--muted);
  font-size: 0.78rem;
  font-weight: 700;
}

.env-summary {
  margin-top: 14px;
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.84);
  padding: 12px;
  font-size: 0.9rem;
  font-weight: 800;
  color: #1d5f73;
}

.env-summary.alert {
  border-color: rgba(211, 66, 66, 0.4);
  background: rgba(255, 233, 233, 0.9);
  color: #a52e2e;
}

.agitation-card {
  background: linear-gradient(165deg, rgba(255, 244, 248, 0.96), rgba(244, 250, 255, 0.92));
}

.agitation-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
}

.agitation-stat {
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.8);
  padding: 10px;
}

.agitation-stat-label {
  margin: 0;
  color: var(--muted);
  font-size: 0.8rem;
  font-weight: 700;
}

.agitation-stat-value {
  margin: 6px 0 0;
  font-size: 1.5rem;
  font-weight: 800;
  line-height: 1.1;
}

.agitation-summary {
  margin-top: 14px;
  border: 1px solid rgba(20, 32, 37, 0.14);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.84);
  padding: 12px;
  font-size: 0.9rem;
  font-weight: 800;
  color: #6a3b6b;
}

.agitation-summary.alert {
  border-color: rgba(211, 66, 66, 0.4);
  background: rgba(255, 233, 233, 0.9);
  color: #a52e2e;
}

.alert-card.alert {
  border-color: rgba(211, 66, 66, 0.58);
  background: var(--danger-bg);
  box-shadow: 0 10px 24px rgba(211, 66, 66, 0.22);
}

.alert-card.alert .value {
  color: var(--danger);
}

.card:nth-child(2) { animation-delay: 0.03s; }
.card:nth-child(3) { animation-delay: 0.06s; }
.card:nth-child(4) { animation-delay: 0.09s; }
.card:nth-child(5) { animation-delay: 0.12s; }
.card:nth-child(6) { animation-delay: 0.15s; }
.card:nth-child(7) { animation-delay: 0.18s; }
.card:nth-child(8) { animation-delay: 0.21s; }
.card:nth-child(9) { animation-delay: 0.24s; }

@keyframes rise {
  from {
    opacity: 0;
    transform: translateY(8px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

@keyframes pulse {
  0% { box-shadow: 0 0 0 0 rgba(22, 176, 116, 0.55); }
  70% { box-shadow: 0 0 0 10px rgba(22, 176, 116, 0); }
  100% { box-shadow: 0 0 0 0 rgba(22, 176, 116, 0); }
}

@keyframes meterSlide {
  0% { filter: brightness(0.95); }
  50% { filter: brightness(1.12); }
  100% { filter: brightness(0.95); }
}

@keyframes waveMove {
  from { transform: translateX(0); }
  to { transform: translateX(-50%); }
}

@keyframes runner {
  0% { transform: translateX(-10%); }
  50% { transform: translateX(390%); }
  100% { transform: translateX(-10%); }
}

@keyframes scanner {
  0% { transform: translateX(-40%); }
  100% { transform: translateX(430%); }
}

@keyframes alertPulse {
  0% { transform: scaleY(0.65); opacity: 0.65; }
  50% { transform: scaleY(1); opacity: 1; }
  100% { transform: scaleY(0.65); opacity: 0.65; }
}

@media (max-width: 980px) {
  .card,
  .heart-card,
  .motion-card,
  .alert-card {
    grid-column: span 6;
  }
}

@media (max-width: 640px) {
  .container {
    padding-top: 18px;
  }

  .topbar {
    flex-direction: column;
    align-items: flex-start;
  }

  .card,
  .heart-card,
  .motion-card,
  .alert-card {
    grid-column: 1 / -1;
  }

  .value {
    font-size: 1.32rem;
  }

  .tabs {
    flex-wrap: wrap;
  }

  .mobility-grid {
    grid-template-columns: 1fr;
  }

  .week-strip {
    grid-template-columns: repeat(4, minmax(0, 1fr));
  }

  .env-grid {
    grid-template-columns: 1fr;
  }

  .agitation-grid {
    grid-template-columns: 1fr;
  }
}
})rawliteral";

const char script_js[] PROGMEM = R"rawliteral(
const el = {
  heartRate: document.getElementById("heartRate"),
  patientMotion: document.getElementById("patientMotion"),
  fallAlert: document.getElementById("fallAlert"),
  fallCard: document.getElementById("fallCard"),
  envStatus: document.getElementById("envStatus"),
  bodyTemp: document.getElementById("bodyTemp"),
  envTemp: document.getElementById("envTemp"),
  humidity: document.getElementById("humidity"),
  pirMotion: document.getElementById("pirMotion"),
  rfid1: document.getElementById("rfid1"),
  rfid2: document.getElementById("rfid2"),
  nearestObject: document.getElementById("nearestObject"),
  nearMotion: document.getElementById("nearMotion"),
  heartIr: document.getElementById("heartIr"),
  lastUpdated: document.getElementById("lastUpdated"),
  heartViz: document.getElementById("heartViz"),
  patientMotionViz: document.getElementById("patientMotionViz"),
  fallViz: document.getElementById("fallViz"),
  envStatusViz: document.getElementById("envStatusViz"),
  bodyTempViz: document.getElementById("bodyTempViz"),
  envTempViz: document.getElementById("envTempViz"),
  humidityViz: document.getElementById("humidityViz"),
  pirViz: document.getElementById("pirViz"),
  rfidViz: document.getElementById("rfidViz"),
  objectViz: document.getElementById("objectViz"),
  nearMotionViz: document.getElementById("nearMotionViz"),
  irViz: document.getElementById("irViz")
};

const objects = [
  "Water Bottle",
  "Pill Box",
  "Remote",
  "Bed Rail",
  "Walking Stick"
];

const tabButtons = Array.from(document.querySelectorAll(".tab-btn"));
const tabPanels = Array.from(document.querySelectorAll(".tab-panel"));
const sleepCanvas = document.getElementById("sleepChart");
const sleepCtx = sleepCanvas?.getContext("2d");
const totalSleepHoursEl = document.getElementById("totalSleepHours");
const sleepFormulaEl = document.getElementById("sleepFormula");
const walkingPctEl = document.getElementById("walkingPct");
const stationaryPctEl = document.getElementById("stationaryPct");
const walkStationaryRatioEl = document.getElementById("walkStationaryRatio");
const mobilityWeekEl = document.getElementById("mobilityWeek");
const mobilityWarningEl = document.getElementById("mobilityWarning");
const coldRiskEl = document.getElementById("coldRisk");
const idealRiskEl = document.getElementById("idealRisk");
const hotRiskEl = document.getElementById("hotRisk");
const coldEventsEl = document.getElementById("coldEvents");
const idealEventsEl = document.getElementById("idealEvents");
const hotEventsEl = document.getElementById("hotEvents");
const envSummaryEl = document.getElementById("envSummary");
const agitationStatusEl = document.getElementById("agitationStatus");
const baselineBpmEl = document.getElementById("baselineBpm");
const agitationDeltaEl = document.getElementById("agitationDelta");
const agitationSustainEl = document.getElementById("agitationSustain");
const agitationEventsEl = document.getElementById("agitationEvents");
const agitationLastEl = document.getElementById("agitationLast");
const agitationSummaryEl = document.getElementById("agitationSummary");

const sleepState = {
  startMs: Date.now(),
  lastMs: Date.now(),
  lastMode: "Active",
  history: [{ hours: 0, level: 1 }],
  restingMs: 0,
  activeMs: 0
};

const mobilityState = {
  lastMs: Date.now(),
  lastMode: "Stationary",
  days: {}
};

const envStressState = {
  lastMs: Date.now(),
  lastTempState: "ideal",
  days: {},
  stressActive: false
};

const agitationState = {
  baselineBpm: null,
  sustainMs: 0,
  lastMs: Date.now(),
  active: false,
  events: 0,
  lastEventMs: null
};

function randomBetween(min, max, digits = 0) {
  const n = Math.random() * (max - min) + min;
  return Number(n.toFixed(digits));
}

function clamp(value, min, max) {
  return Math.min(Math.max(value, min), max);
}

function toNumber(value, fallback) {
  const num = Number(value);
  return Number.isFinite(num) ? num : fallback;
}

function setMeter(viz, level, flowSeconds) {
  viz?.style.setProperty("--level", String(clamp(level, 0, 100)));
  viz?.style.setProperty("--flow", `${clamp(flowSeconds, 0.6, 3).toFixed(2)}s`);
}

function setSpeed(viz, speedSeconds) {
  viz?.style.setProperty("--speed", `${clamp(speedSeconds, 0.45, 4).toFixed(2)}s`);
}

function dayKeyFromMs(ms) {
  const d = new Date(ms);
  const y = d.getFullYear();
  const m = String(d.getMonth() + 1).padStart(2, "0");
  const day = String(d.getDate()).padStart(2, "0");
  return `${y}-${m}-${day}`;
}

function startOfDayMs(ms) {
  const d = new Date(ms);
  d.setHours(0, 0, 0, 0);
  return d.getTime();
}

function ensureMobilityDay(key) {
  if (!mobilityState.days[key]) {
    mobilityState.days[key] = { walkingMs: 0, stationaryMs: 0 };
  }
}

function loadMobilityState() {
  try {
    const raw = localStorage.getItem("mobilityStateV1");
    if (!raw) {
      return;
    }
    const parsed = JSON.parse(raw);
    if (parsed && parsed.days && typeof parsed.days === "object") {
      mobilityState.days = parsed.days;
    }
  } catch {
    mobilityState.days = {};
  }
}

function saveMobilityState() {
  try {
    localStorage.setItem("mobilityStateV1", JSON.stringify({ days: mobilityState.days }));
  } catch {
    // Ignore storage errors.
  }
}

function addDurationToMobility(startMs, endMs, mode) {
  let cursor = startMs;
  while (cursor < endMs) {
    const nextDayMs = startOfDayMs(cursor) + 24 * 3600000;
    const chunkEnd = Math.min(endMs, nextDayMs);
    const key = dayKeyFromMs(cursor);
    ensureMobilityDay(key);
    if (mode === "Walking") {
      mobilityState.days[key].walkingMs += chunkEnd - cursor;
    } else {
      mobilityState.days[key].stationaryMs += chunkEnd - cursor;
    }
    cursor = chunkEnd;
  }
}

function getDayData(key) {
  return mobilityState.days[key] || { walkingMs: 0, stationaryMs: 0 };
}

function getPastDayKeys(daysCount) {
  const keys = [];
  const todayStart = startOfDayMs(Date.now());
  for (let i = daysCount - 1; i >= 0; i -= 1) {
    keys.push(dayKeyFromMs(todayStart - i * 24 * 3600000));
  }
  return keys;
}

function weekdayFromKey(key) {
  const [y, m, d] = key.split("-").map(Number);
  return new Date(y, m - 1, d).toLocaleDateString(undefined, { weekday: "short" });
}

function ensureEnvDay(key) {
  if (!envStressState.days[key]) {
    envStressState.days[key] = {
      exposureMs: { cold: 0, ideal: 0, hot: 0 },
      events: { cold: 0, ideal: 0, hot: 0 }
    };
  }
}

function roomStateFromTemp(tempC) {
  if (tempC < 20) {
    return "cold";
  }
  if (tempC <= 26) {
    return "ideal";
  }
  return "hot";
}

function addExposureToEnvState(startMs, endMs, state) {
  let cursor = startMs;
  while (cursor < endMs) {
    const nextDayMs = startOfDayMs(cursor) + 24 * 3600000;
    const chunkEnd = Math.min(endMs, nextDayMs);
    const key = dayKeyFromMs(cursor);
    ensureEnvDay(key);
    envStressState.days[key].exposureMs[state] += chunkEnd - cursor;
    cursor = chunkEnd;
  }
}

function loadEnvStressState() {
  try {
    const raw = localStorage.getItem("envStressStateV1");
    if (!raw) {
      return;
    }
    const parsed = JSON.parse(raw);
    if (parsed && parsed.days && typeof parsed.days === "object") {
      envStressState.days = parsed.days;
    }
  } catch {
    envStressState.days = {};
  }
}

function saveEnvStressState() {
  try {
    localStorage.setItem("envStressStateV1", JSON.stringify({ days: envStressState.days }));
  } catch {
    // Ignore storage errors.
  }
}

function updateEnvStressTracking(tempC, isStressEvent, nowMs) {
  const currentTempState = roomStateFromTemp(tempC);
  if (nowMs > envStressState.lastMs) {
    addExposureToEnvState(envStressState.lastMs, nowMs, envStressState.lastTempState);
  }

  if (isStressEvent && !envStressState.stressActive) {
    const key = dayKeyFromMs(nowMs);
    ensureEnvDay(key);
    envStressState.days[key].events[currentTempState] += 1;
  }

  envStressState.stressActive = isStressEvent;
  envStressState.lastMs = nowMs;
  envStressState.lastTempState = currentTempState;
  saveEnvStressState();
  renderEnvStress();
}

function formatTimeAgo(ms) {
  if (!ms) {
    return "--";
  }
  const diff = Date.now() - ms;
  if (diff < 60000) {
    return `${Math.round(diff / 1000)}s ago`;
  }
  if (diff < 3600000) {
    return `${Math.round(diff / 60000)}m ago`;
  }
  return new Date(ms).toLocaleTimeString();
}

function updateAgitationTracking(heartBpm, isStationary, nowMs) {
  const elapsedMs = Math.max(0, nowMs - agitationState.lastMs);
  agitationState.lastMs = nowMs;

  if (!isStationary) {
    agitationState.sustainMs = 0;
    agitationState.active = false;
    renderAgitation(heartBpm, 0, "Moving");
    return;
  }

  const alpha = 0.06;
  if (agitationState.baselineBpm == null) {
    agitationState.baselineBpm = heartBpm;
  } else {
    agitationState.baselineBpm = agitationState.baselineBpm + alpha * (heartBpm - agitationState.baselineBpm);
  }

  const delta = heartBpm - agitationState.baselineBpm;
  const threshold = 12;
  const sustainTargetMs = 20000;

  if (delta >= threshold) {
    agitationState.sustainMs += elapsedMs;
  } else {
    agitationState.sustainMs = Math.max(0, agitationState.sustainMs - elapsedMs * 1.4);
  }

  if (!agitationState.active && agitationState.sustainMs >= sustainTargetMs) {
    agitationState.active = true;
    agitationState.events += 1;
    agitationState.lastEventMs = nowMs;
  }

  if (agitationState.active && delta < threshold - 4) {
    agitationState.active = false;
  }

  renderAgitation(heartBpm, delta, agitationState.active ? "Agitated" : "Stable");
}

function renderAgitation(heartBpm, delta, status) {
  if (!agitationStatusEl) {
    return;
  }
  agitationStatusEl.textContent = status;
  baselineBpmEl.textContent = agitationState.baselineBpm ? `${agitationState.baselineBpm.toFixed(0)} BPM` : "--";
  agitationDeltaEl.textContent = `${delta.toFixed(0)} BPM`;
  agitationSustainEl.textContent = `${Math.round(agitationState.sustainMs / 1000)}s`;
  agitationEventsEl.textContent = String(agitationState.events);
  agitationLastEl.textContent = formatTimeAgo(agitationState.lastEventMs);

  agitationSummaryEl.classList.toggle("alert", agitationState.active);
  if (status === "Moving") {
    agitationSummaryEl.textContent = "Patient is moving. Agitation tracking paused until stationary.";
  } else if (agitationState.active) {
    agitationSummaryEl.textContent = "Agitation detected: sustained heart rate increase while stationary.";
  } else {
    agitationSummaryEl.textContent = "Tracking sustained heart rate increases while stationary. No agitation detected.";
  }
}

function renderEnvStress() {
  const weekKeys = getPastDayKeys(7);
  const totals = weekKeys.reduce((acc, key) => {
    const day = envStressState.days[key];
    if (!day) {
      return acc;
    }
    acc.exposure.cold += day.exposureMs.cold || 0;
    acc.exposure.ideal += day.exposureMs.ideal || 0;
    acc.exposure.hot += day.exposureMs.hot || 0;
    acc.events.cold += day.events.cold || 0;
    acc.events.ideal += day.events.ideal || 0;
    acc.events.hot += day.events.hot || 0;
    return acc;
  }, {
    exposure: { cold: 0, ideal: 0, hot: 0 },
    events: { cold: 0, ideal: 0, hot: 0 }
  });

  const risk = (state) => {
    const expHours = totals.exposure[state] / 3600000;
    if (expHours <= 0) {
      return 0;
    }
    return (totals.events[state] / expHours) * 100;
  };

  const coldRisk = risk("cold");
  const idealRisk = risk("ideal");
  const hotRisk = risk("hot");

  coldRiskEl.textContent = `${coldRisk.toFixed(1)}%`;
  idealRiskEl.textContent = `${idealRisk.toFixed(1)}%`;
  hotRiskEl.textContent = `${hotRisk.toFixed(1)}%`;

  coldEventsEl.textContent = `${totals.events.cold} events across ${(totals.exposure.cold / 3600000).toFixed(1)}h`;
  idealEventsEl.textContent = `${totals.events.ideal} events across ${(totals.exposure.ideal / 3600000).toFixed(1)}h`;
  hotEventsEl.textContent = `${totals.events.hot} events across ${(totals.exposure.hot / 3600000).toFixed(1)}h`;

  envSummaryEl.classList.remove("alert");
  if (totals.exposure.cold <= 0 && totals.exposure.ideal <= 0 && totals.exposure.hot <= 0) {
    envSummaryEl.textContent = "Collecting data to correlate falls or high heart rate with room temperature states.";
    return;
  }

  if (coldRisk > idealRisk && (totals.events.cold > 0 || totals.events.ideal > 0)) {
    envSummaryEl.classList.add("alert");
    envSummaryEl.textContent = `Cold-state stress risk is higher than ideal-state risk (${coldRisk.toFixed(1)}% vs ${idealRisk.toFixed(1)}%). Consider keeping room temperature in the ideal range (20-26 deg C) to reduce physical stress.`;
    return;
  }

  envSummaryEl.textContent = `No cold-related stress spike detected. Current 7-day risk: cold ${coldRisk.toFixed(1)}%, ideal ${idealRisk.toFixed(1)}%, hot ${hotRisk.toFixed(1)}%.`;
}

function walkingPercent(day) {
  const total = day.walkingMs + day.stationaryMs;
  return total > 0 ? (day.walkingMs / total) * 100 : 0;
}

function updateMobilityTracking(mode, nowMs) {
  const elapsedStart = mobilityState.lastMs;
  const elapsedEnd = nowMs;
  if (elapsedEnd > elapsedStart) {
    addDurationToMobility(elapsedStart, elapsedEnd, mobilityState.lastMode);
  }
  mobilityState.lastMs = nowMs;
  mobilityState.lastMode = mode;
  saveMobilityState();
  renderMobility();
}

function renderMobility() {
  const todayKey = dayKeyFromMs(Date.now());
  const today = getDayData(todayKey);
  const totalToday = today.walkingMs + today.stationaryMs;
  const walkPct = walkingPercent(today);
  const stationaryPct = totalToday > 0 ? (today.stationaryMs / totalToday) * 100 : 0;
  const ratio = today.stationaryMs > 0 ? today.walkingMs / today.stationaryMs : (today.walkingMs > 0 ? Infinity : 0);

  walkingPctEl.textContent = `${walkPct.toFixed(1)}%`;
  stationaryPctEl.textContent = `${stationaryPct.toFixed(1)}%`;
  walkStationaryRatioEl.textContent = Number.isFinite(ratio) ? ratio.toFixed(2) : "Inf";

  const weekKeys = getPastDayKeys(7);
  mobilityWeekEl.innerHTML = weekKeys.map((key) => {
    const day = getDayData(key);
    const pct = walkingPercent(day);
    const dayLabel = weekdayFromKey(key);
    return `<div class="week-day"><p class="week-day-name">${dayLabel}</p><div class="week-bar"><div class="week-bar-fill" style="height:${pct.toFixed(1)}%"></div></div><p class="week-day-pct">${pct.toFixed(0)}%</p></div>`;
  }).join("");

  const twoWeekKeys = getPastDayKeys(14);
  const prevWeekKeys = twoWeekKeys.slice(0, 7);
  const currWeekKeys = twoWeekKeys.slice(7);

  const aggregate = (keys) => keys.reduce((acc, key) => {
    const day = getDayData(key);
    acc.walking += day.walkingMs;
    acc.total += day.walkingMs + day.stationaryMs;
    return acc;
  }, { walking: 0, total: 0 });

  const prevAgg = aggregate(prevWeekKeys);
  const currAgg = aggregate(currWeekKeys);
  const prevPct = prevAgg.total > 0 ? (prevAgg.walking / prevAgg.total) * 100 : 0;
  const currPct = currAgg.total > 0 ? (currAgg.walking / currAgg.total) * 100 : 0;
  const dropPct = prevPct > 0 ? ((prevPct - currPct) / prevPct) * 100 : 0;

  if (dropPct > 20) {
    mobilityWarningEl.classList.add("alert");
    mobilityWarningEl.textContent = `Mobility Warning: walking activity dropped ${dropPct.toFixed(1)}% this week (from ${prevPct.toFixed(1)}% to ${currPct.toFixed(1)}%).`;
  } else if (prevPct === 0 && currPct === 0) {
    mobilityWarningEl.classList.remove("alert");
    mobilityWarningEl.textContent = "Mobility trend needs more data to compare weekly decline.";
  } else {
    mobilityWarningEl.classList.remove("alert");
    mobilityWarningEl.textContent = `Mobility stable: weekly walking changed from ${prevPct.toFixed(1)}% to ${currPct.toFixed(1)}%.`;
  }
}

function setupTabs() {
  tabButtons.forEach((button) => {
    button.addEventListener("click", () => {
      const targetId = button.dataset.tab;
      tabButtons.forEach((btn) => btn.classList.toggle("active", btn === button));
      tabPanels.forEach((panel) => panel.classList.toggle("active", panel.id === targetId));
      if (targetId === "circadianTab") {
        drawSleepChart();
      }
    });
  });
}

function updateSleepTracking(mode, nowMs) {
  const elapsedMs = Math.max(0, nowMs - sleepState.lastMs);
  if (sleepState.lastMode === "Resting") {
    sleepState.restingMs += elapsedMs;
  } else {
    sleepState.activeMs += elapsedMs;
  }

  sleepState.lastMs = nowMs;
  const hoursFromStart = (nowMs - sleepState.startMs) / 3600000;
  const level = mode === "Resting" ? -1 : 1;
  sleepState.lastMode = mode;
  sleepState.history.push({ hours: hoursFromStart, level });
  if (sleepState.history.length > 900) {
    sleepState.history.shift();
  }

  const totalSleepHours = (sleepState.restingMs - sleepState.activeMs) / 3600000;
  totalSleepHoursEl.textContent = `${totalSleepHours.toFixed(2)} hrs`;
  sleepFormulaEl.textContent = `${(sleepState.restingMs / 3600000).toFixed(2)}h resting - ${(sleepState.activeMs / 3600000).toFixed(2)}h active`;
  drawSleepChart();
}

function drawSleepChart() {
  if (!sleepCanvas || !sleepCtx) {
    return;
  }

  const dpr = window.devicePixelRatio || 1;
  const rect = sleepCanvas.getBoundingClientRect();
  const width = Math.max(320, Math.floor(rect.width));
  const height = 320;

  sleepCanvas.width = Math.floor(width * dpr);
  sleepCanvas.height = Math.floor(height * dpr);
  sleepCtx.setTransform(dpr, 0, 0, dpr, 0, 0);

  const pad = { top: 24, right: 20, bottom: 52, left: 52 };
  const plotW = width - pad.left - pad.right;
  const plotH = height - pad.top - pad.bottom;
  const xMin = 0;
  const xMax = Math.max(1, sleepState.history[sleepState.history.length - 1].hours);
  const yMin = -1;
  const yMax = 1;

  const xToPx = (x) => pad.left + ((x - xMin) / (xMax - xMin)) * plotW;
  const yToPx = (y) => pad.top + ((yMax - y) / (yMax - yMin)) * plotH;

  sleepCtx.clearRect(0, 0, width, height);
  sleepCtx.fillStyle = "rgba(255,255,255,0.85)";
  sleepCtx.fillRect(0, 0, width, height);

  sleepCtx.strokeStyle = "rgba(20,32,37,0.18)";
  sleepCtx.lineWidth = 1;
  sleepCtx.beginPath();
  sleepCtx.moveTo(pad.left, pad.top);
  sleepCtx.lineTo(pad.left, pad.top + plotH);
  sleepCtx.lineTo(pad.left + plotW, pad.top + plotH);
  sleepCtx.stroke();

  const midY = yToPx(0);
  sleepCtx.setLineDash([5, 5]);
  sleepCtx.beginPath();
  sleepCtx.moveTo(pad.left, midY);
  sleepCtx.lineTo(pad.left + plotW, midY);
  sleepCtx.stroke();
  sleepCtx.setLineDash([]);

  sleepCtx.font = "700 12px Manrope, sans-serif";
  sleepCtx.fillStyle = "#4b626b";
  sleepCtx.fillText("Active", 10, yToPx(1) + 4);
  sleepCtx.fillText("Resting", 10, yToPx(-1) + 4);
  sleepCtx.fillText("Activity", 10, 16);

  sleepCtx.fillText("Bed Time (hours)", pad.left + plotW / 2 - 44, height - 16);
  sleepCtx.fillText("0", xToPx(0) - 2, height - 32);
  sleepCtx.fillText((xMax / 2).toFixed(1), xToPx(xMax / 2) - 9, height - 32);
  sleepCtx.fillText(xMax.toFixed(1), xToPx(xMax) - 9, height - 32);

  const points = sleepState.history;
  if (!points.length) {
    return;
  }

  sleepCtx.lineWidth = 3;
  sleepCtx.strokeStyle = "#0b8a83";
  sleepCtx.beginPath();
  sleepCtx.moveTo(xToPx(points[0].hours), yToPx(points[0].level));
  for (let i = 1; i < points.length; i += 1) {
    sleepCtx.lineTo(xToPx(points[i].hours), yToPx(points[i].level));
  }
  sleepCtx.stroke();

  const last = points[points.length - 1];
  sleepCtx.fillStyle = last.level > 0 ? "#0b8a83" : "#de8d38";
  sleepCtx.beginPath();
  sleepCtx.arc(xToPx(last.hours), yToPx(last.level), 5, 0, Math.PI * 2);
  sleepCtx.fill();
}

function applyVisuals(metrics) {
  const heartSpeed = 2.1 - ((metrics.heart - 55) / 70) * 1.5;
  setSpeed(el.heartViz, heartSpeed);

  const patientMoving = metrics.patientMotion === "Walking";
  setSpeed(el.patientMotionViz, patientMoving ? 1.0 : 3.0);

  const envLevel = metrics.envStatus?.includes("Cold") ? 30 : (metrics.envStatus?.includes("Hot") ? 90 : 70);
  setMeter(el.envStatusViz, envLevel, 2.0);

  const nearMoving = metrics.nearMotion === "Moving";
  setSpeed(el.nearMotionViz, nearMoving ? 1.1 : 3.2);
  setSpeed(el.pirViz, nearMoving ? 1.2 : 3.4);

  setMeter(el.bodyTempViz, ((metrics.bodyTemp - 35) / 5) * 100, 2.3 - ((metrics.bodyTemp - 35) / 5) * 1.4);
  setMeter(el.envTempViz, ((metrics.envTemp - 15) / 20) * 100, 2.2 - ((metrics.envTemp - 15) / 20) * 1.2);
  setMeter(el.humidityViz, metrics.humidity, 2.6 - (metrics.humidity / 100) * 1.8);
  setMeter(el.objectViz, (1 - metrics.distance / 3) * 100, 2.4 - (1 - metrics.distance / 3) * 1.4);
  setSpeed(el.irViz, 2.4 - (metrics.heartIr / 100000) * 1.2);

  setSpeed(el.rfidViz, metrics.rfidChanged ? 0.85 : 1.8);

  if (metrics.isFall) {
    el.fallViz?.classList.add("alert");
    setSpeed(el.fallViz, 0.7);
  } else {
    el.fallViz?.classList.remove("alert");
    setSpeed(el.fallViz, 1.7);
  }
}

let lastRfidValue = "";

async function updateDashboard() {
  try {
    // Fetch real JSON data from the ESP32
    const response = await fetch('/data');
    const data = await response.json();

    const heart = toNumber(data.heart ?? data.heartRate, randomBetween(68, 98));
    const bodyTemp = toNumber(data.bodyTemp ?? data.body_temp ?? data.temp, randomBetween(36.2, 37.6, 1));
    const envTemp = toNumber(data.envTemp ?? data.environmentTemp ?? data.temp, randomBetween(22, 29, 1));
    const humidity = toNumber(data.hum ?? data.humidity, randomBetween(40, 65));
    const distance = toNumber(data.dist ?? data.distance, randomBetween(0.4, 2.3, 1));
    const patientMotion = String(data.motion ?? "Resting").includes("Walk") ? "Walking" : "Resting";
    const nearMotion = String(data.nearMotion ?? data.near_motion ?? data.near ?? (patientMotion === "Walking" ? "Moving" : "Stationary"));
    const primaryRfid = String(data.rfid ?? data.rfid1 ?? "A7-91-2C-FF");
    const secondaryRfid = String(data.rfid2 ?? "B4-20-9D-61");
    const envStatus = String(data.env ?? data.envStatus ?? "Ideal State");
    const heartIr = toNumber(data.ir ?? data.heartIr ?? 0, 0);

    el.heartRate.textContent = String(Math.round(heart));
    el.patientMotion.textContent = patientMotion;
    el.envStatus.textContent = envStatus;
    el.bodyTemp.textContent = bodyTemp.toFixed(1);
    el.envTemp.textContent = envTemp.toFixed(1);
    el.humidity.textContent = String(Math.round(humidity));
    el.pirMotion.textContent = nearMotion;
    el.rfid1.textContent = primaryRfid;
    el.rfid2.textContent = secondaryRfid;
    el.heartIr.textContent = String(Math.round(heartIr));

    const nearestName = data.nearestObject ?? data.object ?? objects[Math.floor(Math.random() * objects.length)];
    el.nearestObject.textContent = `${nearestName} (${distance.toFixed(1)} m)`;
    el.nearMotion.textContent = nearMotion.includes("Moving") ? "Moving" : "Stationary";

    // Logic for the Fall Alert Card
    if (data.motion === "!!! FALL !!!") {
      el.fallAlert.textContent = "Emergency: Fall Detected";
      el.fallCard.classList.add("alert");
    } else {
      el.fallAlert.textContent = "No Fall Detected";
      el.fallCard.classList.remove("alert");
    }

    applyVisuals({
      heart,
      patientMotion,
      envStatus,
      bodyTemp,
      envTemp,
      humidity,
      distance,
      heartIr,
      nearMotion: el.nearMotion.textContent,
      isFall: data.motion === "!!! FALL !!!",
      rfidChanged: lastRfidValue && lastRfidValue !== primaryRfid
    });
    lastRfidValue = primaryRfid;
    const isActiveState = data.motion === "!!! FALL !!!" || patientMotion === "Walking";
    updateSleepTracking(isActiveState ? "Active" : "Resting", Date.now());
    updateMobilityTracking(patientMotion === "Walking" ? "Walking" : "Stationary", Date.now());
    const highHeart = heart >= 100;
    updateEnvStressTracking(envTemp, data.motion === "!!! FALL !!!" || highHeart, Date.now());
    updateAgitationTracking(heart, patientMotion !== "Walking", Date.now());

    const now = new Date();
    el.lastUpdated.textContent = `Updated: ${now.toLocaleTimeString()}`;
  } catch (err) {
    console.error("Dashboard update failed:", err);
  }
}

// Keep the interval but reduce it to 2 seconds for real-time monitoring
setInterval(updateDashboard, 2000);

loadMobilityState();
renderMobility();
loadEnvStressState();
renderEnvStress();
setupTabs();
drawSleepChart();
updateDashboard();

)rawliteral";

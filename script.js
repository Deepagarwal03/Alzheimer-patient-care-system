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


const tabButtons = Array.from(document.querySelectorAll(".tab-btn"));
const tabPanels = Array.from(document.querySelectorAll(".tab-panel"));
const sleepCanvas = document.getElementById("sleepChart");
const sleepCtx = sleepCanvas?.getContext("2d");
const totalSleepHoursEl = document.getElementById("totalSleepHours");
const sleepFormulaEl = document.getElementById("sleepFormula");
const sleepRestHoursEl = document.getElementById("sleepRestHours");
const sleepActiveHoursEl = document.getElementById("sleepActiveHours");
const sleepAiSummaryEl = document.getElementById("sleepAiSummary");
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
const baselineIrEl = document.getElementById("baselineIr");
const agitationIrDeltaEl = document.getElementById("agitationIrDelta");
const agitationSustainEl = document.getElementById("agitationSustain");
const agitationEventsEl = document.getElementById("agitationEvents");
const agitationLastEl = document.getElementById("agitationLast");
const agitationSummaryEl = document.getElementById("agitationSummary");
const hrInsightStateEl = document.getElementById("hrInsightState");
const hrSignalQualityEl = document.getElementById("hrSignalQuality");
const hrAvgBpmEl = document.getElementById("hrAvgBpm");
const hrAvgIrEl = document.getElementById("hrAvgIr");
const hrInsightTipEl = document.getElementById("hrInsightTip");
const hrInsightSummaryEl = document.getElementById("hrInsightSummary");
const hrBaselineEl = document.getElementById("hrBaseline");
const hrChangeEl = document.getElementById("hrChange");
const hrIrChangeEl = document.getElementById("hrIrChange");
const hrInsightAlertEl = document.getElementById("hrInsightAlert");
const cnnPatientStateEl = document.getElementById("cnnPatientState");
const cnnConfidenceEl = document.getElementById("cnnConfidence");
const cnnWindowSizeEl = document.getElementById("cnnWindowSize");
const cnnModelStatusEl = document.getElementById("cnnModelStatus");
const cnnSummaryEl = document.getElementById("cnnSummary");
const analyticsServiceStatusEl = document.getElementById("analyticsServiceStatus");
const analyticsRawRowsEl = document.getElementById("analyticsRawRows");
const analyticsProcessedRowsEl = document.getElementById("analyticsProcessedRows");
const analyticsLastTrainEl = document.getElementById("analyticsLastTrain");
const analyticsStatusNoteEl = document.getElementById("analyticsStatusNote");
const predictionHistoryEl = document.getElementById("predictionHistory");
const rfFeedbackEl = document.getElementById("rfFeedback");
const rfConfidenceEl = document.getElementById("rfConfidence");
const alertLevelEl = document.getElementById("alertLevel");
const recommendedActionEl = document.getElementById("recommendedAction");
const riskFactorsEl = document.getElementById("riskFactors");
const rfSummaryEl = document.getElementById("rfSummary");
const rawLogPathEl = document.getElementById("rawLogPath");
const processedLogPathEl = document.getElementById("processedLogPath");
const lastInferenceEl = document.getElementById("lastInference");

const analyticsApiBase = window.localStorage.getItem("analyticsApiBase") || "http://127.0.0.1:5001/api";

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
  baselineIr: null,
  sustainMs: 0,
  lastMs: Date.now(),
  active: false,
  events: 0,
  lastEventMs: null
};

const heartInsightState = {
  samples: []
};

const analyticsState = {
  available: false
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

function formatDateTime(value) {
  if (!value) {
    return "--";
  }
  const dt = new Date(value);
  return Number.isNaN(dt.getTime()) ? String(value) : dt.toLocaleString();
}

function setStatusPill(node, text, tone) {
  if (!node) {
    return;
  }
  node.textContent = text;
  node.classList.remove("good", "warn", "danger");
  if (tone) {
    node.classList.add(tone);
  }
}

function average(values) {
  if (!values.length) {
    return 0;
  }
  return values.reduce((sum, value) => sum + value, 0) / values.length;
}

function updateSleepSuggestions(restHours, activeHours, mode) {
  if (!sleepAiSummaryEl) {
    return;
  }

  sleepRestHoursEl.textContent = `${restHours.toFixed(2)} hrs`;
  sleepActiveHoursEl.textContent = `${activeHours.toFixed(2)} hrs`;
  sleepAiSummaryEl.classList.remove("alert");

  let summary = "Balanced rest and activity trend. Keep the current routine and sleep tight.";

  if (restHours < 7 && activeHours < 2) {
    summary = "Sleep more and add short supervised activity blocks. Rest is below 7 hours and movement is below 2 hours.";
    sleepAiSummaryEl.classList.add("alert");
  } else if (restHours < 7) {
    summary = "Sleep more. The patient is still below the 7-hour sleep goal, so keep the room calm and encourage an earlier rest period.";
    sleepAiSummaryEl.classList.add("alert");
  } else if (restHours >= 7 && restHours <= 9 && activeHours >= 2 && activeHours <= 5) {
    summary = "Sleep tight. Rest and daily activity are in a healthy range, so the current rhythm looks well balanced.";
  } else if (restHours > 9.5 && activeHours < 2) {
    summary = "Sleep hours are increasing steadily while activity is low. Sleep tight, but also encourage gentle daytime movement to avoid over-resting.";
  } else if (activeHours < 2) {
    summary = "Be more active. Add short walks, stretching, or chair exercises so daytime activity reaches at least 2 hours.";
    sleepAiSummaryEl.classList.add("alert");
  } else if (activeHours > 6 && restHours < 6.5) {
    summary = "Activity is high while sleep is low. Encourage recovery, hydration, and an earlier bedtime tonight.";
    sleepAiSummaryEl.classList.add("alert");
  } else if (mode === "Resting" && restHours >= 7) {
    summary = "Sleep tight. The patient is already above the minimum sleep target and is currently in a resting phase.";
  }

  sleepAiSummaryEl.textContent = summary;
}

function updateHeartInsight(heartBpm, heartIr, patientMotion, nowMs) {
  if (!hrInsightStateEl) {
    return;
  }

  heartInsightState.samples.push({ heartBpm, heartIr, patientMotion, nowMs });
  if (heartInsightState.samples.length > 90) {
    heartInsightState.samples.shift();
  }

  const samples = heartInsightState.samples;
  const recent = samples.slice(-12);
  const validHeart = recent.filter((sample) => sample.heartBpm > 0).map((sample) => sample.heartBpm);
  const validIr = recent.filter((sample) => sample.heartIr > 0).map((sample) => sample.heartIr);
  const baselineHeartSource = samples.slice(Math.max(0, samples.length - 30), Math.max(0, samples.length - 5)).filter((sample) => sample.heartBpm > 0).map((sample) => sample.heartBpm);
  const baselineIrSource = samples.slice(Math.max(0, samples.length - 30), Math.max(0, samples.length - 5)).filter((sample) => sample.heartIr > 0).map((sample) => sample.heartIr);
  const avgHeart = average(validHeart);
  const avgIr = average(validIr);
  const baselineHeart = average(baselineHeartSource.length ? baselineHeartSource : validHeart);
  const baselineIr = average(baselineIrSource.length ? baselineIrSource : validIr);
  const heartChange = heartBpm > 0 && baselineHeart > 0 ? heartBpm - baselineHeart : 0;
  const irChange = heartIr > 0 && baselineIr > 0 ? heartIr - baselineIr : 0;
  const spikeVotes = recent.filter((sample) => sample.heartBpm > 0 && baselineHeart > 0 && sample.heartBpm >= baselineHeart + 12).length;
  const dropVotes = recent.filter((sample) => sample.heartBpm > 0 && baselineHeart > 0 && sample.heartBpm <= Math.max(40, baselineHeart - 12)).length;

  let signalQuality = "No Contact";
  if (heartIr >= 85000) {
    signalQuality = "Strong";
  } else if (heartIr >= 55000) {
    signalQuality = "Good";
  } else if (heartIr >= 30000) {
    signalQuality = "Weak";
  }

  let trendState = "Adaptive Monitoring";
  let tip = "Keep watching the trend while the model gathers more heart and IR context.";
  let summary = "The heart insight engine is using recent BPM and IR windows to watch for spike, drop, recovery, and signal-quality changes.";
  let alertTone = "good";
  let alertText = "Stable";

  if (heartIr < 20000) {
    trendState = "Signal Check";
    tip = "Heart sensor IR is very low. Reposition the finger and reduce movement for a cleaner reading.";
    summary = "The IR signal is weak, so any BPM spike or drop may be unreliable until sensor contact improves.";
    alertTone = "warn";
    alertText = "Check Sensor";
  } else if ((heartBpm > 0 && heartBpm >= 118) || spikeVotes >= 3 || heartChange >= 18) {
    trendState = "HR Spike Watch";
    tip = "Heart rate is running above the recent baseline. Slow activity, hydrate, and recheck within 1 to 2 minutes.";
    summary = `Recent BPM is elevated against baseline by ${heartChange.toFixed(0)} BPM, so the model is watching for a sustained spike.`;
    alertTone = heartBpm >= 130 ? "danger" : "warn";
    alertText = heartBpm >= 130 ? "Urgent" : "Observe";
  } else if ((heartBpm > 0 && heartBpm <= 50 && baselineHeart >= 60) || dropVotes >= 3) {
    trendState = "HR Drop Watch";
    tip = "Heart rate is below the recent baseline. Check posture, comfort, and sensor placement, then repeat the reading.";
    summary = `The recent trend is below baseline by ${Math.abs(heartChange).toFixed(0)} BPM, so the model is watching for a persistent drop.`;
    alertTone = heartBpm > 0 && heartBpm < 45 ? "danger" : "warn";
    alertText = heartBpm > 0 && heartBpm < 45 ? "Urgent" : "Observe";
  } else if (patientMotion === "Resting" && heartBpm > 0 && heartBpm >= 60 && heartBpm <= 95 && heartIr >= 50000) {
    trendState = "Stable Recovery";
    tip = "Heart rate and IR look steady during rest. Maintain comfort and continue monitoring.";
    summary = "The model sees a stable recovery pattern with steady BPM and good IR signal quality during rest.";
  } else if (patientMotion === "Walking" && heartBpm > 0) {
    trendState = "Active Response";
    tip = "The heart signal reflects activity. Watch for recovery after movement and offer hydration if needed.";
    summary = "The recent BPM and IR pattern matches an active-movement response rather than a concerning spike.";
  }

  hrInsightStateEl.textContent = trendState;
  hrSignalQualityEl.textContent = signalQuality;
  hrAvgBpmEl.textContent = validHeart.length ? `${avgHeart.toFixed(0)} BPM` : "--";
  hrAvgIrEl.textContent = validIr.length ? `${avgIr.toFixed(0)}` : "--";
  hrInsightTipEl.textContent = tip;
  hrInsightSummaryEl.textContent = summary;
  hrBaselineEl.textContent = baselineHeart > 0 ? `${baselineHeart.toFixed(0)} BPM` : "--";
  hrChangeEl.textContent = heartBpm > 0 && baselineHeart > 0 ? `${heartChange >= 0 ? "+" : ""}${heartChange.toFixed(0)} BPM` : "--";
  hrIrChangeEl.textContent = heartIr > 0 && baselineIr > 0 ? `${irChange >= 0 ? "+" : ""}${irChange.toFixed(0)}` : "--";
  setStatusPill(hrInsightAlertEl, alertText, alertTone);
}

function updateAgitationTracking(heartBpm, heartIr, isStationary, nowMs) {
  const elapsedMs = Math.max(0, nowMs - agitationState.lastMs);
  agitationState.lastMs = nowMs;

  if (!isStationary) {
    agitationState.sustainMs = 0;
    agitationState.active = false;
    renderAgitation(heartIr, 0, "Moving");
    return;
  }

  const alpha = 0.06;
  if (heartBpm > 0) {
    if (agitationState.baselineBpm == null) {
      agitationState.baselineBpm = heartBpm;
    } else {
      agitationState.baselineBpm = agitationState.baselineBpm + alpha * (heartBpm - agitationState.baselineBpm);
    }
  }
  if (heartIr > 0) {
    if (agitationState.baselineIr == null) {
      agitationState.baselineIr = heartIr;
    } else {
      agitationState.baselineIr = agitationState.baselineIr + alpha * (heartIr - agitationState.baselineIr);
    }
  }

  const delta = agitationState.baselineBpm == null ? 0 : heartBpm - agitationState.baselineBpm;
  const irDelta = agitationState.baselineIr == null ? 0 : heartIr - agitationState.baselineIr;
  const threshold = 12;
  const sustainTargetMs = 20000;

  const bpmTrigger = heartBpm > 0 && delta >= threshold;
  const irTrigger = heartIr > 0 && agitationState.baselineIr != null && Math.abs(irDelta) >= 12000;

  if (bpmTrigger || irTrigger) {
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

  renderAgitation(heartIr, irDelta, agitationState.active ? "Agitated" : "Stable");
}

function renderAgitation(heartIr, irDelta, status) {
  if (!agitationStatusEl) {
    return;
  }
  agitationStatusEl.textContent = status;
  baselineIrEl.textContent = agitationState.baselineIr ? `${agitationState.baselineIr.toFixed(0)}` : "--";
  agitationIrDeltaEl.textContent = `${irDelta >= 0 ? "+" : ""}${irDelta.toFixed(0)}`;
  agitationSustainEl.textContent = `${Math.round(agitationState.sustainMs / 1000)}s`;
  agitationEventsEl.textContent = String(agitationState.events);
  agitationLastEl.textContent = formatTimeAgo(agitationState.lastEventMs);

  agitationSummaryEl.classList.toggle("alert", agitationState.active);
  if (status === "Moving") {
    agitationSummaryEl.textContent = "Patient is moving. Agitation tracking pauses until the patient is stationary again.";
  } else if (agitationState.active) {
    agitationSummaryEl.textContent = "Agitation detected: sustained heart and IR change while stationary.";
  } else {
    agitationSummaryEl.textContent = "Tracking sustained heart and IR changes while stationary. No agitation detected.";
  }
}

function renderPredictionHistory(items) {
  if (!predictionHistoryEl) {
    return;
  }
  if (!items || !items.length) {
    predictionHistoryEl.innerHTML = `
      <div class="history-row empty">
        <span>No processed history yet.</span>
        <span>--</span>
        <span>--</span>
        <span>--</span>
      </div>
    `;
    return;
  }

  predictionHistoryEl.innerHTML = items.map((item) => `
    <div class="history-row">
      <span>${formatDateTime(item.created_at_iso)}</span>
      <span>${item.patient_state ?? "--"}</span>
      <span>${item.alert_level ?? "--"}</span>
      <span>${item.comfort_feedback ?? "--"}</span>
    </div>
  `).join("");
}

function renderAnalyticsUnavailable(message) {
  analyticsState.available = false;
  cnnPatientStateEl.textContent = "Offline";
  cnnConfidenceEl.textContent = "--";
  cnnWindowSizeEl.textContent = "--";
  setStatusPill(cnnModelStatusEl, "Offline", "warn");
  cnnSummaryEl.textContent = message;
  setStatusPill(analyticsServiceStatusEl, "Disconnected", "warn");
  analyticsRawRowsEl.textContent = "0";
  analyticsProcessedRowsEl.textContent = "0";
  analyticsLastTrainEl.textContent = "--";
  analyticsStatusNoteEl.textContent = message;
  rfFeedbackEl.textContent = "Offline";
  rfConfidenceEl.textContent = "--";
  setStatusPill(alertLevelEl, "Stable", "good");
  recommendedActionEl.textContent = "Start the local analytics service to receive model feedback and saved CSV output.";
  riskFactorsEl.textContent = "No processed risks yet.";
  rfSummaryEl.textContent = "The Random Forest output will appear here once the local analytics service is running.";
  lastInferenceEl.textContent = "--";
  hrInsightStateEl.textContent = "Monitoring";
  hrSignalQualityEl.textContent = "--";
  hrAvgBpmEl.textContent = "--";
  hrAvgIrEl.textContent = "--";
  hrInsightTipEl.textContent = "Collecting enough heart and IR samples for insight.";
  hrInsightSummaryEl.textContent = "This tab watches recent BPM and HR sensor IR trends for spike, drop, recovery, and signal-quality guidance.";
  hrBaselineEl.textContent = "--";
  hrChangeEl.textContent = "--";
  hrIrChangeEl.textContent = "--";
  setStatusPill(hrInsightAlertEl, "Stable", "good");
  renderPredictionHistory([]);
}

function renderAnalyticsSnapshot(payload) {
  const service = payload?.service ?? {};
  const processed = payload?.processed ?? {};
  const metrics = service.metrics ?? {};
  const modelsReady = Boolean(service.models_ready);
  const alertLevel = processed.alert_level ?? "Stable";
  const riskFactors = Array.isArray(processed.risk_factors) ? processed.risk_factors : [];
  const noteParts = [
    service.message,
    metrics.cnn_accuracy != null ? `CNN ${(metrics.cnn_accuracy * 100).toFixed(1)}%` : null,
    metrics.rf_accuracy != null ? `RF ${(metrics.rf_accuracy * 100).toFixed(1)}%` : null
  ].filter(Boolean);

  analyticsState.available = true;
  cnnPatientStateEl.textContent = processed.patient_state ?? "Collecting data";
  cnnConfidenceEl.textContent = processed.patient_state_confidence != null ? `${(processed.patient_state_confidence * 100).toFixed(1)}%` : "--";
  cnnWindowSizeEl.textContent = processed.cnn_window_size ? `${processed.cnn_window_size} samples` : `${service.window_size ?? "--"} samples`;
  setStatusPill(cnnModelStatusEl, modelsReady ? "Ready" : "Training", modelsReady ? "good" : "warn");
  cnnSummaryEl.textContent = processed.state_summary ?? "The CNN summary will appear here after the first processed prediction.";
  setStatusPill(analyticsServiceStatusEl, modelsReady ? "Connected" : "Starting", modelsReady ? "good" : "warn");
  analyticsRawRowsEl.textContent = String(service.raw_rows ?? 0);
  analyticsProcessedRowsEl.textContent = String(service.processed_rows ?? 0);
  analyticsLastTrainEl.textContent = formatDateTime(service.last_train_iso);
  analyticsStatusNoteEl.textContent = noteParts.join(" | ") || "Analytics service is connected.";
  rfFeedbackEl.textContent = processed.comfort_feedback ?? "Collecting data";
  rfConfidenceEl.textContent = processed.feedback_confidence != null ? `${(processed.feedback_confidence * 100).toFixed(1)}%` : "--";
  setStatusPill(alertLevelEl, alertLevel, alertLevel === "Urgent" ? "danger" : (alertLevel === "Observe" ? "warn" : "good"));
  recommendedActionEl.textContent = processed.recommended_action ?? "Waiting for enough samples to produce care feedback.";
  riskFactorsEl.textContent = riskFactors.length ? riskFactors.join(", ") : "No active high-risk trigger.";
  rfSummaryEl.textContent = processed.feedback_summary ?? "The Random Forest feedback summary will appear here after the first inference.";
  rawLogPathEl.textContent = service.raw_log_path ?? "analytics/data/raw_sensor_log.csv";
  processedLogPathEl.textContent = service.processed_log_path ?? "analytics/data/processed_predictions.csv";
  lastInferenceEl.textContent = formatDateTime(processed.created_at_iso ?? service.last_poll_iso);
  renderPredictionHistory(payload?.history ?? []);
}

async function updateAnalyticsSnapshot() {
  try {
    const response = await fetch(`${analyticsApiBase}/latest`);
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }
    const payload = await response.json();
    renderAnalyticsSnapshot(payload);
  } catch (error) {
    renderAnalyticsUnavailable("Analytics service is not reachable at http://127.0.0.1:5001/api. Start the local logger/model service to save raw and processed CSV files.");
    console.warn("Analytics update failed:", error);
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

  const restHours = sleepState.restingMs / 3600000;
  const activeHours = sleepState.activeMs / 3600000;
  totalSleepHoursEl.textContent = `${restHours.toFixed(2)} hrs`;
  sleepFormulaEl.textContent = `${restHours.toFixed(2)}h resting | ${activeHours.toFixed(2)}h active`;
  updateSleepSuggestions(restHours, activeHours, mode);
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
  setMeter(el.objectViz, (1 - metrics.distanceCm / 300) * 100, 2.4 - (1 - metrics.distanceCm / 300) * 1.4);
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

    const heart = toNumber(data.heartRate ?? data.heart ?? data.bpm, randomBetween(68, 98));
    const bodyTemp = toNumber(data.bodyTemp ?? data.body_temp ?? data.temp, randomBetween(36.2, 37.6, 1));
    const envTemp = toNumber(data.envTemp ?? data.environmentTemp ?? data.temp, randomBetween(22, 29, 1));
    const humidity = toNumber(data.hum ?? data.humidity, randomBetween(40, 65));
    const distanceCm = toNumber(data.dist ?? data.distance, randomBetween(40, 230));
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

    el.nearestObject.textContent = `${Math.round(distanceCm)} cm`;
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
      distanceCm,
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
    const nowMs = Date.now();
    updateAgitationTracking(heart, heartIr, patientMotion !== "Walking", nowMs);
    updateHeartInsight(heart, heartIr, patientMotion, nowMs);

    const now = new Date();
    el.lastUpdated.textContent = `Updated: ${now.toLocaleTimeString()}`;
  } catch (err) {
    console.error("Dashboard update failed:", err);
  }
}

// Keep the interval but reduce it to 2 seconds for real-time monitoring
setInterval(updateDashboard, 2000);
setInterval(updateAnalyticsSnapshot, 4000);

loadMobilityState();
renderMobility();
loadEnvStressState();
renderEnvStress();
setupTabs();
drawSleepChart();
renderAnalyticsUnavailable("Analytics service is not reachable at http://127.0.0.1:5001/api. Start the local logger/model service to enable automatic CSV logging and model predictions.");
updateDashboard();
updateAnalyticsSnapshot();

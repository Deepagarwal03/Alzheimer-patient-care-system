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

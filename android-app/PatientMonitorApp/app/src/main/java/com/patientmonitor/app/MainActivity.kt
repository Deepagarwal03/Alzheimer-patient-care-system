package com.patientmonitor.app

import android.Manifest
import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.webkit.WebChromeClient
import android.webkit.WebSettings
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.TextView
import androidx.activity.ComponentActivity
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.lifecycleScope
import com.patientmonitor.app.R
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import okhttp3.OkHttpClient
import okhttp3.Request
import org.json.JSONObject
import java.util.concurrent.TimeUnit

class MainActivity : ComponentActivity() {
    private lateinit var webView: WebView
    private lateinit var statusText: TextView
    private val httpClient = OkHttpClient.Builder()
        .connectTimeout(4, TimeUnit.SECONDS)
        .readTimeout(4, TimeUnit.SECONDS)
        .build()
    private var pollingJob: Job? = null
    private var lastAlertKey: String? = null

    private val notificationPermissionLauncher = registerForActivityResult(
        ActivityResultContracts.RequestPermission()
    ) { }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        webView = findViewById(R.id.dashboardWebView)
        statusText = findViewById(R.id.statusText)

        createNotificationChannel()
        requestNotificationPermissionIfNeeded()
        setupWebView()
        webView.loadUrl(DASHBOARD_URL)
    }

    override fun onStart() {
        super.onStart()
        startPolling()
    }

    override fun onStop() {
        pollingJob?.cancel()
        super.onStop()
    }

    private fun setupWebView() {
        webView.settings.javaScriptEnabled = true
        webView.settings.domStorageEnabled = true
        webView.settings.loadWithOverviewMode = true
        webView.settings.useWideViewPort = true
        webView.settings.mixedContentMode = WebSettings.MIXED_CONTENT_ALWAYS_ALLOW
        webView.webChromeClient = WebChromeClient()
        webView.webViewClient = object : WebViewClient() {
            override fun onPageFinished(view: WebView?, url: String?) {
                statusText.text = "Dashboard connected to $BASE_URL"
            }
        }
    }

    private fun startPolling() {
        pollingJob?.cancel()
        pollingJob = lifecycleScope.launch(Dispatchers.IO) {
            while (isActive) {
                runCatching { fetchSensorJson() }
                    .onSuccess { payload ->
                        launch(Dispatchers.Main) {
                            statusText.text = buildStatusText(payload)
                        }
                        evaluateAlerts(payload)
                    }
                    .onFailure { error ->
                        launch(Dispatchers.Main) {
                            statusText.text = "Connection issue: ${error.message ?: "Unable to reach ESP32"}"
                        }
                    }
                delay(2500)
            }
        }
    }

    private fun fetchSensorJson(): JSONObject {
        val request = Request.Builder().url("$BASE_URL/data").build()
        httpClient.newCall(request).execute().use { response ->
            if (!response.isSuccessful) {
                error("HTTP ${response.code}")
            }
            val body = response.body?.string().orEmpty()
            return JSONObject(body)
        }
    }

    private fun buildStatusText(payload: JSONObject): String {
        val heart = payload.optInt("heartRate", payload.optInt("heart", 0))
        val ir = payload.optLong("ir", 0L)
        val motion = payload.optString("motion", "Unknown")
        val env = payload.optString("env", "Unknown")
        return "BPM: $heart | IR: $ir | Motion: $motion | Env: $env"
    }

    private fun evaluateAlerts(payload: JSONObject) {
        val heart = payload.optInt("heartRate", payload.optInt("heart", 0))
        val ir = payload.optLong("ir", 0L)
        val motion = payload.optString("motion", "")
        val env = payload.optString("env", "")

        val alert = when {
            motion.contains("FALL", ignoreCase = true) -> AlertState(
                key = "fall",
                title = "Fall Alert",
                body = "Emergency: fall detected. Check the patient immediately."
            )
            heart >= 125 -> AlertState(
                key = "heart_spike",
                title = "Heart Rate Spike",
                body = "BPM is elevated at $heart. Slow activity and recheck the patient."
            )
            heart in 1..48 -> AlertState(
                key = "heart_drop",
                title = "Heart Rate Drop",
                body = "BPM dropped to $heart. Check posture, comfort, and sensor placement."
            )
            ir in 1..19999 -> AlertState(
                key = "weak_signal",
                title = "Weak Heart Sensor Signal",
                body = "IR signal is low at $ir. Adjust finger placement on the heart sensor."
            )
            env.contains("Cold", ignoreCase = true) && heart >= 100 -> AlertState(
                key = "env_stress",
                title = "Environment Stress",
                body = "Cold room with elevated BPM detected. Consider warming the room."
            )
            else -> null
        }

        if (alert == null) {
            lastAlertKey = null
            return
        }
        if (alert.key == lastAlertKey) {
            return
        }
        lastAlertKey = alert.key
        notifyAlert(alert)
    }

    private fun notifyAlert(alert: AlertState) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU &&
            ContextCompat.checkSelfPermission(this, Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED
        ) {
            return
        }

        val notification = NotificationCompat.Builder(this, CHANNEL_ID)
            .setSmallIcon(android.R.drawable.stat_notify_more)
            .setContentTitle(alert.title)
            .setContentText(alert.body)
            .setStyle(NotificationCompat.BigTextStyle().bigText(alert.body))
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .build()

        NotificationManagerCompat.from(this).notify(alert.key.hashCode(), notification)
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) return
        val channel = NotificationChannel(
            CHANNEL_ID,
            "Patient Alerts",
            NotificationManager.IMPORTANCE_HIGH
        ).apply {
            description = "Alerts for fall detection and abnormal patient readings"
        }
        getSystemService(NotificationManager::class.java)?.createNotificationChannel(channel)
    }

    private fun requestNotificationPermissionIfNeeded() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU &&
            ContextCompat.checkSelfPermission(this, Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED
        ) {
            notificationPermissionLauncher.launch(Manifest.permission.POST_NOTIFICATIONS)
        }
    }

    companion object {
        private const val BASE_URL = "http://192.168.4.1"
        private const val DASHBOARD_URL = "$BASE_URL/"
        private const val CHANNEL_ID = "patient_monitor_alerts"
    }
}

data class AlertState(
    val key: String,
    val title: String,
    val body: String,
)


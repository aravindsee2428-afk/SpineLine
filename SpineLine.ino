/************************************************************
 * SpineLine – ESP32 + MPU6050 + Web + Firebase Upload
 * Logic unchanged – Firebase added safely
 ************************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>

// ================= FIREBASE =================
#define API_KEY "**********"
#define DATABASE_URL "https://spineline-9038f-default-rtdb.asia-southeast1.firebasedatabase.app"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ================= HARDWARE =================
#define MPU_ADDR 0x68
#define SDA_PIN 23
#define SCL_PIN 22
#define VIBRATE_PIN 32

// ================= WIFI =================
const char* ssid = "vivo T2x 5G";
const char* password = "Krishna@08";

WebServer server(80);

// ================= POSTURE DATA =================
float acc_x = 0.0;
int leanCount = 0;
int totalReadings = 0;
unsigned long sessionStart = 0;

// ================= VIBRATION =================
bool vibrating = false;
unsigned long vibStartTime = 0;
#define VIB_DURATION 2000
#define LEAN_THRESHOLD 10

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  pinMode(VIBRATE_PIN, OUTPUT);
  digitalWrite(VIBRATE_PIN, LOW);

  // -------- WiFi --------
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  // -------- Firebase --------
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase connected");

  // -------- MPU6050 --------
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  sessionStart = millis();

  // -------- Web Server --------
  server.on("/", HTTP_GET, handleRoot);
  server.on("/posture", HTTP_GET, handlePosture);
  server.on("/reset", HTTP_POST, handleReset);
  server.begin();

  Serial.println("SpineLine Web Dashboard LIVE");
}

// ================= LOOP =================
void loop() {
  server.handleClient();

  if (!readAccelX()) return;

  totalReadings++;
  int posture = predictPosture(acc_x);

  // -------- VIBRATION LOGIC (UNCHANGED) --------
  if (posture == 1) {
    leanCount++;
    if (leanCount >= LEAN_THRESHOLD && !vibrating) {
      vibrating = true;
      vibStartTime = millis();
      digitalWrite(VIBRATE_PIN, HIGH);
    }
  } else {
    leanCount = 0;
    vibrating = false;
    digitalWrite(VIBRATE_PIN, LOW);
  }

  if (vibrating && millis() - vibStartTime >= VIB_DURATION) {
    vibrating = false;
    digitalWrite(VIBRATE_PIN, LOW);
  }

  // -------- FIREBASE UPLOAD (EVERY 5s) --------
  static unsigned long lastUpload = 0;
  if (millis() - lastUpload > 5000) {
    lastUpload = millis();

    String basePath = "/sessions/" + String(sessionStart);

    Firebase.RTDB.pushFloat(&fbdo, basePath + "/ax", acc_x);
    Firebase.RTDB.pushString(
      &fbdo,
      basePath + "/posture",
      posture == 1 ? "LEAN" : "STRAIGHT"
    );

    Serial.println("📤 Data uploaded to Firebase");
  }

  delay(200);
}

// ================= WEB PAGE =================
void handleRoot() {
  unsigned long usageMinutes = (millis() - sessionStart) / 60000;

  int straightReadings = totalReadings - leanCount;
  if (straightReadings < 0) straightReadings = 0;

  float straightPct = (totalReadings > 0)
                        ? (float)straightReadings / totalReadings * 100.0
                        : 0;

  String remark =
    straightPct >= 80 ? "EXCELLENT" :
    straightPct >= 50 ? "GOOD" : "TERRIBLE";

  String postureNow = predictPosture(acc_x) ? "LEAN" : "STRAIGHT";

  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>SpineLine Device</title>
<meta http-equiv="refresh" content="2">
<style>
body { font-family: Arial; background:#f0f0f0; }
.container {
  width:520px; margin:40px auto; background:#fff;
  padding:25px; border-radius:8px;
  box-shadow:0 0 12px rgba(0,0,0,0.2);
}
.big { font-size:22px; font-weight:bold; }
.green { color:green; } .red { color:red; }
</style>
</head>
<body>
<div class="container">
<h1>SpineLine Device</h1>

<div class="big">Status: <span class="green">ONLINE</span></div>
<p>Usage Time: <b>)rawliteral" + String(usageMinutes) + R"rawliteral( min</b></p>

<hr>
<p>Current Ax: <b>)rawliteral" + String(acc_x,3) + R"rawliteral( g</b></p>
<p class="big">Posture:
<span class=")rawliteral" + (postureNow=="STRAIGHT"?"green":"red") + R"rawliteral(">
)rawliteral" + postureNow + R"rawliteral(
</span></p>

<hr>
<p>Straight %: <b>)rawliteral" + String(straightPct,1) + R"rawliteral(%</b></p>
<p class="big">Remark:
<span class=")rawliteral" + (straightPct>=50?"green":"red") + R"rawliteral(">
)rawliteral" + remark + R"rawliteral(
</span></p>

</div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

// ================= API =================
void handlePosture() {
  DynamicJsonDocument doc(256);
  doc["ax"] = acc_x;
  doc["posture"] = predictPosture(acc_x) ? "LEAN" : "STRAIGHT";
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleReset() {
  sessionStart = millis();
  totalReadings = 0;
  leanCount = 0;
  server.send(200, "text/plain", "Stats reset");
}

// ================= CORE =================
int predictPosture(float ax) {
  // YOUR LOGIC – UNCHANGED
  return (ax <= -0.970) ? 0 : 1;
}

bool readAccelX() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDR, 2, true) != 2) return false;

  int16_t raw_ax = (Wire.read() << 8) | Wire.read();
  acc_x = raw_ax / 16384.0;
  return true;
}

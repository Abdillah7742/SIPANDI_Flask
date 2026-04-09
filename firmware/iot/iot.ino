#include <WiFi.h>
#include <WiFiManager.h> // Library: WiFiManager by tzapu
#include <FirebaseESP32.h>
#include <DHT.h>
#include <ESP32Servo.h>

// --- KONFIGURASI FIREBASE ---
#define FIREBASE_HOST "https://sipandi-3d71d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyD_XxqrHpLMn7h9vFGwT2Zu7cxShGT1vOY"

// --- PIN DEFINITIONS ---
#define DHTPIN 4
#define DHTTYPE DHT22
#define SOIL_PIN 34
#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO1_PIN 13
#define SERVO2_PIN 14
#define RESET_PIN 0 

// --- OBJECTS ---
DHT dht(DHTPIN, DHTTYPE);
Servo servo1;
Servo servo2;
FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;
WiFiManager wm;

// --- VARIABLES ---
unsigned long lastMillis = 0;
unsigned long lastHistoryMillis = 0;
int servo2_angle = 90;
String deviceID = "ALAT-01"; 

void setup() {
    Serial.begin(115200);

    pinMode(RESET_PIN, INPUT_PULLUP);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);  

    dht.begin();
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    servo1.write(0);
    servo2.write(90);

    // --- WIFI SETUP (CAPTIVE PORTAL) ---
    Serial.println("Mencari WiFi...");
    
    // Perbaikan: Gunakan setTitle, bukan setHeading
    wm.setTitle("SIPANDI Setup Portal");
    wm.setCustomHeadElement("<style>body{font-family:sans-serif;text-align:center;} h2{color:#5ca904;}</style>");
    
    // Timer keamanan agar hotspot tidak menyala terus (3 menit)
    wm.setConfigPortalTimeout(180); 

    if (!wm.autoConnect(("SIPANDI-" + deviceID).c_str(), "12345678")) {
        Serial.println("Timeout, restart...");
        delay(3000);
        ESP.restart();
    }

    Serial.println("Connected! IP: " + WiFi.localIP().toString());

    // --- FIREBASE SETUP ---
    config.database_url = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;  
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop() {
    // 1. CEK TOMBOL RESET (Tekan 5 detik)
    if (digitalRead(RESET_PIN) == LOW) {
        unsigned long startPress = millis();
        while (digitalRead(RESET_PIN) == LOW) {
            if (millis() - startPress > 5000) {
                Serial.println("Resetting WiFi Settings...");
                wm.resetSettings();
                delay(100);
                ESP.restart();
            }
            delay(10);
        }
    }

    // 2. KIRIM DATA & LOGIKA OTOMATIS (Setiap 5 Detik)
    if (millis() - lastMillis > 5000) {
        lastMillis = millis();

        float h = dht.readHumidity();
        float t = dht.readTemperature();
        int soil_raw = analogRead(SOIL_PIN);
        int soil_percent = map(soil_raw, 4095, 0, 0, 100);

        digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        long duration = pulseIn(ECHO_PIN, HIGH);
        int distance = duration * 0.034 / 2;

         FirebaseJson json;
         json.set("sensors/suhu", t);
         json.set("sensors/humidity", h);
         json.set("sensors/soil", soil_percent);
         json.set("hama/distance", distance);
         json.set("last_seen/.sv", "timestamp"); // Heartbeat 
 
         if (Firebase.updateNode(fbdo, "/devices/" + deviceID, json)) {
             Serial.println("Data terupdate.");
         }

        // Simpan Riwayat (History) 1 Jam
        if (millis() - lastHistoryMillis > 3600000 || lastHistoryMillis == 0) {
            lastHistoryMillis = millis();
            FirebaseJson hist;
            hist.set("suhu", t);
            hist.set("humidity", h);
            hist.set("soil", soil_percent);
            hist.set("hama_detected", (distance > 0 && distance < 30)); 
            Firebase.setJSON(fbdo, "/devices/" + deviceID + "/history/" + String(lastHistoryMillis), hist);
        }

        // Deteksi Hama Otomatis
        static int hamaThreshold = 30;
        if (Firebase.getInt(fbdo, "/devices/" + deviceID + "/settings/hama_threshold")) {
            hamaThreshold = fbdo.intData();
        }

        if (distance > 0 && distance < hamaThreshold) {
            servo1.write(160); delay(150);
            servo1.write(90); delay(150);
        }
    }

    // 3. LISTEN COMMANDS
    if (Firebase.getJSON(fbdo, "/devices/" + deviceID + "/commands")) {
        FirebaseJson &json = fbdo.jsonObject();
        FirebaseJsonData data;
        
        json.get(data, "servo1");
        if (data.success) {
            ringBell();
            Firebase.deleteNode(fbdo, "/devices/" + deviceID + "/commands/servo1");
        }

        json.get(data, "servo2_angle");
        if (data.success) {
            int angle = data.intValue;
            if (angle != servo2_angle) {
                servo2_angle = angle;
                servo2.write(servo2_angle);
            }
        }
    }
}

void ringBell() {
    for (int i = 0; i < 3; i++) {
        servo1.write(160); delay(200);
        servo1.write(90); delay(200);
    }
}

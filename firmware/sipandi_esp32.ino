#include <WiFi.h>
#include <WiFiProv.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <ESP32Servo.h>

// --- KONFIGURASI FIREBASE ---
#define FIREBASE_HOST "https://your-project-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YOUR_DATABASE_SECRET"

// --- PIN DEFINITIONS ---
#define DHTPIN 4
#define DHTTYPE DHT22
#define SOIL_PIN 34
#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO1_PIN 13
#define SERVO2_PIN 14

// --- OBJECTS ---
DHT dht(DHTPIN, DHTTYPE);
Servo servo1;
Servo servo2;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- VARIABLES ---
unsigned long lastMillis = 0;
int servo2_angle = 90;

void SysProvEvent(arduino_event_t *sys_event) {
    switch (sys_event->event_id) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Connected! IP: ");
            Serial.println(IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr));
            break;
        case ARDUINO_EVENT_PROV_START:
            Serial.println("Provisioning Started");
            break;
        case ARDUINO_EVENT_PROV_CRED_RECV: {
            Serial.println("Credentials Received");
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    // Init Sensors
    dht.begin();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Init Servos
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    servo1.write(0);
    servo2.write(90);

    // --- WIFI PROVISIONING via BLE ---
    WiFi.onEvent(SysProvEvent);
    Serial.println("Starting Provisioning...");
    
    // Gunakan nama "SIPANDI_PROV" untuk BLE
    // Gunakan QR Code di dokumentasi ESP untuk scan
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_ADDR_NONE, WIFI_PROV_SECURITY_1, "12345678", "SIPANDI_PROV");

    // Tunggu sampai konek
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // --- FIREBASE SETUP ---
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop() {
    if (millis() - lastMillis > 5000) { // Kirim data setiap 5 detik
        lastMillis = millis();

        // 1. Baca DHT22
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        // 2. Baca Soil Moisture
        int soil_raw = analogRead(SOIL_PIN);
        int soil_percent = map(soil_raw, 4095, 0, 0, 100); // Sesuaikan dengan kalibrasi sensor Anda

        // 3. Baca Ultrasonic (Hama)
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        long duration = pulseIn(ECHO_PIN, HIGH);
        int distance = duration * 0.034 / 2;

        // 4. Update Firebase
        FirebaseJson json;
        json.set("suhu", t);
        json.set("humidity", h);
        json.set("soil", soil_percent);
        Firebase.RTDB.setJSON(&fbdo, "/sensors", &json);

        FirebaseJson hamaJson;
        hamaJson.set("distance", distance);
        Firebase.RTDB.setJSON(&fbdo, "/hama", &hamaJson);

        // Auto Logic: Jika hama dekat (< 30cm), gerakkan Servo 1 (Lonceng)
        if (distance > 0 && distance < 30) {
            Serial.println("HAMA TERDETEKSI!");
            ringBell();
        }
    }

    // --- LISTEN COMMANDS FROM DASHBOARD ---
    
    // Command Servo 1 (Manual Ring)
    if (Firebase.RTDB.getJSON(&fbdo, "/commands/servo1")) {
        if (fbdo.dataType() == "json") {
            // Logika trigger lonceng manual
            ringBell();
            Firebase.RTDB.deleteNode(&fbdo, "/commands/servo1"); // Hapus command setelah diproses
        }
    }

    // Command Servo 2 (Slider Angle)
    if (Firebase.RTDB.getInt(&fbdo, "/commands/servo2_angle")) {
        int angle = fbdo.intData();
        if (angle != servo2_angle) {
            servo2_angle = angle;
            servo2.write(servo2_angle);
            Serial.print("Moving Servo 2 to: ");
            Serial.println(servo2_angle);
        }
    }
}

void ringBell() {
    // Gerakkan servo bolak-balik untuk membunyikan lonceng
    for (int i = 0; i < 3; i++) {
        servo1.write(90);
        delay(200);
        servo1.write(0);
        delay(200);
    }
}

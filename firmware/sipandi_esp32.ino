    #include <WiFi.h>
    #include <WiFiManager.h> // Library: WiFiManager by tzapu
    #include <Firebase_ESP_Client.h>
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
    #define RESET_PIN 0  // Tombol Boot/Flash di ESP32 untuk Reset WiFi

    // --- OBJECTS ---
    DHT dht(DHTPIN, DHTTYPE);
    Servo servo1;
    Servo servo2;
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    WiFiManager wm;

    // --- VARIABLES ---
    unsigned long lastMillis = 0;
    unsigned long lastHistoryMillis = 0;
    int servo2_angle = 90;
    String deviceID = "ALAT-01"; // ID Perangkat

    void setup() {
        Serial.begin(115200);
        
        // Init Pins
        pinMode(RESET_PIN, INPUT_PULLUP);
        pinMode(TRIG_PIN, OUTPUT);
        pinMode(ECHO_PIN, INPUT);
        
        // Init Sensors & Servos
        dht.begin();
        servo1.attach(SERVO1_PIN);
        servo2.attach(SERVO2_PIN);
        servo1.write(0);
        servo2.write(90);

        // --- WIFI SETUP (CAPTIVE PORTAL) ---
        Serial.println("Konek ke WiFi SIPANDI-" + deviceID + " untuk Setup...");
        
        // Kustomisasi Halaman Portal
        String custom_html = "<br><center><h3>ID PERANGKAT: " + deviceID + "</h3>";
        custom_html += "<div id='qrcode'></div>";
        custom_html += "<script src='https://cdn.rawgit.com/davidshimjs/qrcodejs/gh-pages/qrcode.min.js'></script>";
        custom_html += "<script>new QRCode(document.getElementById('qrcode'), '" + deviceID + "');</script>";
        custom_html += "<p>Scan QR di atas lewat Aplikasi SIPANDI</p></center>";
        
        WiFiManagerParameter custom_text(custom_html.c_str());
        wm.addParameter(&custom_text);

        // Jalankan Portal jika tidak konek WiFi
        if (!wm.autoConnect(("SIPANDI-" + deviceID).c_str(), "12345678")) {
            Serial.println("Gagal konek, restart...");
            delay(3000);
            ESP.restart();
        }

        Serial.println("Connected! IP: " + WiFi.localIP().toString());

        // --- FIREBASE SETUP ---
        config.host = FIREBASE_HOST;
        config.signer.tokens.legacy_token = FIREBASE_AUTH;
        Firebase.begin(&config, &auth);
        Firebase.reconnectWiFi(true);
    }

    void loop() {
        // 1. CEK TOMBOL RESET (Tekan 5 detik untuk Reset WiFi)
        if (digitalRead(RESET_PIN) == LOW) {
            unsigned long startPress = millis();
            while (digitalRead(RESET_PIN) == LOW) {
                if (millis() - startPress > 5000) {
                    Serial.println("Resetting WiFi Settings...");
                    wm.resetSettings();
                    ESP.restart();
                }
                delay(10);
            }
        }

        // 2. KIRIM DATA SENSOR (Setiap 5 Detik)
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

            if (Firebase.RTDB.updateNode(&fbdo, "/devices/" + deviceID, &json)) {
                Serial.println("Realtime Data Update Success");
            }

            // 3. LOG RIWAYAT (Setiap 1 Jam)
            if (millis() - lastHistoryMillis > 3600000 || lastHistoryMillis == 0) {
                lastHistoryMillis = millis();
                FirebaseJson hist;
                hist.set("suhu", t);
                hist.set("humidity", h);
                hist.set("soil", soil_percent);
                hist.set("hama_detected", (distance > 0 && distance < 30)); // 30cm default
                
                String path = "/devices/" + deviceID + "/history/" + String(lastHistoryMillis);
                Firebase.RTDB.setNode(&fbdo, path, &hist);
            }

            // --- BACA SETTINGS DASHBOARD ---
            static int hamaThreshold = 30;
            if (Firebase.RTDB.getInt(&fbdo, "/devices/" + deviceID + "/settings/hama_threshold")) {
                hamaThreshold = fbdo.intData();
            }

            // Logic Lonceng Otomatis
            if (distance > 0 && distance < hamaThreshold) {
                servo1.write(160); delay(150);
                servo1.write(90); delay(150);
            }
        }

        // 4. LISTEN COMMANDS
        if (Firebase.RTDB.getJSON(&fbdo, "/devices/" + deviceID + "/commands")) {
            // Logic manual ring & radar rotation here...
            // (Sama seperti versi sebelumnya)
        }
    }

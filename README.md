# SIPANDI (Sistem Pemantauan Padi Terintegrasi) 🌾

**SIPANDI** adalah platform IoT (Internet of Things) modern yang dirancang untuk membantu petani memantau kondisi persawahan secara real-time. Proyek ini menggabungkan microcontroller ESP32, berbagai sensor lingkungan, dan dashboard web berbasis Flask & Firebase.

## ✨ Fitur Utama
- **Dashboard Real-time**: Sinkronisasi data sensor (Suhu, Kelembaban, Tanah) instan via Firebase RTDB.
- **Multi-Device Management**: Kelola banyak alat dalam satu akun pengguna.
- **Authentication**: Keamanan akun menggunakan Firebase Auth (Email/Password & Google Sign-In).
- **QR Code Onboarding**: Tambahkan perangkat baru dengan fitur scan QR Code.
- **Radar Hama**: Deteksi hama otomatis dan kontrol servo lonceng dari jarak jauh.
- **Premium UI**: Desain responsif, bersih, dan kontras tinggi (Sipandi Green Theme #5ca904).

## 🛠️ Teknologi yang Digunakan
- **Backend**: Python (Flask)
- **Frontend**: HTML5, Tailwind CSS, Animate.css
- **Database & Auth**: Firebase (Authentication, Firestore, Realtime Database)
- **Hardware**: ESP32, DHT22, Soil Moisture Sensor, Ultrasonic HC-SR04, Servo Motors.

## 📂 Struktur Proyek
```text
SIPANDI_Flask/
├── app.py              # Server Flask Utama
├── templates/          # Halaman HTML (Jinja2)
│   ├── auth.html       # Login & Register
│   ├── devices.html    # Daftar Perangkat
│   ├── add-device.html # Scan QR Tambah Alat
│   └── dashboard.html  # Monitoring & Kontrol
├── static/             # Aset Statis (CSS, JS, Gambar)
└── README.md           # Dokumentasi Proyek
```

## 🚀 Cara Instalasi Lokal

1. **Clone Repositori**
   ```bash
   git clone https://github.com/username/SIPANDI_Flask.git
   cd SIPANDI_Flask
   ```

2. **Setup Virtual Environment & Install Dependencies**
   ```bash
   python -m venv venv
   source venv/Scripts/activate  # Untuk Windows
   pip install flask
   ```

3. **Konfigurasi Firebase**
   Buka file di dalam folder `templates/` dan pastikan `firebaseConfig` sudah diisi dengan kredensial dari Firebase Console Anda.

4. **Jalankan Aplikasi**
   ```bash
   python app.py
   ```
   Buka `http://localhost:5000` di browser Anda.

## 🔌 Hardware Setup
Pastikan Anda mengunggah firmware ke ESP32 menggunakan library berikut:
- `Firebase ESP Client`
- `DHT sensor library`
- `ESP32Servo`

---
Dibuat dengan ❤️ untuk kemajuan pertanian Indonesia.

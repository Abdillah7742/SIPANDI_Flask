# SIPANDI v2.5 (Sistem Pemantauan Padi Terintegrasi) 🌾

**SIPANDI** adalah platform IoT pertanian modern yang dirancang untuk memantau kondisi persawahan secara real-time. Versi 2.5 ini membawa peningkatan signifikan pada UI/UX, keamanan, dan manajemen perangkat.

## ✨ Fitur Utama
- **Dashboard Real-time**: Monitoring Suhu, Kelembaban Udara, dan Tanah secara instan via Firebase RTDB.
- **Modern Premium UI**: Tema **Sipandi Green (#5ca904)** dengan desain clean, glassmorphism, dan animasi energetik.
- **Multi-Device Support**: Kelola banyak unit SIPANDI dalam satu akun petani.
- **Google Authentication**: Masuk aman dan cepat menggunakan akun Google atau Email.
- **Smart Radar Hama**: Deteksi hama ultrasonik dengan peringatan visual dan kontrol lonceng jarak jauh.
- **PWA Ready**: Dilengkapi Service Worker (v2) untuk performa lebih cepat dan akses offline-first.

## 🛠️ Persyaratan Sistem
- **Microcontroller**: ESP32
- **Sensors**: DHT22, Soil Moisture, HC-SR04 (Ultrasonic)
- **Actuators**: 2x Servo SG90 (Lonceng & Radar)
- **Backend**: Flask (Python 3.x)
- **Database**: Firebase (RTDB & Firestore)

## 🚀 Cara Menjalankan
1. Clone repositori: `git clone https://github.com/Abdillah7742/SIPANDI_Flask.git`
2. Install dependencies: `pip install flask`
3. Jalankan server: `python app.py`
4. Akses di browser: `http://localhost:5000`

## 📂 Struktur Rute
- `/` : Daftar Perangkat & Beranda (Devices List)
- `/auth` : Halaman Masuk & Daftar Akun
- `/add-device` : Tambah Perangkat Baru via QR Scan
- `/dashboard/<id>` : Monitoring Real-time Perangkat Tertentu

---
Dibuat dengan ❤️ untuk Petani Indonesia.

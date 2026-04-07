import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-app.js";
import { getDatabase, ref, onValue, set } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-database.js";

// --- KONFIGURASI FIREBASE ---
// Ganti dengan konfigurasi dari Firebase Console Anda!
const firebaseConfig = {
    apiKey: "YOUR_API_KEY",
    authDomain: "your-project.firebaseapp.com",
    databaseURL: "https://your-project-default-rtdb.firebaseio.com",
    projectId: "your-project",
    storageBucket: "your-project.appspot.com",
    messagingSenderId: "your-id",
    appId: "your-app-id"
};

// Cek apakah config sudah diisi
if (firebaseConfig.apiKey === "YOUR_API_KEY") {
    alert("Peringatan: Konfigurasi Firebase belum diisi di static/js/dashboard.js! Data real-time tidak akan muncul.");
}

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

// --- LISTENER DATA REAL-TIME ---
const statusEl = document.getElementById('conn-status');
const suhuEl = document.getElementById('suhu-val');
const humiEl = document.getElementById('humi-val');
const soilEl = document.getElementById('soil-val');
const hamaStatusEl = document.getElementById('hama-status');
const hamaDistEl = document.getElementById('hama-dist');

// Status Koneksi Firebase
const connectedRef = ref(db, ".info/connected");
onValue(connectedRef, (snap) => {
    if (snap.val() === true) {
        statusEl.innerText = "🟢 Online (Firebase)";
        statusEl.className = "status-indicator status-online";
    } else {
        statusEl.innerText = "🔴 Offline";
        statusEl.className = "status-indicator status-offline";
    }
});

// Baca Data Sensor
onValue(ref(db, 'sensors'), (snapshot) => {
    const data = snapshot.val();
    if (data) {
        if (data.suhu) suhuEl.innerHTML = `${data.suhu}<span class="unit">°C</span>`;
        if (data.humidity) humiEl.innerHTML = `${data.humidity}<span class="unit">%</span>`;
        if (data.soil) soilEl.innerHTML = `${data.soil}<span class="unit">%</span>`;
        
        document.getElementById('suhu-ts').innerText = `Terakhir: ${new Date().toLocaleTimeString()}`;
        document.getElementById('humi-ts').innerText = `Terakhir: ${new Date().toLocaleTimeString()}`;
        document.getElementById('soil-ts').innerText = `Terakhir: ${new Date().toLocaleTimeString()}`;
    }
});

// Baca Data Hama
onValue(ref(db, 'hama'), (snapshot) => {
    const data = snapshot.val();
    if (data) {
        const dist = data.distance || 0;
        hamaDistEl.innerText = `Jarak: ${dist} cm`;
        
        if (dist > 0 && dist < 30) {
            hamaStatusEl.innerText = "⚠️ TERDETEKSI";
            hamaStatusEl.style.color = "#FF4D4D"; // Danger
        } else {
            hamaStatusEl.innerText = "AMAN";
            hamaStatusEl.style.color = "#00FFCC"; // Normal
        }
    }
});

// --- KONTROL SERVO ---
window.triggerServo1 = () => {
    set(ref(db, 'commands/servo1'), {
        action: 'ring',
        timestamp: Date.now()
    }).then(() => {
        const btn = document.getElementById('btn-ring');
        btn.innerText = "MENGIRIM...";
        setTimeout(() => btn.innerText = "BUNYIKAN SEKARANG", 1000);
    });
};

window.updateServo2 = (val) => {
    document.getElementById('angle-val').innerText = `${val}°`;
    set(ref(db, 'commands/servo2_angle'), parseInt(val));
};

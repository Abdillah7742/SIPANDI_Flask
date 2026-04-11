const CACHE_NAME = 'sipandi-v3'; // Naik ke v3 untuk memaksa pembersihan cache lama
const ASSETS = [
    '/',
    '/static/css/base.css',
    '/static/js/service-worker.js'
];

self.addEventListener('install', (e) => {
    e.waitUntil(
        caches.open(CACHE_NAME).then((cache) => {
            return cache.addAll(ASSETS);
        })
    );
    self.skipWaiting();
});

self.addEventListener('activate', (e) => {
    e.waitUntil(
        caches.keys().then((keys) => {
            return Promise.all(
                keys.map((key) => {
                    if (key !== CACHE_NAME) {
                        return caches.delete(key);
                    }
                })
            );
        })
    );
    return self.clients.claim(); // Memastikan SW baru mengambil kendali semua halaman
});

// STRATEGI: Network First (Coba koneksi dulu, baru cache)
self.addEventListener('fetch', (e) => {
    // Abaikan permintaan Firebase/Eksternal agar tidak bentrok
    if (e.request.url.includes('firebaseio.com') || e.request.url.includes('googleapis.com')) {
        return;
    }

    e.respondWith(
        fetch(e.request)
            .then((response) => {
                // Jika sukses, simpan salinan terbaru ke cache
                if (response && response.status === 200 && response.type === 'basic') {
                    const responseToCache = response.clone();
                    caches.open(CACHE_NAME).then((cache) => {
                        cache.put(e.request, responseToCache);
                    });
                }
                return response;
            })
            .catch(() => {
                // Jika offline (koneksi gagal), baru ambil dari cache
                return caches.match(e.request);
            })
    );
});

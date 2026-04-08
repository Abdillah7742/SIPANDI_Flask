const CACHE_NAME = 'sipandi-v2'; // Berubah ke v2 untuk memaksa pembersihan memori
const ASSETS = [
    '/',
    '/static/css/base.css',
    // dashboard.js sudah dihapus dan tidak perlu disimpan di memori lagi
];

self.addEventListener('install', (e) => {
    e.waitUntil(
        caches.open(CACHE_NAME).then((cache) => {
            return cache.addAll(ASSETS);
        })
    );
    self.skipWaiting(); // Langsung aktifkan versi baru
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
});

self.addEventListener('fetch', (e) => {
    e.respondWith(
        caches.match(e.request).then((res) => {
            return res || fetch(e.request);
        })
    );
});

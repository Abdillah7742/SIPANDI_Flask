from flask import Flask, render_template, redirect, url_for

app = Flask(__name__)

# --- ROUTES ---

@app.route("/")
def index():
    return render_template("devices.html")

@app.route("/auth")
def auth():
    return render_template("auth.html")

@app.route("/add-device")
def add_device():
    return render_template("add-device.html")

@app.route("/dashboard/<device_id>")
def dashboard(device_id):
    return render_template("dashboard.html", device_id=device_id)

@app.route("/simulator")
def simulator():
    return render_template("simulator.html")

# Service Worker for Offline-First
@app.route("/service-worker.js")
def sw():
    return app.send_static_file("js/service-worker.js")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
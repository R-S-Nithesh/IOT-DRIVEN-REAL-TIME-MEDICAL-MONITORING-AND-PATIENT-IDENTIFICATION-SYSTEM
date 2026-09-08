# 🏥 IoT-Driven Real-Time Medical Monitoring & Patient Identification System

<div align="center">

![IoT](https://img.shields.io/badge/IoT-Enabled-blue?style=for-the-badge&logo=raspberry-pi)
![ESP32](https://img.shields.io/badge/ESP32-Node-green?style=for-the-badge)
![RFID](https://img.shields.io/badge/RFID-Patient_ID-orange?style=for-the-badge)
![PHP](https://img.shields.io/badge/PHP-Backend-purple?style=for-the-badge&logo=php)
![MySQL](https://img.shields.io/badge/MySQL-Database-blue?style=for-the-badge&logo=mysql)

*A seamless, real-time physiological monitoring system with secure RFID-based patient identification.*

</div>

---

## 📖 Overview

The Internet of Things (IoT) has become a foundational enabler of intelligent, interconnected medical infrastructures. By linking sensors, devices, and clinical information systems, IoT facilitates seamless acquisition, transmission, and analysis of real-time physiological data.

Despite these advances, two persistent challenges continue to undermine patient safety and clinical efficiency:

- **Lack of continuous monitoring** — Conventional approaches rely on periodic manual checks, which fail to capture sudden physiological fluctuations.
- **Patient misidentification** — Manual verification processes remain vulnerable to human error, leading to data-mixing, delayed interventions, and preventable complications.

This project addresses both challenges through an integrated IoT platform that combines **continuous wearable sensing** with **secure RFID-based digital identification**, enabling:

- Real-time measurement of vital signs (heart rate, SpO₂, body temperature, ECG, air quality)
- Automatic matching of sensor data to the correct patient's Electronic Health Record (EHR)
- Centralized visualization and alerting for timely clinical decision-making

---

## 🏗️ System Architecture

```
┌─────────────────────┐        ┌──────────────────────┐
│   ESP32 (ECG Node)  │──────▶│                      │
│  AD8232, MAX30102   │  WiFi  │   XAMPP Local Server │
│  DS18B20, DHT11     │        │   (Apache + MySQL)   │
│  MQ135, Buzzer      │        │                      │
└─────────────────────┘        │   ┌──────────────┐   │
                               │   │  MedicalAPI  │   │
┌─────────────────────┐        │   │  (PHP + HTML)│   │
│  ESP8266 D1 Mini    │──────▶│   └──────────────┘   │
│  (RFID Node)        │  WiFi  │                      │
│  RDM6300 Module     │        └──────────────────────┘
└─────────────────────┘                  │
                                         ▼
                               ┌──────────────────────┐
                               │  Web Dashboard (EHR) │
                               │  Live ECG & Vitals   │
                               └──────────────────────┘
```

---

## 🔧 Hardware Requirements

| Component | Role |
|---|---|
| **ESP32** | ECG & vitals monitoring node |
| **ESP8266 D1 Mini** | RFID reader node |
| **AD8232** | ECG signal acquisition |
| **MAX30102** | Heart rate & SpO₂ (pulse oximetry) |
| **DS18B20** | Body temperature sensing |
| **DHT11** | Ambient temperature & humidity |
| **MQ135** | Air quality / gas sensing |
| **YL-44 Buzzer** | Audible alert output |
| **RDM6300 Module** | 125 kHz RFID reader |
| **RFID Wristbands / QR Codes** | Patient identification tokens |

---

## 💻 Software Requirements

| Tool | Purpose |
|---|---|
| **Arduino IDE** | Firmware development & upload |
| **ESP32 Board Package** | ESP32 support in Arduino IDE |
| **ESP8266 Board Package** | ESP8266 support in Arduino IDE |
| **XAMPP** | Local Apache + MySQL server |
| **Web Browser** | Dashboard access |

---

## 📁 Repository Structure

```
├── Hardware/
│   ├── ECG_Monitoring/
│   │   └── ECG_Monitoring.ino       # ESP32 firmware
│   └── RFID_Reader/
│       └── RFID_Reader.ino          # ESP8266 firmware
├── MedicalAPI/                      # Web application (PHP, HTML, JS, CSS)
│   ├── config.php
│   ├── index.html
│   └── api/
│       ├── get_patients.php
│       ├── get_patient_by_rfid.php
│       └── save_sensor_data.php
└── Database/
    └── medical_monitoring.sql       # MySQL schema & seed data
```

---

## 🚀 Setup & Installation

### Step 1 — Set Up the Database

1. Start **XAMPP** and ensure Apache and MySQL are running.
2. Open [http://localhost/phpmyadmin](http://localhost/phpmyadmin) in your browser.
3. Create a new database named `medical_monitoring`.
4. Import `Database/medical_monitoring.sql` into the new database.
5. *(Optional)* Edit the default patient row to match your RFID wristband UID.

---

### Step 2 — Configure the Web Application

1. Copy the `MedicalAPI/` folder to your XAMPP `htdocs` directory:
   - **Windows:** `C:\xampp\htdocs\MedicalAPI\`
   - **Linux/macOS:** `/opt/lampp/htdocs/MedicalAPI/`

2. Edit `config.php` with your database credentials:

    ```php
    define('DB_HOST', 'localhost');
    define('DB_NAME', 'medical_monitoring');
    define('DB_USER', 'root');
    define('DB_PASS', '');
    ```

3. Verify the setup by opening the following URLs in your browser:
   - `http://localhost/MedicalAPI/api/get_patients.php`
   - `http://localhost/MedicalAPI/index.html`

---

### Step 3 — Configure & Upload the RFID Node (ESP8266)

1. Open `Hardware/RFID_Reader/RFID_Reader.ino` in Arduino IDE.
2. Install the required libraries:
   - `ESP8266WiFi`
   - `ESP8266HTTPClient`
   - `SoftwareSerial`
3. Update the Wi-Fi and API credentials:

    ```cpp
    const char* WIFI_SSID = "YOUR_WIFI_SSID";
    const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
    const char* API_BASE  = "http://YOUR_PC_IP/MedicalAPI/api/get_patient_by_rfid.php?rfid=";
    ```

    > **Note:** Replace `YOUR_PC_IP` with the local IP address of the machine running XAMPP.

4. Wire the RDM6300 to the ESP8266 D1 Mini:

    | RDM6300 Pin | ESP8266 Pin |
    |---|---|
    | VCC | 3.3V |
    | GND | GND |
    | TX | D7 (GPIO13) |

5. Select board **NodeMCU 1.0** or **Wemos D1 Mini** and upload the firmware.

---

### Step 4 — Configure & Upload the ECG Node (ESP32)

1. Open `Hardware/ECG_Monitoring/ECG_Monitoring.ino` in Arduino IDE.
2. Install the required libraries:
   - `WiFi`, `HTTPClient`
   - `Wire`, `OneWire`, `DallasTemperature`
   - `DHT`
   - `MAX30105`, `heartRate`
3. Update the Wi-Fi and server credentials:

    ```cpp
    const char* ssid     = "YOUR_WIFI_SSID";
    const char* password = "YOUR_WIFI_PASSWORD";
    const String serverUrl = "http://YOUR_PC_IP/MedicalAPI/api/save_sensor_data.php";
    ```

4. Wire all sensors to the pins defined in the sketch:
   - **AD8232** → Analog pin (ECG signal)
   - **MAX30102** → I²C (SDA/SCL)
   - **DS18B20** → OneWire data pin
   - **DHT11** → Digital pin
   - **MQ135** → Analog pin
   - **Buzzer (YL-44)** → Digital output pin

5. Select board **ESP32 Dev Module** and upload the firmware.

---

### Step 5 — Run & Test the System

1. Ensure XAMPP (Apache + MySQL) is running.
2. Open the dashboard at `http://localhost/MedicalAPI/` in your browser.
3. Power on both the ESP8266 (RFID node) and ESP32 (ECG node).
4. **Scan a patient's RFID wristband** — the dashboard should switch to **"Monitoring: [Patient Name]"**.
5. Observe live ECG waveforms and vital signs.
6. Verify stored readings under **Manage Patients → History**.

---

## 📊 Features

- ✅ Continuous real-time ECG and vitals monitoring
- ✅ RFID-based patient identification (no manual lookup)
- ✅ Automatic pairing of sensor data with patient EHR
- ✅ Centralized web dashboard with live data visualization
- ✅ Historical data storage and review
- ✅ Audible alerting via buzzer for threshold breaches
- ✅ Fully local deployment (no cloud dependency)

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome. Please open an issue first to discuss proposed changes before submitting a pull request.

---

## 📄 License

This project is intended for academic and research purposes. Please review the repository license before use or distribution.

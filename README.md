# IOT-DRIVEN-REAL-TIME-MEDICAL-MONITORING-AND-PATIENT-IDENTIFICATION-SYSTEM
Internet of Things (IoT) has become a foundational enabler of intelligent, interconnected medical 
infrastructures. By linking sensors, devices, and clinical information systems, IoT facilitates seamless 
acquisition, transmission, and analysis of real-time physiological data. Despite these developments, 
two persistent challenges continue to undermine patient safety and clinical efficiency: the lack of 
continuous patient monitoring and the prevalence of patient misidentification. Conventional 
monitoring approaches rely heavily on periodic manual checks, which fail to capture sudden 
physiological fluctuations, while manual patient verification processes remain vulnerable to human 
error. These limitations can lead to missed clinical cues, delayed interventions, and preventable 
medical complications. 
 
This project introduces an integrated solution using the IoT driven real-time medical monitoring and 
patient identification system. This model combines continuous IoT-based sensing with secure digital 
identification (RFID). The system utilizes a network of wearable and bedside medical sensors such 
as heart rate monitoring, pulse rate, Oxygen saturation, etc., which continuously measures vital 
parameters such as heart rate, body temperature, and blood oxygen saturation. The sensors will be 
communicating wirelessly with a centralized monitoring platform, where data is processed, 
visualized, and analyzed to support timely clinical decision-making. To ensure accuracy and prevent 
data-mixing errors, each patient assigned a unique digital identifier implemented through RFID 
wristbands or machine-readable QR codes allowing all incoming physiological data streams, which 
automatically matches with the correct patient’s electronic health records. This system not only 
strengthens patient identity management, but also enabling uninterrupted observation of critical 
health indicators.


# STEPS-TO-REPLICATE:

1. Prerequisites\n
•	Hardware:
  •	ESP32 (ECG + vitals node)
  •	ESP8266 D1 Mini (RFID node with RDM6300)
  •	AD8232, MAX30102, DS18B20, DHT11, MQ135, YL 44 buzzer, RDM6300 module
•	Software:
  •	Arduino IDE with ESP32 and ESP8266 boards installed
  •	XAMPP (Apache + MySQL) or similar local server
  •	Web browser

2. Download the REPO
  •	Hardware/ECG_Monitoring/ECG_Monitoring.ino
  •	Hardware/RFID_Reader/RFID_Reader.ino
  •	MedicalAPI/ (PHP, HTML, JS, CSS)
  •	Database/medical_monitoring.sql

3. Set Up the Database
  •	Open http://localhost/phpmyadmin.
  •	Create database medical_monitoring.
  •	Import database/medical_monitoring.sql.
  •	Optionally edit the default patient row to match their RFID UID.

4. Configure the Web Application
  i.	Copy MedicalAPI folder into:
    •	C:\xampp\htdocs\MedicalAPI\ (Windows)
    or the equivalent htdocs folder on their system.
  ii.	Edit config.php:
    •	define('DB_HOST', 'localhost');
    •	define('DB_NAME', 'medical_monitoring');
    •	define('DB_USER', 'root');
    •	define('DB_PASS', '');
 iii.	Make sure Apache and MySQL are running in XAMPP.
 iv.	Test in browser:
 •	http://localhost/medical/api/get_patients.php
 •	http://localhost/medical/index.html

5. Configure and Upload RFID Code (ESP8266)
 i.	Open hardware/RFID_Reader/RFID_Reader.ino in Arduino IDE.
 ii.	Install required libraries: ESP8266WiFi, ESP8266HTTPClient, SoftwareSerial.
 iii.	Set Wi Fi and API URL:
  •	const char* WIFI_SSID = "YOUR_WIFI";
  •	const char* WIFI_PASS = "YOUR_PASSWORD";
  •	const char* API_BASE  = "http://YOUR_PC_IP/medical/api/get_patient_by_rfid.php?rfid=";
  •	Replace YOUR_PC_IP with the computer’s IP that runs XAMPP.
iv.	Wire RDM6300 to ESP8266:
  •	RDM6300 VCC → 3.3V
  •	RDM6300 GND → GND
  •	RDM6300 TX → D7 (GPIO13)
 v.	Select board: NodeMCU 1.0 / Wemos D1 mini and upload.

6. Configure and Upload ECG Node Code (ESP32)
 i.	Open hardware/ECG_Monitoring/ECG_Monitoring.ino.
 ii.	Install libraries: WiFi, HTTPClient, Wire, OneWire, DallasTemperature, DHT, MAX30105, heartRate.
 iii.	Set Wi Fi and server URL:
  •	const char* ssid = "YOUR_WIFI_SSID";
  •	const char* password = "YOUR_WIFI_PASSWORD";
  •	const String serverUrl = "http://YOUR_PC_IP/medical/api/save_sensor_data.php";
 iv.	Wire sensors to the pins defined in the sketch (AD8232 to analog pin, MAX30102 via I2C, DS18B20, DHT11, MQ135, buzzer).
 v.	Select board ESP32 Dev Module and upload.

7. Run and Test
  •	Start XAMPP (Apache + MySQL).
  •	Open http://localhost/medical/ in the browser.
  •	Power ESP8266 and ESP32.
  •	Scan a patient RFID card:
  •	The dashboard should switch to “Monitoring: [Patient Name]”.
  •	Watch live ECG and vitals; verify readings are stored under Manage Patients → History.

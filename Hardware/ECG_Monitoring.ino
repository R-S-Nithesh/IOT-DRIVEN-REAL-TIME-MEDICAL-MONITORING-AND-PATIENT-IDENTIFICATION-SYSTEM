/*
 * IoT-Driven Real-Time Medical Monitoring System - ECG Monitoring
 * ESP32 with AD8232, MAX30102, DS18B20, DHT11, MQ135
 * Author: R.S.Nithesh
 * Hardware: ESP32, AD8232 ECG, MAX30102, DS18B20, DHT11, MQ135, YL-44 Buzzer
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <SoftwareSerial.h>

// === WIFI & API CONFIG ===
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const String serverUrl = "http://192.168.1.100/MedicalAPI/api/save_sensor_data.php"; // Update IP

// === PIN DEFINITIONS ===
#define AD8232_PIN 34        // ECG analog pin
#define DHT_PIN 18           // DHT11 data pin
#define DS18B20_PIN 4        // Body temp DS18B20
#define MQ135_PIN 35         // Air quality analog
#define BUZZER_PIN 25        // YL-44 buzzer

// === SENSOR OBJECTS ===
OneWire oneWire(DS18B20_PIN);
DallasTemperature bodyTemp(&oneWire);
DHT dht(DHT_PIN, DHT11);
MAX30105 particleSensor;
const byte RATE_SIZE = 4; // Increase buffer size
byte rates[RATE_SIZE]; // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
bool fingerDetected = false;

// === THRESHOLDS ===
const float TEMP_LOW = 35.0;
const float TEMP_HIGH = 38.0;
const int HR_LOW = 50;
const int HR_HIGH = 120;
const int SPO2_LOW = 90;
const int AQ_HIGH = 200;

void setup() {
  Serial.begin(115200);
  
  // Initialize sensors
  Wire.begin();
  bodyTemp.begin();
  dht.begin();
  
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());
  
  // Initialize MAX30102
  if (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found!");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
}

void loop() {
  // === ECG ===
  int ecgValue = analogRead(AD8232_PIN);
  
  // === HEART RATE & SpO2 ===
  updateHeartRate();
  
  // === TEMPERATURES ===
  bodyTemp.requestTemperatures();
  float bodyTemperature = bodyTemp.getTempCByIndex(0);
  float roomTemp = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // === AIR QUALITY ===
  int airQuality = analogRead(MQ135_PIN);
  
  // === SEND DATA ===
  sendSensorData(ecgValue, beatAvg, rates[rateSpot], bodyTemperature, roomTemp, humidity, airQuality);
  
  // === ALERTS ===
  checkAlerts(bodyTemperature, beatAvg, rates[rateSpot], airQuality);
  
  delay(1000);
}

void updateHeartRate() {
  long irValue = particleSensor.getIR();
  
  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      
      beatAvg = 0;
      for (byte i = 0 ; i < RATE_SIZE ; i++)
        beatAvg += rates[i];
      beatAvg /= RATE_SIZE;
    }
  }
}

void sendSensorData(int ecg, int hr, int spo2, float bodyTemp, float roomTemp, float humidity, int aq) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    String jsonData = "{";
    jsonData += "\"ecg\":" + String(ecg) + ",";
    jsonData += "\"hr\":" + String(hr) + ",";
    jsonData += "\"spo2\":" + String(spo2) + ",";
    jsonData += "\"bodyTemp\":" + String(bodyTemp, 1) + ",";
    jsonData += "\"roomTemp\":" + String(roomTemp, 1) + ",";
    jsonData += "\"humidity\":" + String(humidity, 1) + ",";
    jsonData += "\"airQuality\":" + String(aq);
    jsonData += "}";
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully. Code: " + String(httpResponseCode));
    } else {
      Serial.println("Error sending data: " + String(httpResponseCode));
    }
    http.end();
  }
}

void checkAlerts(float bodyTemp, int hr, int spo2, int aq) {
  bool alert = false;
  String alertMsg = "";
  
  if (bodyTemp < TEMP_LOW) {
    alert = true;
    alertMsg += "Temp LOW ";
  }
  if (bodyTemp > TEMP_HIGH) {
    alert = true;
    alertMsg += "Temp HIGH ";
  }
  if (hr < HR_LOW || hr > HR_HIGH) {
    alert = true;
    alertMsg += "HR " + String(hr) + " ";
  }
  if (spo2 < SPO2_LOW) {
    alert = true;
    alertMsg += "SpO2 LOW ";
  }
  if (aq > AQ_HIGH) {
    alert = true;
    alertMsg += "Air Quality HIGH ";
  }
  
  if (alert) {
    Serial.println("ALERT: " + alertMsg);
    tone(BUZZER_PIN, 1000, 500);
  }
}

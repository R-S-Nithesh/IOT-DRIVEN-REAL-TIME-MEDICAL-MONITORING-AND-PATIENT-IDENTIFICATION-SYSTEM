/*
 * IoT-Driven Real-Time Medical Monitoring System - ECG Monitoring
 * ESP32 with AD8232, MAX30102, DS18B20, DHT11, MQ135
 * Author: R.S.Nithesh
 * Hardware: ESP32, AD8232 ECG, MAX30102, DS18B20, DHT11, MQ135, YL-44 Buzzer
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// ------ SETTINGS ------
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASS = "YOUR_PASSWORD";
const char* SERVER_URL = "http://YOUR_PC_IP/MedicalAPI/api/save_sensor_data.php";  //Changw IP address to your mobile or other device IP

// ------ PINS -------
#define PIN_ECG_OUT   36
#define PIN_ECG_LOP   27
#define PIN_ECG_LOM   26
#define PIN_DS18B20    4
#define PIN_DHT        5
#define PIN_MQ135     34
#define PIN_BUZZER    23 // <- now IO23!

// ------ OBJECTS ------
MAX30105 particleSensor;
OneWire oneWire(PIN_DS18B20);
DallasTemperature ds(&oneWire);
DHT dht(PIN_DHT, DHT11);

// ------ STATE -------
float lastBPM = 0, lastSpO2 = 0;
unsigned long lastBeat = 0, lastSend = 0;
const unsigned long SEND_INTERVAL = 1000;

#define ECG_BUF 50
int ecgBuf[ECG_BUF];
int ecgIdx = 0;

#define HR_HIGH 120
#define HR_LOW   50
#define SPO2_LOW 90
#define BODY_HIGH 38.5
#define BODY_LOW  35.0

// ------ I2C BUS RECOVERY ------
#define SDA_PIN 21
#define SCL_PIN 22
void i2cBusRecover() {
  pinMode(SCL_PIN, OUTPUT_OPEN_DRAIN);
  pinMode(SDA_PIN, INPUT_PULLUP);
  digitalWrite(SCL_PIN, HIGH);
  for (int i = 0; i < 9; i++) {
    digitalWrite(SCL_PIN, LOW);  delayMicroseconds(5);
    digitalWrite(SCL_PIN, HIGH); delayMicroseconds(5);
  }
}

void beep(uint16_t freq, uint16_t ms) {
  tone(PIN_BUZZER, freq);
  delay(ms);
  noTone(PIN_BUZZER);
}

void smartConnectWiFi() {
  WiFi.mode(WIFI_STA);
  if(WiFi.status() == WL_CONNECTED) WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  for(int i=0; WiFi.status()!=WL_CONNECTED && i<60; i++) {
    delay(250);
    Serial.print(".");
  }
  if(WiFi.status()==WL_CONNECTED) {
    Serial.println("\nWiFi: Connected ✔ IP:" + WiFi.localIP().toString());
    beep(1600, 80); delay(60); beep(2500, 60);
  } else {
    Serial.println("\nWiFi: Connect FAILED!");
    beep(600, 300);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_ECG_OUT, INPUT);
  pinMode(PIN_ECG_LOP, INPUT);
  pinMode(PIN_ECG_LOM, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  Serial.println("I2C recovery...");
  i2cBusRecover();
  Wire.begin(SDA_PIN, SCL_PIN);

  ds.begin();
  dht.begin();

  Serial.println("[MAX30102] Initializing...");
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD, 0x57)) {
    Serial.println("ERROR: MAX30102 not found!");
    beep(300,450);
  } else {
    // Conservative startup sequence
    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x1F);
    particleSensor.setPulseAmplitudeIR(0x1F);
    particleSensor.setPulseAmplitudeGreen(0);
    particleSensor.setSampleRate(100);
    particleSensor.setFIFOAverage(4);
    particleSensor.setLEDMode(2); // Red + IR
    particleSensor.setADCRange(16384);
    Serial.println("MAX30102 OK");
  }

  smartConnectWiFi();
}

void loop() {
  // ------ ECG buffer ------
  int ecg = analogRead(PIN_ECG_OUT);
  ecgBuf[ecgIdx++] = ecg; if(ecgIdx>=ECG_BUF) ecgIdx=0;

  // ------ HR & SpO2 ------
  long irValue  = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  bool fingerOn = (irValue > 8000 && redValue > 8000);

  if (fingerOn) {
    if (checkForBeat(irValue)) {
      unsigned long delta = millis() - lastBeat;
      lastBeat = millis();
      float bpm = 60.0 / (delta / 1000.0);
      if (bpm > 40 && bpm < 180) lastBPM = bpm;
    }
    float ratio = (float)redValue / (float)irValue;
    float spo2 = 110 - 25 * ratio;
    if (spo2 > 70 && spo2 < 100) lastSpO2 = spo2;
  } else {
    lastBPM = 0; lastSpO2 = 0;
  }

  // ------ Read Other Sensors ------
  ds.requestTemperatures();
  float bodyTemp = ds.getTempCByIndex(0), roomTemp = dht.readTemperature(), humidity = dht.readHumidity();
  int airQuality = analogRead(PIN_MQ135);

  // ------ Alerts & Send ------
  if (millis()-lastSend >= SEND_INTERVAL) {
    lastSend = millis();
    int ecgAvg=0;
    for(int i=0;i<ECG_BUF;i++) ecgAvg+=ecgBuf[i]; ecgAvg/=ECG_BUF;

    int hr = (lastBPM>0)?(int)(lastBPM+0.5):0, spo2 = (lastSpO2>0)?(int)(lastSpO2+0.5):0;
    Serial.printf("ECG=%d HR=%d SpO2=%d B=%.1f R=%.1f H=%.1f AQ=%d\n",
        ecgAvg, hr, spo2, bodyTemp, roomTemp, humidity, airQuality);

    // Buzzer for alerts
    if(hr > HR_HIGH) { Serial.println("ALERT: HR HIGH"); beep(1600, 100);}
    else if(hr > 0 && hr < HR_LOW) { Serial.println("ALERT: HR LOW"); beep(600, 100);}
    if(spo2 > 0 && spo2 < SPO2_LOW) { Serial.println("ALERT: SpO2 LOW"); beep(900, 120);}
    if(bodyTemp > BODY_HIGH) { Serial.println("ALERT: TEMP HIGH"); beep(1700, 100);}
    else if(bodyTemp > 20 && bodyTemp < BODY_LOW) { Serial.println("ALERT: TEMP LOW"); beep(700, 120);}

    // Reliable WiFi (self-reconnect)
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFi] Reconnecting...");
      smartConnectWiFi();
    }

    // Send to server
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(SERVER_URL);
      http.addHeader("Content-Type", "application/json");
      String json=String("{")+
        "\"ecg\":"+ecgAvg+","+
        "\"hr\":"+hr+","+
        "\"spo2\":"+spo2+","+
        "\"bodyTemp\":"+String(bodyTemp,1)+","+
        "\"roomTemp\":"+String(roomTemp,1)+","+
        "\"humidity\":"+String(humidity,1)+","+
        "\"airQuality\":"+airQuality+"}";
      int code = http.POST(json); if(code!=200) Serial.printf("POST %d\n",code);
      http.end();
    }
  }
  delay(20);
}

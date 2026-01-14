/*
 * IoT-Driven Real-Time Medical Monitoring System - RFID Reader
 * ESP8266 D1 Mini with RDM6300 RFID Reader
 * Author: R.S.Nithesh
 * Scans RFID cards → Activates patient session on web server
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

// ========== WiFi Configuration ==========
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASS = "YOUR_PASSWORD";
const char* API_BASE = "http://YOUR_PC_IP/medical/api/get_patient_by_rfid.php?rfid="; //Change IP address to your mobile or other device IP

// ========== Pin Definitions ==========
#define PIN_RFID_RX   D7   // GPIO13 - RDM6300 TX → ESP8266 RX  //If not working Just Interchange the RX and TX pins.
#define PIN_RFID_TX   D6   // GPIO12 - Not used
#define PIN_LED       D5   // GPIO14 - Status LED

// ========== RDM6300 UART (SoftwareSerial) ==========
SoftwareSerial rfidSerial(PIN_RFID_RX, PIN_RFID_TX);

// ========== State Variables ==========
unsigned long lastScan = 0;
const long SCAN_COOLDOWN = 3000;
String lastUID = "";

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n========================================");
  Serial.println("  RFID Patient Scanner (RDM6300)");
  Serial.println("========================================\n");

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  rfidSerial.begin(9600);
  Serial.println("✓ RDM6300 initialized (D7 RX @ 9600 baud)");
  Serial.println("  Format: EM-4100 125 kHz");

  // LED startup (3 blinks)
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH); delay(100);
    digitalWrite(PIN_LED, LOW); delay(100);
  }

  // WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 30) {
    delay(500);
    Serial.print(".");
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    timeout++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ WiFi connected: " + WiFi.localIP().toString());
    digitalWrite(PIN_LED, HIGH); delay(500); digitalWrite(PIN_LED, LOW);
  } else {
    Serial.println("✗ WiFi connection failed!");
    for (int i = 0; i < 10; i++) {
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      delay(100);
    }
  }

  Serial.println("\n========================================");
  Serial.println("  Ready! Waiting for RFID cards...");
  Serial.println("========================================\n");
}

void loop() {
  if (rfidSerial.available()) {
    byte startByte = rfidSerial.read();

    if (startByte == 0x02) {
      String uid = "";
      
      for (int i = 0; i < 10; i++) {
        unsigned long waitStart = millis();
        while (!rfidSerial.available()) {
          if (millis() - waitStart > 100) {
            Serial.println("❌ RFID read timeout");
            return;
          }
          delay(1);
        }
        char c = rfidSerial.read();
        uid += c;
      }

      // Read checksum
      for (int i = 0; i < 2; i++) {
        while (!rfidSerial.available()) delay(1);
        rfidSerial.read();
      }

      // Flush remaining
      while (rfidSerial.available()) rfidSerial.read();

      uid.toUpperCase();

      if (uid == lastUID && (millis() - lastScan < SCAN_COOLDOWN)) {
        Serial.println("   (duplicate, ignored)");
        return;
      }

      lastUID = uid;
      lastScan = millis();

      digitalWrite(PIN_LED, HIGH);
      
      Serial.println("\n📱 RFID Card Detected!");
      Serial.println("   UID: " + uid);
      
      scanPatient(uid);
      
      delay(300);
      digitalWrite(PIN_LED, LOW);
      delay(200);
      digitalWrite(PIN_LED, HIGH);
      delay(200);
      digitalWrite(PIN_LED, LOW);
    }
  }
}

void scanPatient(String uid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi disconnected");
    blinkError();
    return;
  }

  WiFiClient client;
  HTTPClient http;
  String url = API_BASE + uid;

  Serial.println("   Sending to XAMPP...");
  Serial.println("   GET: " + url);

  if (http.begin(client, url)) {
    int httpCode = http.GET();
    String payload = http.getString();

    Serial.println("   ────────────────────────────────");
    
    if (httpCode == 200) {
      Serial.println("   ✓ HTTP 200 - Patient Loaded");
      Serial.println("   " + payload.substring(0, min(150, (int)payload.length())));
      
      digitalWrite(PIN_LED, HIGH); delay(100);
      digitalWrite(PIN_LED, LOW); delay(100);
      digitalWrite(PIN_LED, HIGH); delay(100);
      digitalWrite(PIN_LED, LOW);
      
    } else if (httpCode == 404) {
      Serial.println("   ❌ HTTP 404 - Patient Not Found");
      blinkError();
      
    } else {
      Serial.printf("   ❌ HTTP %d - Server Error\n", httpCode);
      blinkError();
    }
    
    Serial.println("   ────────────────────────────────\n");
    http.end();
    
  } else {
    Serial.println("   ❌ HTTP connection failed");
    blinkError();
  }
}

void blinkError() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH); delay(100);
    digitalWrite(PIN_LED, LOW); delay(100);
  }
}


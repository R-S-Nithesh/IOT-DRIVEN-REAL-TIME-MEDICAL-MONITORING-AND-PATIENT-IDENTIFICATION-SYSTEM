/*
 * IoT-Driven Real-Time Medical Monitoring System - RFID Reader
 * ESP8266 D1 Mini with RDM6300 RFID Reader
 * Author: R.S.Nithesh
 * Scans RFID cards → Activates patient session on web server
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

// === WIFI & API CONFIG ===
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const String serverUrl = "http://192.168.1.100/MedicalAPI/api/get_patient_by_rfid.php"; // Update IP

// === RFID SETUP ===
SoftwareSerial rfid(2, 14); // RX=D4(GPIO2), TX=D5(GPIO14)    //Sometimes Reverse the pins
String rfidUID = "";

void setup() {
  Serial.begin(9600);
  rfid.begin(9600);
  
  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());
  
  Serial.println("RFID Reader Ready - Scan patient card...");
}

void loop() {
  // Read RFID
  if (rfid.available()) {
    rfidUID = "";
    while (rfid.available()) {
      char c = rfid.read();
      if (c != '\n' && c != '\r') {
        rfidUID += c;
      }
    }
    
    // Remove null terminator
    if (rfidUID.length() > 0) {
      rfidUID.trim();
      Serial.println("RFID UID detected: " + rfidUID);
      
      // Send to server
      activatePatientSession(rfidUID);
      
      delay(2000); // Prevent multiple scans
    }
  }
  
  delay(100);
}

void activatePatientSession(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverUrl + "?rfid=" + uid;
    http.begin(url);
    
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server response: " + response);
      
      if (response.indexOf("\"success\":true") > 0) {
        Serial.println("✅ Patient session activated successfully!");
      } else {
        Serial.println("❌ Patient not found: " + uid);
      }
    } else {
      Serial.println("HTTP Error: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

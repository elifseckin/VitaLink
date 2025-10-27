/*
 * EmergencySystem.cpp
 * 
 * Emergency system implementation
 */

#include <Arduino.h>
#include "EmergencySystem.h"

EmergencySystem::EmergencySystem(int buzzer, int led, RF24* radioModule) {
  buzzerPin = buzzer;
  ledPin = led;
  radio = radioModule;
  isAlertActive = false;
  alertStartTime = 0;
  buzzerInterval = 1000;
  ledInterval = 500;
  buzzerState = false;
  ledState = false;
  
  // Initialize device ID with default value
  strcpy(currentAlert.deviceId, "HRM_001");
}

void EmergencySystem::begin() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);
  
  Serial.println("Emergency system initialized");
}

void EmergencySystem::update() {
  if (isAlertActive) {
    updateBuzzer();
    updateLED();
    
    // Periodic signal transmission
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 10000) { // Every 10 seconds
      sendEmergencySignal();
      lastSend = millis();
    }
  }
  
  // Check incoming emergency messages
  EmergencyAlert receivedAlert;
  if (checkIncomingAlerts(receivedAlert)) {
    Serial.println("=== EXTERNAL EMERGENCY DETECTED ===");
    Serial.print("Device ID: ");
    Serial.println(receivedAlert.deviceId);
    Serial.print("Location: ");
    Serial.print(receivedAlert.latitude, 6);
    Serial.print(", ");
    Serial.println(receivedAlert.longitude, 6);
    Serial.print("Heart Rate: ");
    Serial.print(receivedAlert.heartRate);
    Serial.println(" BPM");
    Serial.print("Alert Level: ");
    Serial.println(receivedAlert.alertLevel);
    
    // Visual warning for external emergency
    flashWarningPattern();
  }
}

void EmergencySystem::triggerAlert(int heartRate, float lat, float lon, int level) {
  if (!isAlertActive) {
    Serial.println("*** EMERGENCY TRIGGERED ***");
    
    isAlertActive = true;
    alertStartTime = millis();
    
    // Prepare alert data
    currentAlert.heartRate = heartRate;
    currentAlert.latitude = lat;
    currentAlert.longitude = lon;
    currentAlert.alertLevel = level;
    currentAlert.timestamp = millis();
    
    generateAlertPattern(level);
    
    Serial.print("Kalp Ritmi: ");
    Serial.print(heartRate);
    Serial.println(" BPM");
    Serial.print("Konum: ");
    Serial.print(lat, 6);
    Serial.print(", ");
    Serial.println(lon, 6);
    Serial.print("Uyarı Seviyesi: ");
    Serial.println(level);
  }
}

void EmergencySystem::stopAlert() {
  if (isAlertActive) {
    isAlertActive = false;
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    Serial.println("Emergency alert stopped");
  }
}

bool EmergencySystem::isActive() {
  return isAlertActive;
}

void EmergencySystem::sendEmergencySignal() {
  radio->stopListening();
  
  bool result = radio->write(&currentAlert, sizeof(EmergencyAlert));
  
  radio->startListening();
  
  if (result) {
    Serial.println("Emergency signal sent");
  } else {
    Serial.println("Signal failed to send!");
  }
}

bool EmergencySystem::checkIncomingAlerts(EmergencyAlert& receivedAlert) {
  if (radio->available()) {
    radio->read(&receivedAlert, sizeof(EmergencyAlert));
    
    // Kendi gönderdiğimiz mesajı filtrele
    if (strcmp(receivedAlert.deviceId, currentAlert.deviceId) != 0) {
      return true;
    }
  }
  return false;
}

void EmergencySystem::setDeviceId(const char* id) {
  strncpy(currentAlert.deviceId, id, 7);
  currentAlert.deviceId[7] = '\0';
}

void EmergencySystem::updateBuzzer() {
  static unsigned long lastBuzzerUpdate = 0;
  
  if (millis() - lastBuzzerUpdate >= buzzerInterval) {
    buzzerState = !buzzerState;
    digitalWrite(buzzerPin, buzzerState ? HIGH : LOW);
    lastBuzzerUpdate = millis();
  }
}

void EmergencySystem::updateLED() {
  static unsigned long lastLEDUpdate = 0;
  
  if (millis() - lastLEDUpdate >= ledInterval) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    lastLEDUpdate = millis();
  }
}

void EmergencySystem::generateAlertPattern(int level) {
  switch (level) {
    case 1: // Düşük risk
      buzzerInterval = 2000;  // 2 saniye
      ledInterval = 1000;     // 1 saniye
      break;
      
    case 2: // Orta risk  
      buzzerInterval = 1000;  // 1 saniye
      ledInterval = 500;      // 0.5 saniye
      break;
      
    case 3: // Yüksek risk
      buzzerInterval = 300;   // 0.3 saniye
      ledInterval = 200;      // 0.2 saniye
      break;
      
    default:
      buzzerInterval = 1000;
      ledInterval = 500;
      break;
  }
}

void EmergencySystem::flashWarningPattern() {
  // Dış acil durum için 3 kez hızlı yanıp sönme
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
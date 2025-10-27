/*
 * Heart Rate Monitoring and Emergency Alert System
 * PlatformIO Simple Version
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include <SPI.h>

// Pin definitions
#define PULSE_INPUT A0
#define PULSE_BLINK 13
#define BUZZER_PIN 6
#define GPS_RX 7
#define GPS_TX 8
#define RF24_CE 9
#define RF24_CSN 10

// Constants
const int MIN_HEART_RATE = 60;
const int MAX_HEART_RATE = 100;

// Global variables
int BPM = 0;
int Signal = 0;
boolean buzzerActive = false;

// Timer variables
unsigned long lastTestTime = 0;
unsigned long buzzerStartTime = 0;

// GPS and communication objects
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
RF24 radio(RF24_CE, RF24_CSN);

// GPS coordinates
float latitude = 41.0082;
float longitude = 28.9784;

// Communication structure
struct EmergencyData {
  float lat;
  float lon;
  int heartRate;
  unsigned long timestamp;
  char deviceId[8];
};

EmergencyData emergencyPacket;
const uint64_t addresses[] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// Function declarations
void triggerAlert();
void stopAlert();
void manageBuzzer();
void checkIncomingMessages();

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  
  // Pin modes
  pinMode(PULSE_BLINK, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // RF24 initialization
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  // Generate device ID
  sprintf(emergencyPacket.deviceId, "HRM_%03d", (int)random(100, 999));
  
  Serial.println("=== HEART RATE MONITORING SYSTEM ===");
  Serial.println("System initialized!");
  Serial.print("Device ID: ");
  Serial.println(emergencyPacket.deviceId);
  Serial.println("Monitoring started...");
}

void loop() {
  // Generate test heart rate (every 1 second)
  if (millis() - lastTestTime > 1000) {
    lastTestTime = millis();
    
    // Read analog value and calculate BPM
    Signal = analogRead(PULSE_INPUT);
    BPM = 60 + (Signal % 40); // Test value range 60-100
    
    Serial.print("Heart Rate: ");
    Serial.print(BPM);
    Serial.print(" BPM (Sensor: ");
    Serial.print(Signal);
    Serial.println(")");
    
    // Emergency condition check
    if (BPM < MIN_HEART_RATE || BPM > MAX_HEART_RATE) {
      triggerAlert();
    } else {
      stopAlert();
    }
  }
  
  // Buzzer control
  manageBuzzer();
  
  // Communication control
  checkIncomingMessages();
  
  delay(50);
}

void triggerAlert() {
  if (!buzzerActive) {
    buzzerActive = true;
    buzzerStartTime = millis();
    Serial.println("🚨 EMERGENCY ALERT!");
    
    // Prepare emergency packet
    emergencyPacket.lat = latitude;
    emergencyPacket.lon = longitude;
    emergencyPacket.heartRate = BPM;
    emergencyPacket.timestamp = millis();
    
    // Send RF signal
    radio.stopListening();
    boolean result = radio.write(&emergencyPacket, sizeof(EmergencyData));
    radio.startListening();
    
    if (result) {
      Serial.println("📡 Emergency signal sent");
    }
  }
}

void stopAlert() {
  if (buzzerActive) {
    buzzerActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(PULSE_BLINK, LOW);
    Serial.println("✅ Alert stopped");
  }
}

void manageBuzzer() {
  if (buzzerActive) {
    // 500ms on/off pattern
    unsigned long elapsed = millis() - buzzerStartTime;
    if ((elapsed / 500) % 2 == 0) {
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(PULSE_BLINK, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(PULSE_BLINK, LOW);
    }
  }
}

void checkIncomingMessages() {
  if (radio.available()) {
    EmergencyData receivedData;
    radio.read(&receivedData, sizeof(EmergencyData));
    
    // Filter our own messages
    if (strcmp(receivedData.deviceId, emergencyPacket.deviceId) != 0) {
      Serial.println("🚨 EXTERNAL EMERGENCY DETECTED!");
      Serial.print("Device: ");
      Serial.println(receivedData.deviceId);
      Serial.print("Heart Rate: ");
      Serial.println(receivedData.heartRate);
      
      // Visual alert
      for (int i = 0; i < 6; i++) {
        digitalWrite(PULSE_BLINK, HIGH);
        delay(100);
        digitalWrite(PULSE_BLINK, LOW);
        delay(100);
      }
    }
  }
}
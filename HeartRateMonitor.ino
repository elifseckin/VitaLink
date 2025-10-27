/*
 * HeartRateMonitor.ino
 * 
 * Heart Rate Monitoring and Emergency Alert System
 * Main file for Arduino IDE compilation
 */

// Library includes - for Arduino IDE
#include <SoftwareSerial.h>
#include <RF24.h>
#include <SPI.h>

// Pin definitions
#define PULSE_INPUT A0    // Heart rate sensor analog input
#define PULSE_BLINK 13    // LED indicator (heartbeat)
#define PULSE_FADE 5      // PWM LED (rhythm indicator)
#define BUZZER_PIN 6      // Buzzer output
#define GPS_RX 7          // GPS module RX
#define GPS_TX 8          // GPS module TX
#define RF24_CE 9         // RF24 CE pin
#define RF24_CSN 10       // RF24 CSN pin

// Constants
const int THRESHOLD = 530;        
const int MIN_HEART_RATE = 60;    
const int MAX_HEART_RATE = 100;   

// Global variables
volatile int BPM = 0;                 
volatile int Signal = 0;              
volatile int IBI = 600;           
volatile boolean Pulse = false;   
volatile boolean QS = false;      

// Timer variables
unsigned long lastBeatTime = 0;
unsigned long lastGPSTime = 0;
unsigned long lastCommTime = 0;
unsigned long buzzerStartTime = 0;
boolean buzzerActive = false;

// GPS and communication objects
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
RF24 radio(RF24_CE, RF24_CSN);

// GPS coordinates
float latitude = 0.0;
float longitude = 0.0;
boolean gpsValid = false;

// Communication structure
struct EmergencyData {
  float lat;
  float lon;
  int heartRate;
  unsigned long timestamp;
  char deviceId[8];
};

EmergencyData emergencyPacket;
const uint64_t addresses[][2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  
  // Pin modes
  pinMode(PULSE_BLINK, OUTPUT);
  pinMode(PULSE_FADE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // RF24 initialization
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  // Startup message
  Serial.println("=== HEART RATE MONITORING SYSTEM ===");
  Serial.println("System started...");
  Serial.println("Sensor calibration in progress...");
  
  delay(3000); // Calibration delay
  
  Serial.println("Calibration completed!");
  Serial.println("Monitoring started...");
  
  // Generate device ID
  sprintf(emergencyPacket.deviceId, "HRM_%03d", random(100, 999));
  Serial.print("Device ID: ");
  Serial.println(emergencyPacket.deviceId);
}

void loop() {
  // Read analog value (for heart rate simulation)
  Signal = analogRead(PULSE_INPUT);
  
  // Simple heart rate detection (for testing without real sensor)
  static unsigned long lastTest = 0;
  if (millis() - lastTest > 1000) { // Test every second
    lastTest = millis();
    
    // Generate test BPM value (60-100 range)
    BPM = 60 + (Signal % 40);
    
    Serial.print("Test Heart Rate: ");
    Serial.print(BPM);
    Serial.print(" BPM (Sensor: ");
    Serial.print(Signal);
    Serial.println(")");
    
    // Emergency check
    if (BPM < MIN_HEART_RATE) {
      Serial.println("⚠️  WARNING: Low heart rate!");
      triggerAlert();
    } else if (BPM > MAX_HEART_RATE) {
      Serial.println("⚠️  WARNING: High heart rate!");
      triggerAlert();
    } else {
      Serial.println("✅ Heart rate normal");
      stopAlert();
    }
  }
  
  // GPS simulation
  if (millis() - lastGPSTime > 5000) { // Every 5 seconds
    lastGPSTime = millis();
    // Test coordinates (Istanbul)
    latitude = 41.0082 + (random(-100, 100) / 10000.0);
    longitude = 28.9784 + (random(-100, 100) / 10000.0);
    gpsValid = true;
    
    Serial.print("📍 GPS Location: ");
    Serial.print(latitude, 6);
    Serial.print(", ");
    Serial.println(longitude, 6);
  }
  
  // Buzzer control
  if (buzzerActive) {
    unsigned long elapsed = millis() - buzzerStartTime;
    if ((elapsed / 500) % 2 == 0) { // 500ms on/off
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(PULSE_BLINK, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(PULSE_BLINK, LOW);
    }
  }
  
  // RF communication check
  if (radio.available()) {
    EmergencyData receivedData;
    radio.read(&receivedData, sizeof(EmergencyData));
    
    Serial.println("🚨 EMERGENCY SIGNAL RECEIVED!");
    Serial.print("Device: ");
    Serial.println(receivedData.deviceId);
    Serial.print("Heart Rate: ");
    Serial.println(receivedData.heartRate);
    
    // LED blink for external emergency
    for (int i = 0; i < 6; i++) {
      digitalWrite(PULSE_BLINK, HIGH);
      delay(100);
      digitalWrite(PULSE_BLINK, LOW);
      delay(100);
    }
  }
  
  // Main loop delay
  delay(50);
}

void triggerAlert() {
  if (!buzzerActive) {
    buzzerActive = true;
    buzzerStartTime = millis();
    Serial.println("🚨 EMERGENCY ALERT ACTIVE!");
    
    // Prepare and send emergency packet
    if (gpsValid && millis() - lastCommTime > 3000) {
      emergencyPacket.lat = latitude;
      emergencyPacket.lon = longitude;
      emergencyPacket.heartRate = BPM;
      emergencyPacket.timestamp = millis();
      
      radio.stopListening();
      boolean result = radio.write(&emergencyPacket, sizeof(EmergencyData));
      radio.startListening();
      
      if (result) {
        Serial.println("📡 Emergency signal sent");
      } else {
        Serial.println("❌ Signal failed to send");
      }
      lastCommTime = millis();
    }
  }
}

void stopAlert() {
  if (buzzerActive) {
    buzzerActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(PULSE_BLINK, LOW);
    Serial.println("✅ Emergency alert stopped");
  }
}
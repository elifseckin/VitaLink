/*
 * EmergencySystem.h
 * 
 * Class for emergency management
 * Buzzer control, LED indicators and communication coordination
 */

#ifndef EMERGENCY_SYSTEM_H
#define EMERGENCY_SYSTEM_H

#include <Arduino.h>
#include <RF24.h>

struct EmergencyAlert {
  float latitude;
  float longitude;
  int heartRate;
  unsigned long timestamp;
  char deviceId[8];
  int alertLevel;  // 1: Low, 2: Medium, 3: High
};

class EmergencySystem {
private:
  int buzzerPin;
  int ledPin;
  RF24* radio;
  bool isAlertActive;
  unsigned long alertStartTime;
  EmergencyAlert currentAlert;
  
  // Alert patterns
  unsigned long buzzerInterval;
  unsigned long ledInterval;
  bool buzzerState;
  bool ledState;
  
public:
  EmergencySystem(int buzzer, int led, RF24* radioModule);
  
  void begin();
  void update();
  void triggerAlert(int heartRate, float lat, float lon, int level = 2);
  void stopAlert();
  bool isActive();
  
  // Communication methods
  void sendEmergencySignal();
  bool checkIncomingAlerts(EmergencyAlert& receivedAlert);
  void setDeviceId(const char* id);
  
private:
  void updateBuzzer();
  void updateLED();
  void generateAlertPattern(int level);
};

#endif
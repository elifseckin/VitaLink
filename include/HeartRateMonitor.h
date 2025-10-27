/*
 * HeartRateMonitor.h
 * 
 * Helper class for heart rate sensor
 * Processes analog signal from pulse sensor to calculate BPM
 */

#ifndef HEART_RATE_MONITOR_H
#define HEART_RATE_MONITOR_H

#include <Arduino.h>

class HeartRateMonitor {
private:
  int sensorPin;
  int threshold;
  int sampleRate;
  unsigned long lastBeatTime;
  int beatCount;
  int bpm;
  bool isCalibrated;
  
  // Signal processing variables
  int signalBuffer[10];
  int bufferIndex;
  int avgSignal;
  
public:
  HeartRateMonitor(int pin, int thresh = 530);
  
  void begin();
  void update();
  int getBPM();
  bool isHeartBeatDetected();
  void calibrate(int duration = 5000);
  bool isValidReading();
  void reset();
  
private:
  void processSignal(int signal);
  int getAverageSignal();
  bool detectBeat(int signal);
};

#endif
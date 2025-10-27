/*
 * HeartRateMonitor.cpp
 * 
 * Heart rate sensor implementation
 */

#include <Arduino.h>
#include "HeartRateMonitor.h"

HeartRateMonitor::HeartRateMonitor(int pin, int thresh) {
  sensorPin = pin;
  threshold = thresh;
  sampleRate = 500; // Hz
  lastBeatTime = 0;
  beatCount = 0;
  bpm = 0;
  isCalibrated = false;
  bufferIndex = 0;
  avgSignal = 0;
  
  // Clear buffer
  for (int i = 0; i < 10; i++) {
    signalBuffer[i] = 0;
  }
}

void HeartRateMonitor::begin() {
  pinMode(sensorPin, INPUT);
  Serial.println("Heart rate sensor initialized");
}

void HeartRateMonitor::update() {
  int signal = analogRead(sensorPin);
  processSignal(signal);
  
  if (detectBeat(signal)) {
    unsigned long currentTime = millis();
    if (lastBeatTime > 0 && isCalibrated) {
      int interval = currentTime - lastBeatTime;
      if (interval > 300 && interval < 2000) { // Geçerli kalp atışı aralığı
        bpm = 60000 / interval;
        beatCount++;
      }
    }
    lastBeatTime = currentTime;
  }
}

int HeartRateMonitor::getBPM() {
  return bpm;
}

bool HeartRateMonitor::isHeartBeatDetected() {
  int currentSignal = analogRead(sensorPin);
  return (currentSignal > threshold && currentSignal > avgSignal + 50);
}

void HeartRateMonitor::calibrate(int duration) {
  Serial.println("Calibrating heart rate sensor...");
  Serial.println("Please stay calm and place your finger on the sensor");
  
  unsigned long startTime = millis();
  long totalSignal = 0;
  int sampleCount = 0;
  
  while (millis() - startTime < duration) {
    int signal = analogRead(sensorPin);
    totalSignal += signal;
    sampleCount++;
    
    // Progress indicator
    if (sampleCount % 100 == 0) {
      Serial.print(".");
    }
    
    delay(10);
  }
  
  avgSignal = totalSignal / sampleCount;
  threshold = avgSignal + 30; // Dynamic threshold
  isCalibrated = true;
  
  Serial.println();
  Serial.print("Calibration completed. Average signal: ");
  Serial.print(avgSignal);
  Serial.print(", Threshold: ");
  Serial.println(threshold);
}

bool HeartRateMonitor::isValidReading() {
  return (bpm >= 40 && bpm <= 180 && isCalibrated);
}

void HeartRateMonitor::reset() {
  bpm = 0;
  beatCount = 0;
  lastBeatTime = 0;
  bufferIndex = 0;
  
  for (int i = 0; i < 10; i++) {
    signalBuffer[i] = 0;
  }
}

void HeartRateMonitor::processSignal(int signal) {
  // Signal data for circular buffer
  signalBuffer[bufferIndex] = signal;
  bufferIndex = (bufferIndex + 1) % 10;
  
  // Calculate moving average
  avgSignal = getAverageSignal();
}

int HeartRateMonitor::getAverageSignal() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += signalBuffer[i];
  }
  return sum / 10;
}

bool HeartRateMonitor::detectBeat(int signal) {
  static bool beatDetected = false;
  static unsigned long lastPeak = 0;
  
  // Peak detection algorithm
  if (signal > threshold && signal > avgSignal + 20) {
    if (!beatDetected && millis() - lastPeak > 300) { // Minimum 300ms between beats
      beatDetected = true;
      lastPeak = millis();
      return true;
    }
  } else if (signal < avgSignal) {
    beatDetected = false;
  }
  
  return false;
}
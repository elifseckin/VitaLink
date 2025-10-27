/*
 * GPSModule.h
 * 
 * Helper class for GPS module
 * Reads GPS data via NMEA protocol and parses coordinates
 */

#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <SoftwareSerial.h>

struct GPSCoordinate {
  float latitude;
  float longitude;
  bool isValid;
  unsigned long timestamp;
};

class GPSModule {
private:
  SoftwareSerial* gpsSerial;
  GPSCoordinate currentLocation;
  String nmeaBuffer;
  
public:
  GPSModule(int rxPin, int txPin, int baudRate = 9600);
  
  void begin();
  void update();
  GPSCoordinate getLocation();
  bool isLocationValid();
  float getLatitude();
  float getLongitude();
  
private:
  bool parseNMEA(String sentence);
  float convertDMStoDD(String coordinate);
  bool isValidCoordinate(float lat, float lon);
};

#endif
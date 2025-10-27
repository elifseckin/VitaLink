/*
 * GPSModule.cpp
 * 
 * GPS module implementation
 */

#include <Arduino.h>
#include "GPSModule.h"

GPSModule::GPSModule(int rxPin, int txPin, int baudRate) {
  gpsSerial = new SoftwareSerial(rxPin, txPin);
  currentLocation.latitude = 0.0;
  currentLocation.longitude = 0.0;
  currentLocation.isValid = false;
  currentLocation.timestamp = 0;
  nmeaBuffer = "";
}

void GPSModule::begin() {
  gpsSerial->begin(9600);
  Serial.println("GPS module initialized");
  Serial.println("Waiting for GPS signal...");
}

void GPSModule::update() {
  while (gpsSerial->available() > 0) {
    char c = gpsSerial->read();
    
    if (c == '\n') {
      // Parse NMEA sentence
      if (parseNMEA(nmeaBuffer)) {
        currentLocation.timestamp = millis();
        Serial.print("GPS güncellemesi: ");
        Serial.print(currentLocation.latitude, 6);
        Serial.print(", ");
        Serial.println(currentLocation.longitude, 6);
      }
      nmeaBuffer = "";
    } else if (c != '\r') {
      nmeaBuffer += c;
    }
  }
  
  // GPS timeout kontrolü (10 saniye)
  if (millis() - currentLocation.timestamp > 10000) {
    currentLocation.isValid = false;
  }
}

GPSCoordinate GPSModule::getLocation() {
  return currentLocation;
}

bool GPSModule::isLocationValid() {
  return currentLocation.isValid;
}

float GPSModule::getLatitude() {
  return currentLocation.latitude;
}

float GPSModule::getLongitude() {
  return currentLocation.longitude;
}

bool GPSModule::parseNMEA(String sentence) {
  if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
    // GPGGA formatı: $GPGGA,time,lat,N/S,lon,E/W,quality,satellites,hdop,altitude,M,geoid,M,dgps_age,dgps_id*checksum
    
    int commaPositions[15];
    int commaCount = 0;
    
    // Find comma positions
    for (int i = 0; i < sentence.length() && commaCount < 15; i++) {
      if (sentence.charAt(i) == ',') {
        commaPositions[commaCount] = i;
        commaCount++;
      }
    }
    
    if (commaCount >= 6) {
      // Quality check (0 = invalid, 1+ = valid)
      String quality = sentence.substring(commaPositions[5] + 1, commaPositions[6]);
      if (quality.toInt() > 0) {
        
        // Latitude parsing
        String latStr = sentence.substring(commaPositions[1] + 1, commaPositions[2]);
        String latDir = sentence.substring(commaPositions[2] + 1, commaPositions[3]);
        
        // Longitude parsing  
        String lonStr = sentence.substring(commaPositions[3] + 1, commaPositions[4]);
        String lonDir = sentence.substring(commaPositions[4] + 1, commaPositions[5]);
        
        if (latStr.length() > 0 && lonStr.length() > 0) {
          float lat = convertDMStoDD(latStr);
          float lon = convertDMStoDD(lonStr);
          
          // Direction check
          if (latDir == "S") lat = -lat;
          if (lonDir == "W") lon = -lon;
          
          if (isValidCoordinate(lat, lon)) {
            currentLocation.latitude = lat;
            currentLocation.longitude = lon;
            currentLocation.isValid = true;
            return true;
          }
        }
      }
    }
  }
  
  return false;
}

float GPSModule::convertDMStoDD(String coordinate) {
  if (coordinate.length() < 4) return 0.0;
  
  // Convert from DDMM.MMMM format to DD.DDDDDD
  int dotPos = coordinate.indexOf('.');
  if (dotPos < 2) return 0.0;
  
  String degreesStr = coordinate.substring(0, dotPos - 2);
  String minutesStr = coordinate.substring(dotPos - 2);
  
  float degrees = degreesStr.toFloat();
  float minutes = minutesStr.toFloat();
  
  return degrees + (minutes / 60.0);
}

bool GPSModule::isValidCoordinate(float lat, float lon) {
  // Valid coordinate ranges
  return (lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0 && 
          lat != 0.0 && lon != 0.0);
}
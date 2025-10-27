# VitaLink - Smart Health Monitoring System

VitaLink is an advanced Arduino-based health monitoring system that provides real-time heart rate tracking. When abnormal conditions are detected, it delivers audio/visual alerts and sends emergency signals with GPS coordinates to nearby VitaLink devices.

## 🎯 Project Features

- **Continuous Heart Rate Monitoring**: Real-time heart rate measurement using pulse sensor
- **GPS Location Tracking**: Position determination for emergency situations
- **Smart Alert System**: Audio and visual warnings for low/high heart rate conditions
- **Wireless Communication**: RF24 module for communication with nearby devices
- **Modular Code Structure**: Component-based code architecture for easy development and maintenance
- **Safety Measures**: Sensor error detection and power management control

## 🔧 Hardware Requirements

### Main Components
- **Arduino Uno/Nano** - Main processor
- **Pulse Sensor** - Heart rate measurement
- **GPS Module (NEO-6M)** - Position tracking
- **RF24 Module** - Wireless communication
- **Buzzer** - Audio alerts
- **LED** - Visual indicators
- **Jumper wires** - Connections

### Pin Connections

```
Arduino Pin  |  Component        |  Description
-------------|-------------------|------------------
A0           |  Pulse Sensor     |  Analog signal input
13           |  LED (Heart)      |  Heartbeat indicator
5            |  LED (PWM)        |  Rhythm indicator
6            |  Buzzer           |  Audio alert
7            |  GPS RX           |  GPS module communication
8            |  GPS TX           |  GPS module communication
9            |  RF24 CE          |  RF module control
10           |  RF24 CSN         |  RF module select
11           |  RF24 MOSI        |  SPI communication
12           |  RF24 MISO        |  SPI communication
13           |  RF24 SCK         |  SPI clock signal
```

## 📁 Project Structure

```
CS/
├── .github/
│   └── copilot-instructions.md    # Development guidelines
├── include/                       # Header files
│   ├── HeartRateMonitor.h        # Heart rate sensor class
│   ├── GPSModule.h               # GPS module class
│   └── EmergencySystem.h         # Emergency system class
├── lib/                          # Library implementations
│   ├── HeartRateMonitor.cpp
│   ├── GPSModule.cpp
│   └── EmergencySystem.cpp
├── src/
│   └── main.cpp                  # Main program file
└── README.md                     # This file
```

## 🚀 Installation and Setup

### 1. Required Libraries

Install the following libraries using Arduino IDE or PlatformIO:

```cpp
// Libraries to install from Arduino IDE Library Manager:
- RF24 (by TMRh20)
- SoftwareSerial (included with Arduino Core)
- SPI (included with Arduino Core)
```

### 2. Code Upload

1. Clone or download this project
2. Open with Arduino IDE or VS Code (PlatformIO)
3. Wire hardware components according to the pin table above
4. Upload `src/main.cpp` to your Arduino

### 3. Initial Setup

1. **Sensor Calibration**: System performs 5-second calibration on first run
2. **GPS Signal**: Wait for GPS module to acquire signal in open area
3. **RF Communication**: Use same channel settings if multiple devices are present

## 📊 Working Principle

### Heart Rate Monitoring
- Pulse sensor reads analog signal from finger
- 500Hz sampling using timer interrupt
- Peak detection algorithm identifies heartbeats
- BPM (Beats Per Minute) value is calculated

### Emergency Detection
- **Low rate**: < 60 BPM
- **High rate**: > 100 BPM  
- **Invalid reading**: Sensor connection error

### Communication Protocol
```cpp
struct EmergencyAlert {
  float latitude;      // GPS latitude
  float longitude;     // GPS longitude  
  int heartRate;       // Heart rate (BPM)
  unsigned long timestamp; // Timestamp
  char deviceId[8];    // Device identifier
  int alertLevel;      // Alert level (1-3)
};
```

## 🔔 Alert Levels

| Level | Heart Rate | Buzzer Interval | LED Interval | Description |
|-------|------------|-----------------|--------------|-------------|
| 1     | 50-60 BPM  | 2 seconds       | 1 second     | Low risk |
| 2     | <50 or >100 BPM | 1 second | 0.5 seconds | Medium risk |
| 3     | <40 or >120 BPM | 0.3 seconds | 0.2 seconds | High risk |

## 🔧 Development and Customization

### Adjusting Threshold Values
```cpp
// In main.cpp file:
const int MIN_HEART_RATE = 60;    // Minimum safe BPM
const int MAX_HEART_RATE = 100;   // Maximum normal BPM
const int THRESHOLD = 530;        // Pulse sensor threshold
```

### Adding New Features
1. **Data Logging**: Save data using SD card module
2. **WiFi Connectivity**: Internet connection with ESP32
3. **Mobile App**: Phone integration via Bluetooth
4. **Additional Sensors**: Temperature, humidity, oxygen level

### Debug Mode
```cpp
// For debug messages in Serial Monitor:
Serial.begin(9600);
```

## 🛠️ Troubleshooting

### Common Issues

1. **Heart rate not reading**
   - Check pulse sensor connections
   - Press finger gently on sensor
   - Wait for calibration period

2. **GPS coordinates not available**
   - Test in open area
   - Ensure GPS module antenna faces sky
   - Initial GPS fix may take 30 seconds - 5 minutes

3. **RF communication not working**
   - Verify devices use same channel settings
   - Keep RF modules within 10 meters distance
   - Check power sources

### Debug Commands
```cpp
// Messages you can see in Serial Monitor:
"Heart Rate: XXX BPM"
"GPS Location: lat, lon"
"Emergency signal sent"
"EMERGENCY ALERT TRIGGERED"
```

## 📈 Performance Optimization

- **Power Consumption**: Extend battery life using sleep modes
- **Signal Quality**: Reduce noise with digital filters
- **Communication Range**: Use more powerful RF modules
- **Speed**: Real-time processing with interrupt-based operations

## 🔒 Security and Privacy

- Personal health data is processed locally
- No data sharing except during emergencies
- Device identity remains anonymous
- Communication is unencrypted, contains no sensitive information

## 📝 License and Contributing

This project is developed for educational purposes. To contribute:

1. Fork the project
2. Create a feature branch
3. Test your changes
4. Submit a pull request

## 🆘 Important Warnings

- **Medical Use**: This device cannot be used for medical diagnosis
- **Emergencies**: Seek professional medical help in real emergency situations
- **Hardware Safety**: Handle electronic components carefully
- **Testing Environment**: Perform initial tests in safe environment

## 📞 Contact and Support

For questions:
- Use GitHub Issues section
- Open Pull Requests for code improvement suggestions
- Contribute to documentation improvements

---

**Stay healthy! 💚**
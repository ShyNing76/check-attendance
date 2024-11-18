# IoT Attendance System with Arduino

## Project Overview
An IoT-based attendance tracking system using Arduino Uno and additional components for check-in functionality.

## Hardware Components
- Arduino Uno
- ESP8266-12E (WiFi module)
- LCD 16x2 Display
- MB-102 Breadboard Power Supply
- 4x4 Matrix Keypad
- Fingerprint Sensor AS608

## Key Features
- Fingerprint-based authentication
- LCD display for user feedback
- Keypad for manual input
- WiFi connectivity for data transmission

## Circuit Connections
- Fingerprint Sensor → Arduino Serial/Digital Pins
- LCD 16x2 → Arduino Digital Pins
- 4x4 Keypad → Arduino Digital Pins
- ESP8266-12E → Arduino Serial/Digital Pins

## Software Requirements
- Arduino IDE
- Libraries:
  - Adafruit Fingerprint Library
  - LiquidCrystal Library
  - ESP8266WiFi Library

## Basic Workflow
1. User places finger on AS608 sensor
2. Fingerprint verification
3. Successful match displays on LCD
4. Data sent via ESP8266 to central system

## Sample Code Structure
```cpp
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>

// Initialization and setup functions
void setup() {
  // Initialize sensors, displays, WiFi
}

void loop() {
  // Continuous fingerprint scanning
  // Authentication process
  // Data transmission
}
```

## Potential Improvements
- Cloud data storage
- Real-time attendance tracking
- Multiple authentication methods

## Troubleshooting
- Check wire connections
- Verify library installations
- Ensure proper power supply

## Contributors
-  Đỗ Duy Long
-  Trần Nguyễn Việt Quang
-  Lê Huy Vũ
-  Huỳnh Gia Bảo

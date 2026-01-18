# SpineLine 📱🩺

**Real-time Posture Monitoring Device**  
ESP32 + MPU6050 accelerometer with web dashboard & Firebase cloud sync

[![Status](https://img.shields.io/badge/Status-LIVE-brightgreen.svg)](https://github.com/yourusername/spineline)
[![ESP32](https://img.shields.io/badge/ESP32-Compatible-blue.svg)](https://www.espressif.com/)
[![Firebase](https://img.shields.io/badge/Firebase-Real--time-orange.svg)](https://firebase.google.com/)

## 🚀 Features

- **Real-time posture detection** using MPU6050 accelerometer
- **Vibration alerts** for poor posture (hunching/leaning)
- **Live web dashboard** at device IP (auto-refreshes every 2s)
- **Firebase cloud sync** - data uploaded every 5 seconds
- **Posture analytics** - straight posture percentage + performance rating
- **Session tracking** - automatic start/stop with reset button
- **Mobile-friendly** web interface

## 📊 Live Dashboard Preview

## 🛠 Hardware Setup

![WhatsApp Image 2026-01-18 at 10 30 03 AM](https://github.com/user-attachments/assets/ea34514a-d988-4f24-8da2-4ac654d1563a)

![WhatsApp Image 2026-01-18 at 10 30 04 AM](https://github.com/user-attachments/assets/008448f4-348e-4184-ae94-9b21c03f23f2)

ESP32 Dev Board

├── MPU6050 Accelerometer (I2C: SDA=23, SCL=22)

├── Vibration Motor (Pin 32)

├── 3.3V Power

└── USB for programming


## ⚙️ Software Requirements
Arduino IDE 2.x

Libraries:

├── Firebase ESP Client

├── ESP32 WiFi

└── Wire (built-in)

**Open Dashboard:** http://192.168.1.100

##📈 Performance Metrics
Straight %: (straight_readings / total_readings) × 100

Ratings:

🟢 EXCELLENT: ≥80%

🟢 GOOD: ≥50%

🔴 TERRIBLE: <50%

## 📱 Mobile Access
Connect phone to same WiFi

Open browser: http://192.168.1.100

Add to home screen for app-like experience

## 🔮 Future Enhancements
 BLE mobile app

 Machine learning posture model

 Battery monitoring

 Multiple user profiles

 Email/SMS alerts

 ## Contributers
 - Pooja S Nair
 - Krishna Jyothy J S
 - Aaron Binoy
 - -Aravind Sanakan

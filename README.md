# 🫀 TTGO T-Display Heart Rate Monitor Firmware

Real-time firmware for TTGO T-Display ESP32 that reads heart rate using MAX30105, displays it on TFT, and updates a backend in real-time.

🚀 Overview

This project is a real-time heart rate monitoring system for the TTGO T-Display ESP32:

Measures heart rate (BPM) and average BPM using MAX30105 sensor
Displays live metrics on built-in TFT display
Sends data to a cloud backend (Firestore or other)
Fully task-based architecture using FreeRTOS for concurrency
Continuously monitors and updates every few seconds
⚙️ Core Features
💓 Heart Rate Monitoring
Sensor: MAX30105
Calculates BPM and average BPM
Uses array-based averaging for smoother readings
Detects when the sensor is not properly covered
🖥️ TFT Display Updates
TTGO T-Display (135x240)
Shows:
Current BPM
Average BPM
Status messages (e.g., “ok”, “No finger”)
🌐 Network Integration
Custom Network class for WiFi and backend updates
Sends real-time readings to Firestore or any compatible backend
Allows remote monitoring of multiple devices
⏱️ Real-Time Firmware
Uses FreeRTOS tasks (xTaskCreatePinnedToCore)
Heart rate updates run independently of main loop
Scheduler ensures non-blocking, real-time behavior
Periodic updates via Ticker
🛠️ Safety & Reliability
Tasks can be paused/resumed (vTaskSuspend, xTaskResume)
Automatic detection of sensor misplacement
Serial logging for debugging

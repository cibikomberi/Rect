# Rect PlatformIO Extension

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Installation](#installation)
- [Integration with Rect Backend](#integration-with-rect-backend)
- [Integration with Rect Frontend](#integration-with-rect-frontend)
- [Contact](#contact)

## Overview
The Rect PlatformIO Extension is designed to streamline the development and deployment of ESP32 and ESP8266 firmware for the Rect IoT Device Management platform. This extension simplifies the process of flashing, configuring, and managing IoT devices via **PlatformIO**.

## Features
- **Seamless Integration**: Works directly with the Rect backend for OTA updates.
- **Device Configuration**: Easily set up device credentials for MQTT communication.
- **Automated Firmware Upload**: Simplifies uploading firmware to ESP32 and ESP8266 devices.
- **Logging & Debugging**: Provides real-time logs from connected devices.

## Technologies Used
- **PlatformIO** – Embedded development framework
- **MQTT** – Protocol for IoT communication
- **C++** – Firmware development for ESP32/ESP8266

## Installation
To install the Rect PlatformIO Extension:
1. Open **VS Code** and navigate to the Extensions Marketplace.
2. Search for **Rect** in PlatformIO Extension's library page.
3. Click **Add to project**.
4. Ensure PlatformIO is installed by running:
   ```sh
   pio --version
   ```

## Integration with Rect Backend
- The extension allows **direct communication** with the Rect backend using MQTT.
- Devices can **register, send data, and receive commands** through the backend.

🔗 **Backend Repository:** [https://github.com/cibikomberi/rect-backend](https://github.com/cibikomberi/rect-backend)

## Integration with Rect Frontend
- Users can view and control their connected devices via the Rect frontend dashboard.
- Live data updates and logs can be accessed through the frontend’s real-time interface.
- OTA updates can be pushed from the frontend using the Rect API.

🔗 **Frontend Repository:** [https://github.com/cibikomberi/rect-frontend](https://github.com/cibikomberi/rect-frontend)

## Contact
For any queries or support, please contact [cibikomberi@gmail.com](mailto:cibikomberi@gmail.com).

---
*Thank you for using the Rect PlatformIO Extension!* 🚀

# Kubota BX Digital Cluster

A drop-in replacement for the Kubota BX tractor’s gauge cluster, powered by an **ESP32-S3** and a **4.0″ TFT LCD**. The stock clusters are known to fail; this project builds a rugged, sunlight-readable, and extensible digital dashboard instead.

## Features
- **Real-time RPM** using the ESP32’s PCNT hardware counter.  
- **Sensor support**:
  - Fuel and coolant temp (analog).
  - Oil pressure, battery, glow plug, lights, and blinkers (digital).  
- **Display**: ST7796S 320×480 TFT LCD (evaluating Sharp Memory LCD for sunlight readability).  
- **Web dashboard** served from SPIFFS with WebSockets + `gauge.min.js` gauges.  
- **Update rate**: ~40 FPS for smooth animations.  
- **Emulation mode** for development without tractor hardware.  
- Future expansion: CAN bus, additional sensors, automation.

## Software Stack
- [PlatformIO](https://platformio.org/) / Arduino framework.  
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) for display rendering.  
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) + AsyncTCP for non-blocking web server.  
- [ArduinoJson](https://arduinojson.org/) for state serialization.  
- Frontend: `index.html` + `gauge.min.js` hosted in SPIFFS.  

## How It Works
1. **TractorInputs** abstracts hardware vs emulator.  
2. State is read (RPM, sensors, indicators).  
3. State is broadcast via JSON over WebSockets.  
4. The onboard TFT and the browser dashboard both update gauges/indicators in real time.  

## Motivation
Kubota OEM clusters are hard to source and fragile. This project delivers a reliable, customizable, and open solution that can grow with new features — and doubles as a fun embedded/web integration exercise.  

---

## Getting Started

1. Clone the repo:  
   ```bash
   git clone https://github.com/youruser/KubotaBX-Cluster.git
   cd KubotaBX-Cluster
   ```

2. Open in VS Code with PlatformIO installed.  

3. Upload firmware:  
   ```bash
   pio run -t upload
   ```

4. Upload web assets (from `data/`):  
   ```bash
   pio run -t uploadfs
   ```

5. Connect to the tractor or run in emulator mode and browse to `http://<esp32-ip>/`.  

---

## License
MIT License. See `LICENSE` for details.

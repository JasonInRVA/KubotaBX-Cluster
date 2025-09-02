# Concept of Operations (CONOPS) - BX-Cluster Digital Dashboard

## 1.0 Introduction

### 1.1 Project Purpose
* To replace the factory-installed analog, fragile instrument cluster on a Kubota BX-series tractor with a modern, reliable digital dashboard. The new system aims to provide a clearer, more accurate, and more comprehensive display of critical engine information, improving operator awareness and long-term reliability.

### 1.2 Scope
* This system will read data from the tractor's existing analog and digital sensors, process it, and display it on a graphical TFT LCD screen. It will also host a local WiFi access point and web server to broadcast the tractor's state for remote viewing on a phone or tablet. The system is a receive-and-display unit only; it will not control any tractor functions.

### 1.3 Document Overview
* This document describes the BX-Cluster system from the operator's perspective, detailing its behavior during startup, normal operation, and in response to specific events and fault conditions.

## 2.0 System Overview

### 2.1 System Description
* The system consists of a custom main controller board utilizing an ESP32-S3-DevKitC-1-N32R16V microcontroller, which includes 16MB of PSRAM for graphics buffering. The primary user interface is a 5-inch TFT LCD display. A custom wiring harness connects the controller to the tractor's electrical system and sensors via a single, locking, environmentally-sealed Deutsch-style connector.

### 2.2 Key Features
* Real-time display of engine RPM, coolant temperature, and fuel level via graphical arc gauges.
* Status indicators for:
  * Left & Right Turn Signals
  * Oil Pressure Warning
  * Glow Plug Activation
  * Battery Charge Warning
  * Headlight Activation
* A splash screen on startup.
* A WiFi-based web server for remote monitoring of all displayed data.

## 3.0 The Operator

### 3.1 Operator Profile
* The operator is a tractor owner who requires clear, immediate, and reliable feedback on the engine's status while performing tasks such as mowing, tilling, or loader work.

### 3.2 Operator Goals
* To monitor engine health to prevent damage from overheating or low oil pressure.
* To accurately track fuel level to avoid running out during a task.
* To have clear, unambiguous visual confirmation of active indicators like turn signals and glow plugs.

## 4.0 Operational Scenarios

This section describes the system in action from the user's point of view.

### 4.1 Scenario 1: System Startup (Key On)
* **Trigger:** Operator turns the tractor key to the "ON" position.
* **Sequence of Events:**
  1. The system receives power, and the screen backlights turn on.
  2. A splash screen appears, displaying "BX Cluster - Initializing..." for approximately 2 seconds.
  3. During this time, the WiFi and web server is initialized.
  4. The display transitions to the main dashboard view.
  5. All gauges (RPM, Fuel, Temp) show a reading of zero. All indicator icons are visible in a dim, dark gray "standby" state.
  6. The oil warning light is illuminated by virtue of insufficent oil pressure hitting the sensor since the engine is not running.

### 4.2 Scenario 2: Normal Operation
* **Trigger:** Engine is running under normal conditions.
* **System Behavior:**
  * The main dashboard is displayed, featuring a large central RPM gauge flanked by smaller temperature and fuel gauges. A row of indicator icons is positioned across the top of the screen.
  * The gauge needles move smoothly to reflect real-time data from the sensors.
  * In the absence of any active signals or warnings, all top-row indicators remain in their dim gray "standby" state.

### 4.3 Scenario 3: User-Initiated Events
* **Trigger:** Operator interacts with a tractor control.
* **Events:**
  * **Left/Right Turn Signal:** When a turn signal is activated, the corresponding green arrow icon on the screen flashes brightly at approximately 1 Hz. It returns to the standby state when the signal is deactivated.
  * **Headlights Activated:** When the headlights are turned on, the blue high-beam style icon illuminates with a steady blue light.
  * **Glow Plugs Activated:** During engine pre-heating, the orange glow plug icon illuminates with a steady orange light.

### 4.4 Scenario 4: System-Initiated Events (Alerts & Warnings)
* **Trigger:** A sensor reports an out-of-spec condition.
* **Events:**
  * **Low Oil Pressure:** The red oil can icon flashes brightly at 2 Hz to indicate a critical warning.
  * **Low Battery Voltage:** The battery indicator icon, normally gray, turns a steady, bright red to indicate a charging system fault.
  * **High Coolant Temperature:** As the temperature gauge needle enters the upper 15% of its range (the red zone), the numerical temperature readout turns bright red.
  * **Low Fuel Level:** As the fuel level reaches the lower 10% of its range (the red zone), the numberical fuel readout turns bright red. If the level reaches 5%, the analog fuel gauge flashes at 1 Hz.

### 4.5 Scenario 5: System Shutdown (Key Off)
* **Trigger:** Operator turns the tractor key to the "OFF" position.
* **Sequence of Events:**
  * Power is cut to the system, and the screen turns off completely.

## 5.0 Assumptions and Limitations

### 5.1 Assumptions
* The tractor provides a reasonably stable 12V power source that is conditioned by the board's internal power supply.
* The existing tractor sensors provide predictable analog voltage levels or digital high/low signals.
* The operator can connect to the device's WiFi network with a standard smartphone or tablet for remote viewing.

### 5.2 Limitations
* The TFT display used in Version 1.0 is not sunlight-readable and may be difficult to see in direct, bright sunlight.
* The system does not log data; it is for real-time display only.
* The remote web interface is view-only and provides no control over the system.
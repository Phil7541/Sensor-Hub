# ESP32 Sensor Hub

ESP32 Sensor Hub is a modular environmental monitoring system built using PlatformIO and C++ for the ESP32-C6.

The project exposes sensor data through a RESTful API, allowing external services, dashboards, or home automation systems to retrieve live environmental readings over the network.

Currently supported sensors:
- SHT31 temperature and humidity sensor

![Platform](https://img.shields.io/badge/platform-ESP32--C6-green)
![Framework](https://img.shields.io/badge/framework-PlatformIO-orange)
![Language](https://img.shields.io/badge/language-C++-blue)

---

## Features

- RESTful API for sensor data access
- Modular sensor architecture
- Wi-Fi enabled ESP32-C6 platform
- JSON formatted responses
- Designed for easy expansion with additional sensors
- Component-based firmware structure
- Modular sensor integration system

## Planned Features

- Additional environmental sensors
- MQTT support
- Web dashboard
- Data logging
- OTA firmware updates
- Authentication/API security

## Hardware

Current hardware configuration:
- ESP32-C6 development board
- SHT31 temperature and humidity sensor

## API Example

Example response:

```json
{
  "temperature": 22.4,
  "humidity": 48.1
}
```

## Tech Stack

- ESP32-C6
- PlatformIO
- C++
- Arduino framework
- REST API
- JSON serialization

## Installation

1. Clone the repository:

```bash
git clone https://github.com/Phil7541/Sensor-Hub.git
```

2. Open the project in Visual Studio Code with the PlatformIO extension installed

3. Configure Wi-Fi credentials

4. Build and upload to the ESP32-C6

## Project Structure

```text
src/
└── main.cpp

components/
├── api/            # REST API endpoints and request handling
├── neopixel/       # Status LED control
├── sensors/        # Sensor management layer
├── sht31/          # SHT31 sensor driver and readings
├── sntp_service/   # Network time synchronisation
└── status/         # System status and diagnostics
```

## Design Goals

The project was designed to:
- provide a lightweight local sensor API
- make adding new sensors straightforward
- separate hardware, networking, and API logic cleanly
- serve as a learning project for embedded networking and modular firmware design

## What I Learned

This project helped improve my understanding of:
- embedded networking
- REST API design on constrained hardware
- sensor integration over I2C
- modular firmware architecture
- asynchronous data handling on microcontrollers

## Future Improvements

- Add support for multiple simultaneous sensors
- Improve error handling and diagnostics
- Add API documentation
- Implement persistent configuration storage
- Add Home Assistant integration

## License

MIT License

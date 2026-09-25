# 🌐 ESP32 BME280 AWS IoT Environmental Monitoring System

<p align="center">

<img src="https://img.shields.io/badge/ESP32-IoT-blue?style=for-the-badge&logo=espressif" alt="ESP32">
<img src="https://img.shields.io/badge/BME280-Sensor-green?style=for-the-badge" alt="BME280">
<img src="https://img.shields.io/badge/MQTT-Protocol-orange?style=for-the-badge&logo=mqtt" alt="MQTT">
<img src="https://img.shields.io/badge/AWS-IoT%20Core-yellow?style=for-the-badge&logo=amazonaws" alt="AWS IoT Core">
<img src="https://img.shields.io/badge/Amazon-DynamoDB-red?style=for-the-badge&logo=amazondynamodb" alt="DynamoDB">

</p>

<p align="center">
  <b>Real-Time Environmental Monitoring Using ESP32, MQTT and AWS Cloud</b>
</p>

---

## 📌 Project Overview

The **ESP32 BME280 AWS IoT Environmental Monitoring System** is an end-to-end IoT solution designed to collect, transmit, store and visualize environmental parameters in real time.

A **BME280 environmental sensor** is interfaced with an **ESP32 microcontroller** using the I²C communication protocol. The ESP32 connects to the Internet through Wi-Fi and publishes sensor readings using **MQTT** to **AWS IoT Core**.

AWS IoT Core receives the data and forwards it through an IoT Rule to **Amazon DynamoDB** for persistent cloud storage. The stored data can then be retrieved through the application backend and displayed on a web-based monitoring dashboard.

### 🔄 Complete Data Pipeline

```text
┌──────────────┐
│    BME280    │
│ Temperature  │
│ Humidity     │
│ Pressure     │
└──────┬───────┘
       │
       │ I²C
       ▼
┌──────────────┐
│    ESP32     │
│ Data Process │
│ Wi-Fi        │
└──────┬───────┘
       │
       │ MQTT
       ▼
┌──────────────┐
│ AWS IoT Core │
│ MQTT Broker  │
└──────┬───────┘
       │
       │ IoT Rule
       ▼
┌──────────────┐
│  DynamoDB    │
│ Cloud Store  │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ API / Backend│
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Web Dashboard│
└──────────────┘
```

---

# 🎯 Objectives

The primary objectives of this project are:

* Collect environmental data using the BME280 sensor.
* Process sensor readings using ESP32.
* Establish Wi-Fi connectivity for cloud communication.
* Implement MQTT-based IoT communication.
* Securely connect the ESP32 to AWS IoT Core.
* Store sensor data in Amazon DynamoDB.
* Provide remote access to historical sensor readings.
* Visualize environmental data through a web dashboard.
* Build a scalable foundation for multi-device IoT monitoring.

---

# ✨ Key Features

### 🌡️ Environmental Monitoring

* Temperature measurement
* Relative humidity measurement
* Atmospheric pressure measurement

### 📡 IoT Connectivity

* ESP32 Wi-Fi connectivity
* MQTT communication
* AWS IoT Core integration
* Topic-based publish/subscribe architecture

### ☁️ Cloud Infrastructure

* AWS IoT Core for device communication
* IoT Rules for data routing
* Amazon DynamoDB for cloud data storage
* API/backend integration for dashboard access

### 📊 Visualization

* Real-time sensor information
* Historical data
* Timestamped measurements
* Graphical representation of sensor parameters
* Device/data status monitoring

### 🔐 Security

* Secure MQTT communication
* AWS IoT device authentication
* Credential separation from source code
* Sensitive files excluded using `.gitignore`

---

# 🧩 System Architecture

```text
                         ┌─────────────────────┐
                         │       BME280        │
                         │                     │
                         │ • Temperature       │
                         │ • Humidity          │
                         │ • Pressure          │
                         └──────────┬──────────┘
                                    │
                                    │ I²C
                                    ▼
                         ┌─────────────────────┐
                         │       ESP32         │
                         │                     │
                         │ • Sensor Interface  │
                         │ • Data Processing   │
                         │ • Wi-Fi            │
                         └──────────┬──────────┘
                                    │
                                    │ MQTT
                                    ▼
                         ┌─────────────────────┐
                         │    AWS IoT Core     │
                         │                     │
                         │ • MQTT Broker       │
                         │ • Device Auth       │
                         │ • IoT Rules         │
                         └──────────┬──────────┘
                                    │
                                    │ IoT Rule
                                    ▼
                         ┌─────────────────────┐
                         │      DynamoDB       │
                         │                     │
                         │ • Sensor Records    │
                         │ • Timestamps        │
                         │ • Device Data       │
                         └──────────┬──────────┘
                                    │
                                    ▼
                         ┌─────────────────────┐
                         │   Backend / API     │
                         └──────────┬──────────┘
                                    │
                                    ▼
                         ┌─────────────────────┐
                         │   Web Dashboard     │
                         │                     │
                         │ • Live Data         │
                         │ • Graphs            │
                         │ • History           │
                         └─────────────────────┘
```

---

# 🔧 Hardware Components

| Component                 | Purpose                                                      |
| ------------------------- | ------------------------------------------------------------ |
| **ESP32**                 | Main microcontroller, data processing and Wi-Fi connectivity |
| **BME280**                | Temperature, humidity and atmospheric pressure sensing       |
| **USB / 5V Power Supply** | System power                                                 |
| **Jumper Wires**          | Sensor and controller connections                            |
| **Wi-Fi Network**         | Internet connectivity                                        |

---

# 🔌 BME280 Interface

The BME280 communicates with the ESP32 using **I²C**.

### Typical Connection

| BME280 | ESP32     |
| ------ | --------- |
| VCC    | 3.3V      |
| GND    | GND       |
| SDA    | ESP32 SDA |
| SCL    | ESP32 SCL |

> Use the GPIO pins configured in the firmware for SDA and SCL.

---

# 💻 Software & Technologies

## Embedded System

* ESP32
* C/C++
* I²C
* Wi-Fi
* MQTT
* BME280 sensor library

## Cloud

* AWS IoT Core
* MQTT
* AWS IoT Rules
* Amazon DynamoDB

## Backend

* API / Serverless backend
* AWS Lambda, if applicable
* REST API, if applicable

## Frontend

* HTML5
* CSS3
* JavaScript
* AWS/API integration

## Development Tools

* Visual Studio Code
* Arduino IDE / ESP-IDF
* Git
* GitHub

---

# 📡 MQTT Communication

MQTT is used as the lightweight messaging protocol between the ESP32 and AWS IoT Core.

### Communication Flow

```text
ESP32
  │
  │ MQTT PUBLISH
  ▼
AWS IoT Core
```

### Example MQTT Topic

```text
sensors/bme280
```

### Example Payload

```json
{
  "device": "ESP32-BME280",
  "temperature": 28.5,
  "humidity": 62.0,
  "pressure": 1008.0
}
```

The exact topic and payload structure may be modified according to the deployed firmware.

---

# ☁️ AWS IoT Core

AWS IoT Core acts as the cloud MQTT communication layer.

The ESP32:

1. Connects to Wi-Fi.
2. Establishes a secure connection with AWS IoT Core.
3. Publishes BME280 readings to an MQTT topic.
4. AWS IoT Core receives the message.
5. An IoT Rule processes/routes the message.
6. The sensor data is stored in DynamoDB.

```text
ESP32
   │
   │ MQTT
   ▼
AWS IoT Core
   │
   │ IoT Rule
   ▼
DynamoDB
```

---

# 🗄️ DynamoDB Data Storage

The sensor readings are stored in Amazon DynamoDB.

Typical data fields include:

| Field       | Description                 |
| ----------- | --------------------------- |
| Device ID   | Unique identifier of ESP32  |
| Timestamp   | Time of measurement         |
| Temperature | Temperature in °C           |
| Humidity    | Relative humidity in %      |
| Pressure    | Atmospheric pressure in hPa |

Example:

```text
Device ID:     ESP32-BME280
Timestamp:     2026-09-25T18:30:00
Temperature:   28.5 °C
Humidity:      62.0 %
Pressure:      1008 hPa
```

---

# 📊 Web Dashboard

The web dashboard provides a user-friendly interface for monitoring the collected environmental data.

### Dashboard Features

* Current temperature
* Current humidity
* Current pressure
* Historical readings
* Sensor graphs
* Timestamp information
* Device/data status

### Dashboard Preview

Add your screenshot here:

```text
![Dashboard](screenshots/dashboard.png)
```

---

# 🔄 End-to-End Data Flow

```text
1. BME280 measures environmental parameters
                ↓
2. ESP32 reads sensor through I²C
                ↓
3. ESP32 processes the readings
                ↓
4. ESP32 connects through Wi-Fi
                ↓
5. Sensor data is published using MQTT
                ↓
6. AWS IoT Core receives the message
                ↓
7. AWS IoT Rule processes the message
                ↓
8. DynamoDB stores the sensor record
                ↓
9. Backend/API retrieves the data
                ↓
10. Web dashboard displays the information
```

---

# 🧪 Testing & Validation

The system can be validated at each stage of the data pipeline.

### Hardware Level

* BME280 detected correctly
* I²C communication verified
* Sensor values displayed on ESP32 serial monitor

### Network Level

* ESP32 successfully connects to Wi-Fi
* Internet connectivity verified

### MQTT Level

* ESP32 successfully connects to AWS IoT Core
* MQTT topic receives sensor messages
* Publish operation verified

### Cloud Level

* AWS IoT Core receives messages
* IoT Rule processes incoming data
* DynamoDB records are created

### Application Level

* API returns stored data
* Dashboard displays current readings
* Historical readings are accessible

---

# 🩺 IoT Pipeline Health Monitoring

An important extension of the project is monitoring the health of the complete data pipeline.

```text
ESP32
  ↓
Wi-Fi
  ↓
AWS IoT Core
  ↓
DynamoDB
  ↓
Dashboard
```

The system can identify situations such as:

* ESP32 stops transmitting
* MQTT connection is lost
* AWS IoT Core stops receiving expected messages
* DynamoDB stops receiving new records
* Dashboard data becomes stale

This helps distinguish between a **sensor problem**, **network problem**, **cloud ingestion problem**, and **application problem**.

---

# 📁 Repository Structure

```text
ESP32-BME280-AWS-IoT-Monitoring/
│
├── README.md
├── .gitignore
├── LICENSE
│
├── firmware/
│   └── esp32-bme280/
│       ├── src/
│       ├── include/
│       └── README.md
│
├── aws/
│   ├── iot-core/
│   │   ├── README.md
│   │   ├── mqtt-topics.md
│   │   └── iot-rule.md
│   │
│   ├── dynamodb/
│   │   └── README.md
│   │
│   └── lambda/
│       └── README.md
│
├── dashboard/
│   ├── index.html
│   ├── app.js
│   ├── style.css
│   └── README.md
│
├── hardware/
│   ├── schematic/
│   ├── pcb/
│   └── images/
│
│
└── screenshots/
    ├── dashboard.png
    ├── aws-iot-core.png
    ├── dynamodb.png
    └── esp32-serial-monitor.png
```

---

# ⚙️ Installation & Setup

## 1. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/ESP32-BME280-AWS-IoT-Monitoring.git
cd ESP32-BME280-AWS-IoT-Monitoring
```

## 2. Configure ESP32

Configure:

```text
Wi-Fi SSID
Wi-Fi Password
AWS IoT Endpoint
MQTT Topic
Device Certificate
Private Key
Root CA Certificate
```

Do not commit private credentials or certificates to GitHub.

## 3. Upload Firmware

Open the firmware project in the selected ESP32 development environment and upload it to the ESP32.

## 4. Configure AWS

Configure the required:

* AWS IoT Thing
* Device certificate
* IoT policy
* MQTT topic
* IoT Rule
* DynamoDB table

## 5. Configure Dashboard

Configure the dashboard with the appropriate backend/API endpoint.

---

# 🔐 Security

Sensitive credentials must **never** be committed to this repository.

The following should remain private:

```text
AWS Access Keys
AWS Secret Keys
Private Certificates
Private Keys
Wi-Fi Passwords
API Keys
.env files
Device Credentials
```

Use configuration templates such as:

```text
config.example.h
```

instead of uploading real credentials.

---

# 📷 Project Images

## Hardware Prototype

Add your hardware image:

```text
![Hardware Prototype](hardware/images/prototype.jpg)
```

## AWS IoT Core

```text
![AWS IoT Core](screenshots/aws-iot-core.png)
```

## DynamoDB

```text
![DynamoDB](screenshots/dynamodb.png)
```

## Dashboard

```text
![Dashboard](screenshots/dashboard.png)
```

---

# 🎥 Project Demonstration

A typical demonstration follows this sequence:

```text
BME280 Sensor
      ↓
ESP32 Serial Monitor
      ↓
AWS IoT Core MQTT Message
      ↓
DynamoDB Record
      ↓
Web Dashboard
```

Add your project demonstration video here:

```text
[▶ Watch Project Demonstration](YOUR_VIDEO_LINK)
```

---

# 📈 Results

The implemented system demonstrates an end-to-end IoT data pipeline capable of:

* Collecting environmental sensor data
* Processing data on an ESP32
* Transmitting data over Wi-Fi
* Publishing data through MQTT
* Receiving data using AWS IoT Core
* Storing sensor readings in DynamoDB
* Retrieving cloud data
* Visualizing environmental information through a web interface

---

# 🚀 Future Scope

The system can be further extended with:

* 📱 Mobile application
* 🔔 Real-time environmental alerts
* 🌐 Multiple ESP32 sensor nodes
* 📍 Multi-location monitoring
* 🔋 Battery-powered operation
* ☀️ Solar-powered sensor nodes
* 🤖 AI-based anomaly detection
* 📊 Advanced cloud analytics
* 📈 Long-term environmental trend analysis
* 🔐 Advanced device management
* 🩺 Automated IoT pipeline health monitoring

---

# 🎓 Applications

Potential applications include:

* Smart homes
* Weather monitoring
* Industrial environments
* Agricultural monitoring
* Server rooms
* Laboratories
* Indoor air/environment monitoring
* Educational IoT systems
* Remote environmental sensing

---

# 🧠 Key Learning Outcomes

Through this project, the following concepts are demonstrated:

* Embedded system development
* ESP32 programming
* Sensor interfacing
* I²C communication
* Wi-Fi networking
* MQTT protocol
* Cloud IoT architecture
* AWS IoT Core
* Cloud database management
* API-based data retrieval
* Web dashboard development
* IoT system debugging
* Secure credential management

---

# 👨‍💻 Author

**Yash Mane**

Electronics & Telecommunication Engineering

**Interests:** Embedded Systems • IoT • Robotics • Hardware Design • Cloud-connected Devices

---

# ⭐ Acknowledgements

This project was developed as an educational and engineering project to explore the integration of embedded systems with cloud-based IoT infrastructure.

---

# 📜 License

This project is intended for educational, research and portfolio purposes.

See the `LICENSE` file for details.

---

<p align="center">

**ESP32 × BME280 × MQTT × AWS IoT Core × DynamoDB**

</p>

<p align="center">
⭐ If you find this project useful, consider giving the repository a star.
</p>

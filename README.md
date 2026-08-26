# **edge-ai-underground-mine-safety**

Real-time underground mine safety monitoring using ESP32, TinyML-based hazard prediction, multi-sensor fusion, and LoRa communication.



## **An Edge AI-Based Real-Time Underground Mine Safety Monitoring System Using TinyML and LoRa Communication**



An embedded underground mine safety monitoring system that combines \*\*multi-parameter environmental sensing, TinyML-based edge intelligence, and LoRa communication\*\* to identify hazardous environmental conditions and provide immediate local and remote alerts.



The system uses an ESP32-based monitoring node to collect environmental parameters, perform on-device TinyML inference, classify the monitored condition as \*\*SAFE, WARNING, or CRITICAL\*\*, and transmit the monitoring information to a LoRa gateway.



\---



### \## Table of Contents



\- \[Overview](#overview)

\- \[Key Features](#key-features)

\- \[System Architecture](#system-architecture)

\- \[System Workflow](#system-workflow)

\- \[Hardware](#hardware)

\- \[Software](#software)

\- \[TinyML-Based Hazard Prediction](#tinyml-based-hazard-prediction)

\- \[Safety Classification](#safety-classification)

\- \[LoRa Communication](#lora-communication)

\- \[Alert Mechanism](#alert-mechanism)

\- \[Repository Structure](#repository-structure)

\- \[Implementation](#implementation)

\- \[Experimental Evaluation](#experimental-evaluation)

\- \[Results](#results)

\- \[Applications](#applications)

\- \[Limitations](#limitations)

\- \[Future Improvements](#future-improvements)

\- \[Getting Started](#getting-started)

\- \[Authors](#authors)



\---



### \# Overview



Underground mines can experience rapidly changing environmental conditions caused by hazardous gases, temperature variation, humidity changes, and pressure fluctuations. Conventional monitoring systems often depend on sensor measurements followed by threshold-based processing or remote analysis.



This project introduces an \*\*Edge AI-based underground safety monitoring system\*\* in which environmental measurements are processed directly at the monitoring node.



The monitoring node uses an ESP32 to acquire:



\- Gas concentration

\- Temperature

\- Humidity

\- Pressure



The collected sensor measurements are processed by a lightweight \*\*TinyML model running on the ESP32\*\*. Instead of sending raw measurements to a remote server before making a decision, the monitoring node performs the initial hazard assessment locally.



The TinyML model classifies the monitored environmental condition into:



\- \*\*SAFE\*\*

\- \*\*WARNING\*\*

\- \*\*CRITICAL\*\*



The predicted class and its confidence are then transmitted together with the sensor measurements through \*\*LoRa\*\* to a remote gateway.



The gateway receives and decodes the packet, displays the monitoring information, and generates appropriate audible alerts based on the predicted safety state.



The system therefore combines:



\*\*Environmental Sensing → Edge AI Prediction → LoRa Communication → Gateway Monitoring → Safety Alerts\*\*



\---

### 

### \# Key Features



\## 1. Multi-Parameter Environmental Monitoring



The monitoring node continuously acquires multiple environmental parameters instead of relying on a single sensor.



The monitored parameters include:



\- Gas concentration

\- Temperature

\- Humidity

\- Pressure



Combining multiple environmental measurements allows the TinyML model to consider the overall environmental condition.



\---



## **TinyML-Based Edge Intelligence**



A lightweight machine learning model is deployed directly on the ESP32.



The model receives the environmental sensor measurements and predicts the current safety state:



```text

Sensor Measurements

&#x20;       │

&#x20;       ▼

&#x20;  TinyML Model

&#x20;       │

&#x20;       ▼

&#x20;┌──────┼─────────┐

&#x20;▼      ▼         ▼

SAFE  WARNING   CRITICAL



### TinyML-Based Hazard Prediction



The main intelligent component of the system is the TinyML prediction mechanism implemented on the monitoring node.



Instead of transmitting sensor values to a cloud service before determining the environmental state, the ESP32 processes the measurements locally.



The input feature set consists of:



Temperature

Humidity

Pressure

Gas Value



These values are passed to the TinyML model.



The model produces the safety classification:



&#x20;            Environmental Inputs

&#x20;                    │

&#x20;                    ▼

&#x20;       ┌─────────────────────────┐

&#x20;       │       TinyML Model      │

&#x20;       └────────────┬────────────┘

&#x20;                    │

&#x20;                    ▼

&#x20;            Class Probabilities

&#x20;                    │

&#x20;                    ▼

&#x20;         Highest Confidence Class

&#x20;                    │

&#x20;         ┌──────────┼──────────┐

&#x20;         ▼          ▼          ▼

&#x20;       SAFE      WARNING    CRITICAL



The selected class is used as the system's current environmental safety state.



### Why TinyML?



Traditional sensor monitoring systems can rely on fixed threshold rules.



For example:



IF gas > threshold

&#x20;   WARNING



The proposed system instead uses multiple environmental measurements as inputs to a machine learning model.



This allows the system to consider the combined sensor state when producing the classification.



TinyML also allows the prediction step to be performed directly on the ESP32.



This provides:



Local decision-making

Reduced dependence on cloud processing

Low-latency inference

Reduced communication dependency for the initial classification

Suitability for resource-constrained embedded devices



The LoRa link is then used to communicate the monitoring result to the gateway.



### Safety Classification



The model produces three safety states.



SAFE



The monitored environmental condition is classified as safe.



Monitoring Node

LED = OFF

Gateway

No audible alarm

WARNING



The system identifies a condition requiring attention.



Monitoring Node

LED = STROBE

Gateway

Short Buzzer Beep

CRITICAL



The system identifies a severe hazardous condition.



Monitoring Node

LED = CONTINUOUS ON

Gateway

CONTINUOUS SIREN



The different alert levels allow the system to communicate the severity of the predicted condition without using the same alarm response for every event.



### LoRa-Based Long-Range Communication



LoRa communication is used to transfer monitoring information from the underground monitoring node to the gateway.



The transmitted packet contains information such as:



Packet number

Node ID

Temperature

Humidity

Pressure

Gas value

Predicted safety state

Prediction confidence

Timestamp



The gateway also records communication parameters such as:



RSSI

SNR



The system uses the TinyML prediction as the common decision source for both local and remote alerts.



&#x20;             TinyML Prediction

&#x20;                    │

&#x20;       ┌────────────┼────────────┐

&#x20;       ▼            ▼            ▼

&#x20;     SAFE         WARNING      CRITICAL

&#x20;       │            │            │

&#x20;       ▼            ▼            ▼

&#x20;    LED OFF      LED STROBE   LED ON

&#x20;                    │            │

&#x20;                    ▼            ▼

&#x20;               Short Beep     Siren



This creates a coordinated alert mechanism between the monitoring node and gateway.



### Confidence-Aware Prediction



In addition to the predicted safety state, the system calculates the prediction confidence.



The confidence value is included in the LoRa monitoring packet and is also available at the gateway.



This provides additional information about how strongly the TinyML model supports its predicted class.





## **System Architecture**

## 

The overall system consists of two main embedded sections:



Monitoring Node

LoRa Gateway



The monitoring node performs sensing and TinyML inference, while the gateway receives and presents the transmitted information and generates remote alerts.



&#x20;                UNDERGROUND MONITORING NODE

&#x20;                ───────────────────────────



&#x20;       ┌───────────────────────────────────┐

&#x20;       │              ESP32                │

&#x20;       │                                   │

&#x20;       │  ┌────────┐  ┌────────┐           │

&#x20;       │  │ MQ-02  │  │ DHT11  │           │

&#x20;       │  │  Gas   │  │Temp/Hum │           │

&#x20;       │  └────┬───┘  └───┬────┘           │

&#x20;       │       │          │                │

&#x20;       │  ┌────┴──────────┴───────┐        │

&#x20;       │  │       BMP180          │        │

&#x20;       │  │       Pressure        │        │

&#x20;       │  └────────────┬──────────┘        │

&#x20;       │               │                   │

&#x20;       │               ▼                   │

&#x20;       │      Sensor Data Processing       │

&#x20;       │               │                   │

&#x20;       │               ▼                   │

&#x20;       │        TinyML Inference           │

&#x20;       │               │                   │

&#x20;       │        ┌──────┴──────┐            │

&#x20;       │        ▼             ▼            │

&#x20;       │   Prediction     Confidence       │

&#x20;       │        │             │             │

&#x20;       │        └──────┬──────┘             │

&#x20;       │               ▼                   │

&#x20;       │     SAFE / WARNING / CRITICAL     │

&#x20;       │               │                   │

&#x20;       │               ▼                   │

&#x20;       │         Local LED Alert           │

&#x20;       │               │                   │

&#x20;       │               ▼                   │

&#x20;       │          LoRa Transmitter         │

&#x20;       └───────────────┬───────────────────┘

&#x20;                       │

&#x20;                       │ LoRa Wireless Link

&#x20;                       ▼

&#x20;                ┌──────────────────┐

&#x20;                │   LoRa Gateway   │

&#x20;                │      ESP32       │

&#x20;                └────────┬─────────┘

&#x20;                         │

&#x20;            ┌────────────┼─────────────┐

&#x20;            ▼            ▼             ▼

&#x20;       Data Display    Buzzer        Siren

&#x20;            │            │             │

&#x20;            ▼            ▼             ▼

&#x20;       Monitoring     WARNING       CRITICAL







## **System Workflow**



The complete operating sequence is:





START

&#x20; │

&#x20; ▼

Power ON Monitoring Node

&#x20; │

&#x20; ▼

Initialize Sensors, LoRa, TinyML Model and LED

&#x20; │

&#x20; ▼

Read Environmental Sensor Data

&#x20; │

&#x20; ├── Temperature

&#x20; ├── Humidity

&#x20; ├── Pressure

&#x20; └── Gas

&#x20; │

&#x20; ▼

Prepare Sensor Values for TinyML

&#x20; │

&#x20; ▼

Execute TinyML Inference on ESP32

&#x20; │

&#x20; ▼

Calculate Prediction Confidence

&#x20; │

&#x20; ▼

Select Predicted Safety State

&#x20; │

&#x20; ├──────────────┬───────────────┐

&#x20; ▼              ▼               ▼

&#x20;SAFE          WARNING        CRITICAL

&#x20; │              │               │

LED OFF      LED STROBE     LED CONTINUOUS

&#x20; │              │               │

&#x20; └──────────────┴───────────────┘

&#x20;                │

&#x20;                ▼

&#x20;      Create LoRa Monitoring Packet

&#x20;                │

&#x20;                ▼

&#x20;         Transmit Using LoRa

&#x20;                │

&#x20;                ▼

&#x20;         LoRa Gateway Receives

&#x20;                │

&#x20;                ▼

&#x20;         Decode Monitoring Data

&#x20;                │

&#x20;                ▼

&#x20;      Display / Log Monitoring Data

&#x20;                │

&#x20;                ▼

&#x20;       Evaluate Safety Classification

&#x20;                │

&#x20;       ┌────────┼──────────┐

&#x20;       ▼        ▼          ▼

&#x20;     SAFE    WARNING    CRITICAL

&#x20;       │        │          │

&#x20;       │      BEEP       SIREN

&#x20;       │     SHORT       CONTINUOUS

&#x20;       │        │          │

&#x20;       └────────┼──────────┘

&#x20;                │

&#x20;                ▼

&#x20;       Store Monitoring Data

&#x20;                │

&#x20;                ▼

&#x20;       Wait for Next Interval

&#x20;                │

&#x20;                ▼

&#x20;              REPEAT



### Hardware



The system is built using low-cost embedded hardware suitable for prototyping and real-time monitoring.



Monitoring Node



The monitoring node includes:



ESP32

MQ-02 gas sensor

DHT11 temperature and humidity sensor

BMP180 pressure sensor

LoRa communication module

LED indicator

Power supply

Gateway



The gateway includes:



ESP32-based controller

LoRa communication module

Buzzer

Siren

Monitoring/serial interface

Sensors

MQ-02 Gas Sensor



The gas sensor provides the gas-related measurement used as one of the inputs to the TinyML prediction mechanism.



The gas measurement is combined with temperature, humidity, and pressure information rather than being used as the only input to the classification process.



DHT11



The DHT11 provides:



Temperature

Humidity



These values form part of the environmental feature set used by the monitoring system.



BMP180



The BMP180 provides atmospheric pressure measurements.



Pressure is included as an additional environmental parameter for the TinyML-based hazard assessment.



### Software



The project uses embedded firmware running on ESP32-based hardware.



Main technologies include:



Embedded C

ESP32

TinyML

Edge AI

LoRa

IoT

GPIO

Sensor interfacing

Real-time embedded programming



Development and firmware testing are performed using the appropriate ESP32 development environment.

## 

## Implementation



#### Monitoring Node



#### The monitoring node follows this sequence:



Initialize ESP32.

Initialize environmental sensors.

Initialize LoRa communication.

Load the TinyML model.

Read environmental measurements.

Prepare the measurements for inference.

Execute TinyML inference.

Determine the predicted safety state.

Calculate prediction confidence.

Update the local LED according to the predicted state.

Create the LoRa monitoring packet.

Transmit the packet to the gateway.

Repeat the monitoring cycle.

Gateway

#### 

#### The gateway follows this sequence:



Initialize ESP32.

Initialize LoRa receiver.

Wait for an incoming packet.

Receive the monitoring packet.

Decode the packet fields.

Extract sensor measurements.

Extract prediction and confidence.

Display or log the received information.

Evaluate the received safety state.

Generate the corresponding warning or critical alert.

Wait for the next packet.

Experimental Evaluation



The prototype was evaluated using the complete sensing, TinyML, LoRa, and alert pipeline.



#### The evaluation considered:



Environmental sensor variation

TinyML safety classification

Prediction confidence

Safety-state distribution

LoRa communication interval

RSSI

SNR

Local LED response

Gateway buzzer response

Gateway siren response



##### Results



The project evaluation demonstrates the operation of the complete embedded monitoring chain.



Environmental Response



The gas sensor measurements showed noticeable variation during the experimental sequence.



The corresponding TinyML classifications changed between SAFE, WARNING, and CRITICAL states as the monitored conditions changed.



This demonstrates the connection between changing environmental inputs and the edge classification mechanism.



The experimental results are included in:



Results



### Applications



The proposed system can be adapted for:



Underground mine environmental monitoring

Worker safety monitoring

Industrial environmental monitoring

Hazardous-area monitoring

Remote environmental sensing

Edge-based safety monitoring

Low-power IoT monitoring systems



The architecture can also serve as a prototype platform for future industrial safety monitoring systems.



### Limitations



The current prototype is an experimental embedded system and has several limitations.



The prototype was evaluated under controlled experimental conditions.

The system is not intended to replace certified mine-safety equipment.

Sensor measurements can be affected by sensor calibration and environmental conditions.

LoRa communication performance depends on deployment conditions and physical surroundings.

The current classification model is intended for the demonstrated prototype rather than certified industrial hazard detection.

Further field testing is required before deployment in operational underground mines.

Future Improvements



### Future development can focus on:



1\. Improved Sensor Calibration



More extensive calibration can improve the reliability of environmental measurements.



2\. Expanded Training Dataset



A larger and more diverse dataset can be used to improve the robustness of the TinyML model.



3\. Additional Environmental Parameters



Additional sensors can be incorporated to monitor further mine-environment conditions.



4\. Improved Wireless Deployment



Multiple monitoring nodes can be deployed to create a distributed underground monitoring network.



5\. Gateway Expansion



The gateway can be extended with a dedicated monitoring dashboard for centralized visualization and long-term data analysis.



6\. Hardware Optimization



The monitoring node can be further optimized for power consumption, size, and rugged deployment.



7\. Field Validation



Extended underground field testing can be performed to evaluate system behavior under realistic environmental and communication conditions.



### Project Highlights

Edge Intelligence



The safety classification is performed directly on the ESP32 using TinyML.



Multi-Sensor Decision Making



Temperature, humidity, pressure, and gas measurements are considered together by the prediction mechanism.



Confidence-Aware Prediction



The system records and transmits the confidence associated with the predicted safety state.



Long-Range Communication



LoRa provides the communication link between the monitoring node and gateway.



Dual-Level Safety Response



The predicted state drives both local and remote safety alerts.



Embedded Implementation



The complete prototype integrates sensing, machine learning inference, communication, and physical alert generation on resource-constrained embedded hardware.



### Project Status



Status: Prototype / Experimental Validation



The current implementation demonstrates:



Multi-parameter environmental sensing

TinyML-based edge classification

SAFE/WARNING/CRITICAL state prediction

Prediction confidence generation

LoRa packet transmission

Gateway reception

Local LED indication

Gateway buzzer alert

Gateway critical siren

Experimental communication evaluation



## Author



##### R G Adithiyaa

##### 

##### Electronics and Communication Engineering

##### 

##### Bengaluru, India



### **License**



No open-source license is currently specified for this repository.



All rights to the original project implementation, documentation, and research work are retained by the project author(s).





This project is an academic and experimental prototype intended for research, learning, and demonstration purposes.



It should not be treated as a certified mine-safety system or used as the sole safety mechanism in an operational underground mine.



Deployment in real mining environments would require appropriate industrial certification, sensor calibration, environmental testing, communication validation, and compliance with applicable mine-safety regulations.


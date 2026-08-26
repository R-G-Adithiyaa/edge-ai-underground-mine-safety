/*
==============================================================
 Project  : TinyML-Based Adaptive Multi-Sensor Hazard Assessment
            with Intelligent LoRa Communication
 Platform : ESP32 DevKit V1 (30 Pin)
 Author   : R G Adithiyaa
 Version  : TinyML Integrated Version
==============================================================
*/

//==============================================================
// Arduino Libraries
//==============================================================

#include <SPI.h>
#include <Wire.h>

#include <LoRa.h>

#include <DHT.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

//==============================================================
// Edge Impulse
//==============================================================

#include <TinyML_underground_safety_for_workers_inferencing.h>

//==============================================================
// LoRa Pins
//==============================================================

#define LORA_SS        5
#define LORA_RST       14
#define LORA_DIO0      26

//==============================================================
// Sensor Pins
//==============================================================

#define DHT_PIN        4
#define DHT_TYPE       DHT11

#define MQ2_PIN        34

//==============================================================
// Outputs
//==============================================================

#define HELMET_LED     27
#define BUZZER_PIN     25

//==============================================================
// Objects
//==============================================================

DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_BMP085_Unified bmp(10085);

//==============================================================
// Sensor Variables
//==============================================================

float temperature = 0.0;

float humidity = 0.0;

float pressure = 0.0;

int gasValue = 0;

//==============================================================
// TinyML Variables
//==============================================================

// Feature vector
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

// Edge Impulse signal
ei::signal_t ei_signal;

// Classification result
ei_impulse_result_t result;

// Prediction
String prediction = "SAFE";

// Confidence
float predictionConfidence = 0.0;

//==============================================================
// Communication Variables
//==============================================================

String helmetID = "H001";

unsigned long packetNumber = 0;

unsigned long lastTransmit = 0;

unsigned long transmitInterval = 5000;

//==============================================================
// Function Prototypes
//==============================================================

void initializeSystem();

void readSensors();

void runTinyML();

void updateLED();

void transmitPacket();

void printConsole();

void printCSV();

void printFeatures();

//==============================================================
// Edge Impulse Callback
//==============================================================

static int raw_feature_get_data(
    size_t offset,
    size_t length,
    float *out_ptr)
{
    memcpy(
        out_ptr,
        features + offset,
        length * sizeof(float));

    return 0;
}
//==============================================================
// Setup
//==============================================================

void setup()
{
    initializeSystem();
}

//==============================================================
// Initialize Complete System
//==============================================================

void initializeSystem()
{
    Serial.begin(115200);

    while (!Serial);

    Serial.println();
    Serial.println("===============================================");
    Serial.println("TinyML Underground Worker Safety Helmet");
    Serial.println("ESP32 Helmet Node");
    Serial.println("===============================================");

    //----------------------------------------------------------
    // LED
    //----------------------------------------------------------

    pinMode(HELMET_LED, OUTPUT);

    digitalWrite(HELMET_LED, LOW);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    //----------------------------------------------------------
    // DHT11
    //----------------------------------------------------------

    dht.begin();

    Serial.println("[OK] DHT11 Initialized");

    //----------------------------------------------------------
    // BMP180
    //----------------------------------------------------------

    if (!bmp.begin())
    {
        Serial.println("[ERROR] BMP180 Initialization Failed");

        while (1);
    }

    Serial.println("[OK] BMP180 Initialized");

    //----------------------------------------------------------
    // LoRa
    //----------------------------------------------------------

    LoRa.setPins(
        LORA_SS,
        LORA_RST,
        LORA_DIO0
    );

    while (!LoRa.begin(433E6))
    {
        Serial.println("Initializing LoRa...");

        delay(1000);
    }

    Serial.println("[OK] LoRa Initialized");

    //----------------------------------------------------------
    // Edge Impulse Information
    //----------------------------------------------------------

    Serial.println();
    Serial.println("Edge Impulse Model Loaded");

    Serial.print("Input Features : ");
    Serial.println(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);

    Serial.print("Output Classes : ");
    Serial.println(EI_CLASSIFIER_LABEL_COUNT);

    //----------------------------------------------------------
    // Transmission Timer
    //----------------------------------------------------------

    lastTransmit = millis();

    Serial.println();
    Serial.println("Helmet Node Ready");
    Serial.println("===============================================");
}
//==============================================================
// Read All Sensors
//==============================================================

void readSensors()
{
    //----------------------------------------------------------
    // Read DHT11
    //----------------------------------------------------------

    temperature = dht.readTemperature();

    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("[ERROR] DHT11 Read Failed");

        return;
    }

    //----------------------------------------------------------
    // Read BMP180
    //----------------------------------------------------------

    sensors_event_t event;

    bmp.getEvent(&event);

    if (event.pressure)
    {
        pressure = event.pressure;
    }
    else
    {
        Serial.println("[ERROR] BMP180 Read Failed");

        return;
    }

    //----------------------------------------------------------
    // Read MQ2
    //----------------------------------------------------------

    gasValue = analogRead(MQ2_PIN);

    //----------------------------------------------------------
    // Create TinyML Feature Vector
    //----------------------------------------------------------

    features[0] = temperature;

    features[1] = humidity;

    features[2] = pressure;

    features[3] = (float)gasValue;
}

//==============================================================
// Print Feature Vector
//==============================================================

void printFeatures()
{
    Serial.println();

    Serial.println("========== TinyML Features ==========");

    Serial.print("Temperature : ");
    Serial.println(features[0]);

    Serial.print("Humidity    : ");
    Serial.println(features[1]);

    Serial.print("Pressure    : ");
    Serial.println(features[2]);

    Serial.print("Gas         : ");
    Serial.println(features[3]);

    Serial.println("=====================================");
}

//==============================================================
// Run TinyML Inference
//==============================================================

void runTinyML()
{
    //----------------------------------------------------------
    // Create Edge Impulse Signal
    //----------------------------------------------------------

    ei_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;

    ei_signal.get_data = &raw_feature_get_data;

    //----------------------------------------------------------
    // Run Inference
    //----------------------------------------------------------

    EI_IMPULSE_ERROR err =
        run_classifier(
            &ei_signal,
            &result,
            false
        );

    if (err != EI_IMPULSE_OK)
    {
        Serial.print("[ERROR] Inference Failed : ");

        Serial.println(err);

        return;
    }

    //----------------------------------------------------------
    // Find Highest Confidence Prediction
    //----------------------------------------------------------

    prediction = "";

    predictionConfidence = 0.0f;

    for (size_t i = 0;
         i < EI_CLASSIFIER_LABEL_COUNT;
         i++)
    {
       /* Serial.print(result.classification[i].label);

        Serial.print(" : ");

        Serial.println(
            result.classification[i].value,
            5
        );
        */

        if(result.classification[i].value >
           predictionConfidence)
        {
            predictionConfidence =
                result.classification[i].value;

            prediction =
                result.classification[i].label;
        }
    }

    //----------------------------------------------------------
    // Print Final Prediction
    //----------------------------------------------------------

    /*Serial.println();

    Serial.print("Prediction : ");

    Serial.println(prediction);

    Serial.print("Confidence : ");

    Serial.println(predictionConfidence,4);

    Serial.println();
    */
}

//==============================================================
// Update Helmet LED Based on AI Prediction
//==============================================================

void updateLED()
{
    static unsigned long previousBlink = 0;
    static bool ledState = false;

    if (prediction == "SAFE")
    {
        digitalWrite(HELMET_LED, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }

    else if (prediction == "WARNING")
    {
        if (millis() - previousBlink >= 500)
        {
            previousBlink = millis();

            ledState = !ledState;

            digitalWrite(HELMET_LED, ledState);
            digitalWrite(BUZZER_PIN, ledState);
        }
    }

    else if (prediction == "CRITICAL")
    {
        digitalWrite(HELMET_LED, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
    }

    else
    {
        digitalWrite(HELMET_LED, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }
}

//==============================================================
// LoRa Packet Transmission
//==============================================================

void transmitPacket()
{
    packetNumber++;

    char packet[150];

    snprintf(
        packet,
        sizeof(packet),
        "%lu,%s,%.2f,%.2f,%.2f,%d,%s,%.3f,%lu",
        packetNumber,
        helmetID.c_str(),
        temperature,
        humidity,
        pressure,
        gasValue,
        prediction.c_str(),
        predictionConfidence,
        millis()
    );

    Serial.println();
    Serial.println("========================================");

    Serial.print("Packet Number : ");

    Serial.println(packetNumber);

    Serial.print("Prediction    : ");

    Serial.println(prediction);

    Serial.print("Confidence    : ");

    Serial.println(predictionConfidence,3);

    Serial.println();

    Serial.print("Packet : ");

    Serial.println(packet);

    LoRa.beginPacket();

    LoRa.print(packet);

    int status = LoRa.endPacket();

    if(status)
    {
        Serial.println("Packet Sent Successfully");
    }
    else
    {
        Serial.println("Packet Transmission Failed");
    }

    Serial.println("========================================");
}

void printCSV()
{
    Serial.print(packetNumber);

    Serial.print(",");

    Serial.print(temperature);

    Serial.print(",");

    Serial.print(humidity);

    Serial.print(",");

    Serial.print(pressure);

    Serial.print(",");

    Serial.print(gasValue);

    Serial.print(",");

    Serial.print(prediction);

    Serial.print(",");

    Serial.println(predictionConfidence,3);
}

//======================================================
// Serial Console
//======================================================

void printConsole()
{
    static unsigned long previousConsole = 0;

    if(millis() - previousConsole < 1000)
        return;

    previousConsole = millis();

    Serial.println();
    Serial.println("=================================================");
    Serial.println("     TINYML HELMET NODE");
    Serial.println("=================================================");

    Serial.print("Helmet ID          : ");
    Serial.println(helmetID);

    Serial.print("Packet Number      : ");
    Serial.println(packetNumber);

    Serial.print("Temperature        : ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity           : ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Pressure           : ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Gas Value          : ");
    Serial.println(gasValue);

    Serial.println();

    Serial.println();
Serial.println("------ CLASS PROBABILITIES ------");

for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++)
{
    Serial.print(result.classification[i].label);
    Serial.print(" : ");
    Serial.println(result.classification[i].value, 4);
}

Serial.println("---------------------------------");

    Serial.println("------------ AI PREDICTION ------------");

    Serial.print("Prediction         : ");
    Serial.println(prediction);

    Serial.print("Confidence         : ");
    Serial.print(predictionConfidence * 100.0, 2);
    Serial.println(" %");

    Serial.println("---------------------------------------");

    Serial.print("Transmit Interval  : ");
    Serial.print(transmitInterval / 1000);
    Serial.println(" sec");

    Serial.print("Current Time       : ");
    Serial.println(millis());

    Serial.print("Last TX Time       : ");
    Serial.println(lastTransmit);

    Serial.println("=================================================");
}

//======================================================
// Main Loop
//======================================================

void loop()
{
    //--------------------------------------------------
    // Read Sensors
    //--------------------------------------------------

    readSensors();

    //--------------------------------------------------
    // Run TinyML Inference
    //--------------------------------------------------

    runTinyML();

    //--------------------------------------------------
    // Update LED & Buzzer
    //--------------------------------------------------

    updateLED();

    //--------------------------------------------------
    // Print Console every second
    //--------------------------------------------------

    printConsole();

    //--------------------------------------------------
    // Transmit every 5 seconds
    //--------------------------------------------------

    if (millis() - lastTransmit >= transmitInterval)
    {
        lastTransmit = millis();

        transmitPacket();

        // Uncomment if CSV logging is required
        // printCSV();
    }

    //--------------------------------------------------
    // Small delay
    //--------------------------------------------------

    delay(100);
}
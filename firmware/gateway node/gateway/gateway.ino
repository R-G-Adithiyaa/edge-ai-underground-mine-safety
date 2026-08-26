#include <SPI.h>
#include <LoRa.h>
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 26
String receivedPacket = "";

int packetNumber;

String helmetID;

float temperature;

float humidity;

float pressure;

int gasValue;

String prediction;

float confidence;

unsigned long timestamp;
void setup()
{
    Serial.begin(115200);

    while(!Serial);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" Underground Gateway");
    Serial.println("=================================");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if(!LoRa.begin(433E6))
    {
        Serial.println("LoRa Failed");

        while(true);
    }

    Serial.println("LoRa Receiver Ready");
}
void loop()
{
    int packetSize = LoRa.parsePacket();

    if(packetSize)
    {
        receivedPacket = "";

        while(LoRa.available())
        {
            receivedPacket += (char)LoRa.read();
        }

        Serial.println();
        Serial.println("Packet Received");

        Serial.println(receivedPacket);

        parsePacket();
    }
}
void parsePacket()
{
    int index = 0;

    String field[9];

    for (int i = 0; i < receivedPacket.length(); i++)
    {
        if (receivedPacket[i] == ',')
        {
            index++;

            if (index >= 9)
                break;
        }
        else
        {
            field[index] += receivedPacket[i];
        }
    }

    packetNumber = field[0].toInt();

    helmetID = field[1];

    temperature = field[2].toFloat();

    humidity = field[3].toFloat();

    pressure = field[4].toFloat();

    gasValue = field[5].toInt();

    prediction = field[6];

    confidence = field[7].toFloat();

    timestamp = field[8].toInt();

    printPacket();
}
void printPacket()
{
    Serial.println();
    Serial.println("==============================================");
    Serial.println("      UNDERGROUND SAFETY GATEWAY");
    Serial.println("==============================================");

    Serial.print("Packet Number    : ");
    Serial.println(packetNumber);

    Serial.print("Helmet ID        : ");
    Serial.println(helmetID);

    Serial.print("Temperature      : ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity         : ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Pressure         : ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Gas Value        : ");
    Serial.println(gasValue);

    Serial.println();

    Serial.println("------------- AI RESULT -------------");

    Serial.print("Prediction       : ");
    Serial.println(prediction);

    Serial.print("Confidence       : ");
    Serial.print(confidence * 100.0, 2);
    Serial.println(" %");

    Serial.println("-------------------------------------");

    Serial.print("Timestamp        : ");
    Serial.println(timestamp);

    Serial.print("RSSI             : ");
    Serial.print(LoRa.packetRssi());
    Serial.println(" dBm");

    Serial.print("SNR              : ");
    Serial.print(LoRa.packetSnr());
    Serial.println(" dB");

    Serial.println("==============================================");
    Serial.println();
}
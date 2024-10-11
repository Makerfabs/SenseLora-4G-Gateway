// 使用 4.6.0  版本的库 RadioLib 在文件夹： C:\Users\maker\Documents\Arduino\libraries\RadioLib 
// 使用 2.0.0  版本的库 SPI 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.15\libraries\SPI 
// 使用 7.0.4  版本的库 ArduinoJson 在文件夹： C:\Users\maker\Documents\Arduino\libraries\ArduinoJson 

#include <Arduino.h>
#include <RadioLib.h>
#include <SPI.h>
#include <ArduinoJson.h>

// 4G
#define BAT_PIN 14
#define IO_RXD2 1
#define IO_TXD2 2
#define IO_GSM_PWRKEY 42
#define IO_GSM_RST 41
#define POWER_3V3_PIN 21

// LoRa
#define LORA_MOSI 11
#define LORA_MISO 13
#define LORA_SCK 12
#define LORA_CS 4

#define LORA_RST 5
#define LORA_DIO0 6
#define LORA_DIO1 7

#define FREQUENCY 868
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 12
#define CODING_RATE 6
#define OUTPUT_POWER 20
#define PREAMBLE_LEN 8
#define GAIN 0

#define THINGSPEAK_APIKEY "YOUR_KEY"

HardwareSerial mySerial2(2);
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI, SPISettings());
int count = 0;

void setup()
{

    USBSerial.begin(115200);
    mySerial2.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI);

    pin_init();
    lora_init();
    at_init();

    while (0)
    {
        String str = "{\"temp\":25.64,\"humi\":47.72}";
        char url[200];
        json2url(str, url);
        USBSerial.println(url);
        delay(3000);
    }
}

long runtime = 0;

void loop()
{
    lora_receive_task();
    // http_request();
}

void pin_init()
{
    USBSerial.println(F("void pin_init()"));

    pinMode(BAT_PIN, INPUT);
    pinMode(IO_GSM_RST, OUTPUT);
    pinMode(IO_GSM_PWRKEY, OUTPUT);
    pinMode(POWER_3V3_PIN, OUTPUT);

    digitalWrite(POWER_3V3_PIN, HIGH);
    digitalWrite(IO_GSM_RST, HIGH);
    delay(1000);
    digitalWrite(IO_GSM_RST, LOW);
    delay(500);
    digitalWrite(IO_GSM_PWRKEY, HIGH);
    delay(1000);
    digitalWrite(IO_GSM_PWRKEY, LOW);
}

void lora_init()
{
    USBSerial.println(F("void lora_init()"));

    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SX127X_SYNC_WORD, OUTPUT_POWER, PREAMBLE_LEN, GAIN);
    if (state == ERR_NONE)
    {
        USBSerial.println(F("success!"));
    }
    else
    {
        USBSerial.print(F("failed, code "));
        USBSerial.println(state);
        while (true)
            ;
    }
}

void at_init()
{
    USBSerial.println(F("void at_init()"));

    sendData("AT", 1000);
    delay(1000);
    sendData("AT+SIMCOMATI", 1000);
    delay(1000);
    sendData("AT+CICCID", 1000);
    sendData("AT+CNUM", 1000);
    sendData("AT+COPS?", 1000);
    sendData("AT+CPSI?", 1000);
    sendData("AT+CSQ", 1000);
}

void http_request()
{
    // Http test
    sendData("AT+HTTPINIT", 1000);
    // sendData("AT+HTTPPARA=\"URL\",\"http://www.baidu.com\"", 1000);
    sendData("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=5WOQ0JZURMWBTRF4&field1=123&field2=456\"", 1000);
    sendData("AT+HTTPACTION=0", 1000);
    sendData("AT+HTTPHEAD", 1000);
    sendData("AT+HTTPREAD=0,500", 1000);
    sendData("AT+HTTPTERM", 1000);
}

String sendData(String command, const int timeout)
{
    String response = "";
    mySerial2.println(command);
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (mySerial2.available())
        {
            char c = mySerial2.read();
            response += c;
        }
    }

    USBSerial.print(response);

    return response;
}

void lora_receive_task()
{
    // test LoRa
    String str;
    int state = radio.receive(str);

    if (state == ERR_NONE)
    {
        // packet was successfully received
        USBSerial.println(F("success!"));

        // print the data of the packet
        USBSerial.print(F("[SX1276] Data:\t\t\t"));
        USBSerial.println(str);

        // print the RSSI (Received Signal Strength Indicator)
        // of the last received packet
        USBSerial.print(F("[SX1276] RSSI:\t\t\t"));
        USBSerial.print(radio.getRSSI());
        USBSerial.println(F(" dBm"));

        // print the SNR (Signal-to-Noise Ratio)
        // of the last received packet
        USBSerial.print(F("[SX1276] SNR:\t\t\t"));
        USBSerial.print(radio.getSNR());
        USBSerial.println(F(" dB"));

        // print frequency error
        // of the last received packet
        USBSerial.print(F("[SX1276] Frequency error:\t"));
        USBSerial.print(radio.getFrequencyError());
        USBSerial.println(F(" Hz"));

        char url[200];
        json2url(str, url);
        sendData("AT+HTTPINIT", 1000);
        sendData(url, 1000);
        sendData("AT+HTTPACTION=0", 1000);
        sendData("AT+HTTPHEAD", 1000);
        sendData("AT+HTTPREAD=0,500", 1000);
        sendData("AT+HTTPTERM", 1000);
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        // timeout occurred while waiting for a packet
        // Serial.println(F("timeout!"));
    }
    else if (state == ERR_CRC_MISMATCH)
    {
        // packet was received, but is malformed
        USBSerial.println(F("CRC error!"));
    }
    else
    {
        // some other error occurred
        USBSerial.print(F("failed, code "));
        USBSerial.println(state);
    }
}

// SenseLora Air Monitor
// {"ID":"AirM01","COUNT":16,"SLEEP":3600,"bat":3.97,"temp":25.64,"humi":47.72,"eco2":400.00,"lux":109.17}
// Senselora soil Monitor
// {"ID":"SoilM01","COUNT":16,"SLEEP":3600,"bat":3.97,"temp":25.64,"humi":47.72,"PH":4.00}
void json2url(String input, char *url)
{
    JsonDocument doc;
    deserializeJson(doc, input);

    float temp = doc["temp"];
    float humi = doc["humi"];
    sprintf(url, "AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=%s&field1=%.1f&field2=%.1f\"",THINGSPEAK_APIKEY, temp, humi);
}
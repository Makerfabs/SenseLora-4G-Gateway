// 使用 4.6.0  版本的库 RadioLib 在文件夹： C:\Users\maker\Documents\Arduino\libraries\RadioLib 
// 使用 2.0.0  版本的库 SPI 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.15\libraries\SPI 


#include <Arduino.h>
#include <RadioLib.h>
#include <SPI.h>

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

#define FREQUENCY 915.0 //868
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 10
#define PREAMBLE_LEN 8
#define GAIN 0

HardwareSerial mySerial2(2);
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI, SPISettings());
int count = 0;

void setup()
{

    Serial.begin(115200);
    mySerial2.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI);

    pin_init();
    lora_init();
    at_init();
    // http_request();

    while (1)
    {
        lora_receive_task();

        // if ((millis() - runtime) > 3000)
        // {
        //     lora_send_task();
        //     runtime = millis();
        // }
    }
}

long runtime = 0;

void loop()
{
    // put your main code here, to run repeatedly:
    while (Serial.available() > 0)
    {
        mySerial2.write(Serial.read());
        yield();
    }
    while (mySerial2.available() > 0)
    {
        Serial.write(mySerial2.read());
        yield();
    }
}

void pin_init()
{
    Serial.println(F("void pin_init()"));

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
    Serial.println(F("void lora_init()"));

    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SX127X_SYNC_WORD, OUTPUT_POWER, PREAMBLE_LEN, GAIN);
    if (state == ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
            ;
    }
}

void at_init()
{
    Serial.println(F("void at_init()"));

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

    Serial.print(response);

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
        Serial.println(F("success!"));

        // print the data of the packet
        Serial.print(F("[SX1276] Data:\t\t\t"));
        Serial.println(str);

        // print the RSSI (Received Signal Strength Indicator)
        // of the last received packet
        Serial.print(F("[SX1276] RSSI:\t\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print the SNR (Signal-to-Noise Ratio)
        // of the last received packet
        Serial.print(F("[SX1276] SNR:\t\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        // print frequency error
        // of the last received packet
        Serial.print(F("[SX1276] Frequency error:\t"));
        Serial.print(radio.getFrequencyError());
        Serial.println(F(" Hz"));
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        // timeout occurred while waiting for a packet
        // Serial.println(F("timeout!"));
    }
    else if (state == ERR_CRC_MISMATCH)
    {
        // packet was received, but is malformed
        Serial.println(F("CRC error!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

void lora_send_task()
{
    Serial.println(F("void lora_send_task()"));
    // String msg = "SenseLora Test";
    // msg = msg + count++;
    // Serial.println(msg);
    // int state = radio.transmit(msg.c_str());

    int state = radio.transmit("Hello 123");
    if (state == ERR_NONE)
    {
        // the packet was successfully transmitted
        Serial.println(F(" success!"));

        // print measured data rate
        Serial.print(F("[SX1278] Datarate:\t"));
        Serial.print(radio.getDataRate());
        Serial.println(F(" bps"));
    }
    else if (state == ERR_PACKET_TOO_LONG)
    {
        // the supplied packet was longer than 256 bytes
        Serial.println(F("too long!"));
    }
    else if (state == ERR_TX_TIMEOUT)
    {
        // timeout occurred while transmitting packet
        Serial.println(F("timeout!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

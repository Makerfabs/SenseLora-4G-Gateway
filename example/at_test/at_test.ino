#include <Arduino.h>
#include <SPI.h>

// 4G
#define BAT_PIN 14
#define IO_RXD2 1
#define IO_TXD2 2
#define IO_GSM_PWRKEY 42
#define IO_GSM_RST 41
#define POWER_3V3_PIN 21

HardwareSerial mySerial2(2);

void setup()
{

    Serial.begin(115200);
    mySerial2.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);

    pin_init();
    at_init();
    http_request();
}

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

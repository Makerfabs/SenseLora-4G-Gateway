#include <Arduino.h>
#include <Adafruit_AHT10.h>
#include <Wire.h>

// 4G
#define BAT_PIN 14
#define IO_RXD2 1
#define IO_TXD2 2
#define IO_GSM_PWRKEY 42
#define IO_GSM_RST 41
#define POWER_3V3_PIN 21

//AHT10
#define SDA 17
#define SCL 18

String Apikey = "N9IBFTBI3J36T779";

HardwareSerial mySerial2(2);
Adafruit_AHT10 aht;

float temperature = 0.0;
float humidity = 0.0;

void setup()
{
    USBSerial.begin(115200);
    mySerial2.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);    
    pin_init();
    aht10_init();
    at_init();
}

void loop()
{    
  read_aht10();
  http_request();
  delay(3000); // Data is uploaded every 3 seconds
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

void aht10_init()
{
    Wire.begin(SDA, SCL);
    if (!aht.begin())
        USBSerial.println("AHT10 not found.");
    else
        read_aht10();
}

void read_aht10()
{
    sensors_event_t humi, temp;
    aht.getEvent(&humi, &temp); // populate temp and humidity objects with fresh data
    temperature = temp.temperature;
    humidity = humi.relative_humidity;
    USBSerial.print("Temperature: ");
    USBSerial.print(temperature);
    USBSerial.println(" degrees C");
    USBSerial.print("Humidity: ");
    USBSerial.print(humidity);
    USBSerial.println("% rH");
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
    sendData("AT+HTTPINIT", 2000);
    sendData("AT+HTTPPARA=\"URL\",\"https://api.thingspeak.com/update?api_key=" + Apikey + "&field1=" + (String)temperature + "&field2=" + (String)humidity +"\"\r\n", 2000);
    sendData("AT+HTTPACTION=0", 2000);
    sendData("AT+HTTPHEAD", 2000);
    sendData("AT+HTTPREAD=0,500", 2000);
    sendData("AT+HTTPTERM", 2000);
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

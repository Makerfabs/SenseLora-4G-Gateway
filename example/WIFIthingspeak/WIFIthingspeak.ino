#include <Arduino.h>
#include <Adafruit_AHT10.h>
#include <Wire.h>
#include "WiFi.h"

const char* ssid     = "9"; // Change this to your WiFi SSID
const char* password = "12345678"; // Change this to your WiFi password

const char* host = "api.thingspeak.com"; // This should not be changed
const int httpPort = 80; // This should not be changed
const String writeApiKey = "N9IBFTBI3J36T779"; // Change this to your Write API key

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

Adafruit_AHT10 aht;

float temperature = 0.0;
float humidity = 0.0;

void setup()
{
    USBSerial.begin(115200);  
    pin_init();
    aht10_init();
    WiFi.begin(ssid, password); //connect WIFI

    while (WiFi.status() != WL_CONNECTED) //wait for connect success
    {
      delay(500);
      USBSerial.print(".");
    }
    USBSerial.println("WiFi connected!");

    USBSerial.println("IP address: ");
    USBSerial.println(WiFi.localIP()); //print IP address
}

void loop()
{    
    WiFiClient client;
    String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "\r\n" + "Connection: close\r\n\r\n";
    read_aht10();
    // WRITE --------------------------------------------------------------------------------------------
    if (!client.connect(host, httpPort)) {
      return;
    }
    client.print("GET /update?api_key=" + writeApiKey + "&field1=" + temperature + "&field2=" + humidity + footer);
    readResponse(&client);
    delay(10000);
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

void readResponse(WiFiClient *client){
  unsigned long timeout = millis();
  while(client->available() == 0){
    if(millis() - timeout > 5000){
      USBSerial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client->available()) {
    String line = client->readStringUntil('\r');
    USBSerial.print(line);
  }

  USBSerial.printf("\nClosing connection\n\n");
}


//For Maduino Lora Radio

// 使用 4.6.0  版本的库 RadioLib 在文件夹： C:\Users\maker\Documents\Arduino\libraries\RadioLib 
// 使用 1.0  版本的库 SPI 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\libraries\SPI 
// 使用 1.0  版本的库 SoftwareSerial 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\libraries\SoftwareSerial 


// include the library
#include <RadioLib.h>
#include <SPI.h>
#include "avr/boot.h"

// create instance of LoRa class using SX1278 module
// this pinout corresponds to RadioShield
// https://github.com/jgromes/RadioShield

#define DIO0 2
#define DIO1 6
// #define DIO2 7
// #define DIO5 8

#define LORA_RST 9
#define LORA_CS 10

#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13


#define FREQUENCY 868.0
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 12
#define CODING_RATE 6
#define OUTPUT_POWER 20
#define PREAMBLE_LEN 8
#define GAIN 0

SX1276 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1);
// SX1278 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1, SPI, SPISettings());

void setup()
{
  Serial.begin(115200);

  // initialize SX1278 with default settings
  Serial.print(F("Initializing ... "));

  // int state = radio.begin();
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
  Serial.print(F("Waiting for incoming transmission ... "));
}

void loop()
{
  Serial.println(F("void lora_send_task()"));

  int state = radio.transmit("Makerfabs Lora Test 868");
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

  delay(3000);
}

void loop2()
{
  // Serial.print(F("Waiting for incoming transmission ... "));

  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
  // NOTE: for spreading factor 6, the packet length
  //       must be known in advance, and provided to
  //       receive() method!
  String str;
  int state = radio.receive(str);

  // you can also receive data as byte array
  /*
    size_t len = 8;
    byte byteArr[len];
    int state = radio.receive(byteArr, len);
  */

  if (state == ERR_NONE)
  {
    // packet was successfully received
    Serial.println(F("success!"));

    // print data of the packet
    Serial.print(F("Data:\t\t\t"));
    Serial.println(str);

    // print RSSI (Received Signal Strength Indicator)
    // of the last received packet
    Serial.print(F("RSSI:\t\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    // print SNR (Signal-to-Noise Ratio)
    // of the last received packet
    Serial.print(F("SNR:\t\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));

    // print frequency error
    // of the last received packet
    Serial.print(F("Frequency error:\t"));
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
}

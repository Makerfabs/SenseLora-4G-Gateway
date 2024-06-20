# SenseLoRa 4G Gateway

```c++
/*
Version:		V1.0
Author:			Vincent
Create Date:	2024/6/19
Note:

*/
```
# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://wiki.makerfabs.com/)

# How To Use

## Arduino Complier Option

**If you have any questions，such as how to install the development board, how to download the code, how to install the library. Please refer to :[Makerfabs_FAQ](https://github.com/Makerfabs/Makerfabs_FAQ)**

- Install board : ESP32 Version 2.0.11
- Select "ESP32-S3"
- Select Erase All Flash
- Upload codes

Arduino libraries:

- RadioLib 4.6.0

## Example

### fw_test

The factory test program detects whether the AT module can search for the operator and receives Lora data in a loop.

### at_test

Used to debug the AT module and perform transparent serial port transmission.


### lora2thingspeak

The JSON data from the SenseLora Air Monitor is parsed and forwarded to ThingSpeak.
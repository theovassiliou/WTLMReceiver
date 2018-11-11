# WTLMReceiver

## Scope
The Water Tank Level Meter Receiver an Arduino/ESP based receiver for receiving sensor readings from a TS-FT002 (aka XC-0331) sensor.

## Supported sensor
As a sensor currently the TS-FT002 outdoor sensor is support. Sensor readings
are transmitted via 433 MHz and a proprietary protocol. More details on the
sensor can be found here.

## Supported hardware
* Sensor: TS-FT002 as found e.g. [Amazon](https://amzn.to/2PN7DMf)
* Arduino Nano
* TTGO LORA32 868/915 mhz ESP32 LoRa OLED as found e.g. [Aliexpress](https://de.aliexpress.com/item/TTGO-LORA32-868-915-mhz-ESP32-LoRa-OLED-0-96-zoll-Blau-Display-Bluetooth-WIFI-ESP32/32840238513.html)

## Installation 
* Download the latest release from https://github.com/theovassiliou/WTLMReceiver/releases/latest
* Unzip and copy the fold to your Arduino libraries folder
* Check out and compile the provided example `WaterTankLevelSensorGeneric` 

## Todo 
[] Check compatibility with XC-0331: Couldn't check whether they are really compatible. 

# WTLMReceiver

## Scope
The Water Tank Level Meter Receiver an Arduino/ESP based receiver for receiving sensor readings from a TS-FT002 (aka XC-0331) sensor.

## NOTE AND WARNING
I have realized that the protocol that is used in between the sensor and the display of the TS-FT002 can be quite different between different charges. I have currently five sensors in place, with apparantly three different protocols, of which I have been able to decode only one. So please be aware of this, in case you will not receive anything with your sensor. 

Right now (Nov '19) I would highly discourage buying the sensor if you insist in using this libary. If you have a spare one, and would just like to pimp it, of course you can give it a try. But don't buy it, to interface it with an Arduino, and then blame me ;-) Feel free to work support me on the decoding of the other variants. 

## Supported sensor
As a sensor currently the TS-FT002 outdoor sensor is support. Sensor readings
are transmitted via 433 MHz and a proprietary protocol. More details on the
sensor can be found here. AND PLEASE READ THE NOTE ABOVE.

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

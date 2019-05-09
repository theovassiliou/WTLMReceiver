#include <WTLMReceiver.h>

WTLMReceiver myReceiver = WTLMReceiver();

void setup() {
        Serial.begin(9600);
#if defined(ARDUINO) && !defined(ESP32)
        myReceiver.enableReceive(2);
        Serial.println("Let's start ARDUINO");
#elif defined(ESP32)
        myReceiver.enableReceive(21);
        Serial.println("Let's start TTGO");
#endif
}

void logData(WTLMReceiver myReceiver) {

      char logStr[128];

      if(myReceiver.checkCRC() == true) {

                sprintf(logStr, "\nCRC: OK (0x%x)",  myReceiver.getReceivedMessage()[8]);
                Serial.print(logStr);
        } else {
                sprintf(logStr, "\nCRC: NOT OK (0x%x vs 0x%x)", myReceiver.calcCRC(myReceiver.getReceivedMessage(),8), myReceiver.getReceivedMessage()[8]);
                Serial.print(logStr);
        }

        Serial.println("");
        unsigned int temp = myReceiver.getTemp();
        sprintf(logStr, "Temperatur: %d.%d \n", temp / 10, temp % 10);
        Serial.print(logStr);

        unsigned int depth = myReceiver.getDepth();
        sprintf(logStr, "Depth: %d cm\n", depth);
        Serial.print(logStr);

        unsigned char deviceId = myReceiver.getDevice();
        sprintf(logStr, "Device: 0x%x with ", deviceId);
        Serial.print(logStr);

        unsigned char serialId = myReceiver.getSerial();
        sprintf(logStr, "Serial: 0x%x\n", serialId);
        Serial.print(logStr);

        unsigned char lowBattery = myReceiver.getLowBattery();
        if (lowBattery == 0) {
                sprintf(logStr, "LowBattery: NO (0x%x)\n", lowBattery);
        } else {
                sprintf(logStr, "LowBattery: YES (0x%x)\n", lowBattery);
        }
        Serial.print(logStr);

        unsigned char transmitIntervall = myReceiver.getTransmitInterval();
        sprintf(logStr, "Transmit Interval: %d sec.", transmitIntervall);
        Serial.print(logStr);

        return;
}

void loop() {
        //        Serial.print(".");
        delay(900);
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        if (myReceiver.available()) {
                // if myReceiver indicates that a message is available
                // you can retrieve the complete message, or individual fields.
                unsigned char *message = myReceiver.getReceivedMessage();
                logData(myReceiver);
                myReceiver.resetAvailable(); // reset the availabilty and wait
                                             // for another message
        }
}

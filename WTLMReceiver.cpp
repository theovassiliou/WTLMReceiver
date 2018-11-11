/*
   WTLMReceiver - Arduino libary for capturing and decoding sensor readings
   from a TS-FT002 ultrasonic water level sensor.

   Copyright (c) 2018 Theofanis Vassiliou-Gioles.  All right reserved.

   Project home: https://github.com/theovassiliou/WTLMReceiver

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */
#include "WTLMReceiver.h"

#ifdef ESP8266
// interrupt handler and related code must be in RAM on ESP8266,
#define RECEIVE_ATTR ICACHE_RAM_ATTR
#else
#define RECEIVE_ATTR
#endif

unsigned int
WTLMReceiver::sTimings[WTLMRECEIVER_MAX_CHANGES];     // array containing the
                                                      // timings
unsigned long WTLMReceiver::sLastTime = 0;            // last time of the capture
bool WTLMReceiver::sRunning  = true;
unsigned int WTLMReceiver::sScanningTiming = 0;
unsigned int WTLMReceiver::sPos      = 0;
unsigned int WTLMReceiver::sPhase    = SCANNING_PHASE;

unsigned char rawMessage[9]; // the bytes from the bits in the captured order
unsigned char message[9];    // the bytes from the nibble reversed order
bool messageAvailable =
        false; // flag to indicate that a message has been received completly

int receiverPin, interruptPin;

WTLMReceiver::WTLMReceiver() {
        initMemory();
        this->receiverPin   = -1;
        this->interruptPin  = -1;
}

/**
 * Enable receiving data
 */
void WTLMReceiver::enableReceive(int receiverPin) {
        this->receiverPin = receiverPin;
        this->enableReceive();
}

void WTLMReceiver::enableReceive() {
        if (this->receiverPin != -1) {
                WTLMReceiver::sPhase = SCANNING_PHASE;
#ifdef ARDUINO
                this->interruptPin = digitalPinToInterrupt(this->receiverPin);
#else
                this->interruptPin = this->receiverPin;
#endif
                initMemory();
                WTLMReceiver::sRunning = true;
                attachInterrupt(this->interruptPin, handleInterrupt, CHANGE);
        }
}

/**
 * Disable receiving data
 */
void WTLMReceiver::disableReceive() {
        detachInterrupt(this->interruptPin);
        this->interruptPin = -1;
}

bool WTLMReceiver::available() {
        return (!WTLMReceiver::sRunning && this->checkCRC());
}

void WTLMReceiver::resetAvailable() {
        initMemory();
        this->messageAvailable = false;
        WTLMReceiver::sPhase = SCANNING_PHASE;
        WTLMReceiver::sRunning = true;
}


unsigned char *WTLMReceiver::getReceivedMessage() {
        if (!this->messageAvailable) {
                // first we have to translate the timings into a raw message
                timing2BinData(WTLMReceiver::sTimings, SIGNAL_SIZE * 2, bits);
                bitstr2Byte(sizeof(bits), bits, rawMessage);
                reverseNibbles(bits, sizeof(bits), rBits);
                bitstr2Byte(sizeof(rBits), rBits, message);
        }
        return this->message;
}



/**
   calcCRC computes a crc-8 for message for len bytes by XORing the len bytes.

   @param *message         the message to be processed
   @param len              number of bytes taken
   @return                 the calculated CRC
 */
unsigned char WTLMReceiver::calcCRC(unsigned char *message, int len) {
        unsigned char result = 0;
        for (int i = 0; i < len; i++) {
                result ^= message[i];
        }
        return result;
}

/**
   checkCRC validates the received CRC against the message.

   @return                 true if the CRC is valid
 */
bool WTLMReceiver::checkCRC() {
        unsigned char mycrc = this->calcCRC(this->getReceivedMessage(), 8);
        if ( mycrc == this->getReceivedMessage()[8]) {
                return true;
        } else {
                return false;
        }
}

/**
   getTemp extracts the tempratur from the received message.
   Temp: (13-TEMP_H) (12-TEMP_M) (10-TEMP_L)

   @param *message        the received message
   @return                tge measured temperatur in celsuis*10. -40 if invalid
 */
signed int WTLMReceiver::getTemp() {
        unsigned char *message = getReceivedMessage();
        unsigned int result = 0;
        result = (result | getNibble(message, 13)) << 4;
        result = (result | getNibble(message, 12)) << 4;
        result = (result | getNibble(message, 10));
        if (result >= 400 && result <= 1000 ) return result - 400;
        return -40;
}

/**
   getDepth extracts the received depth.
   Depth: (07-DEPTH_H) (06-DEPTH_M) (08-DEPTH_L)

   @param *message        the received message
   @return                the measured depth in cm
 */
unsigned int WTLMReceiver::getDepth() {
        unsigned char *message = getReceivedMessage();
        unsigned int result = 0;
        result = (result | getNibble(message, 7)) << 4;
        result = (result | getNibble(message, 6)) << 4;
        result = (result | getNibble(message, 8));
        return result;
}

/**
   getDevice extracts the device id. Should be 0x11.
   Temp: (04-DEVICE_H) (05-DEPTH_L)

   @param *message        the received message
   @return                the deviceId
 */
unsigned char WTLMReceiver::getDevice() {
        unsigned char *message = getReceivedMessage();
        // Captured:
        // H: 4
        // M: 5
        unsigned char result = 0;
        result = (result | getNibble(message, 4)) << 4;
        result = (result | getNibble(message, 5));
        return result;
}

unsigned char WTLMReceiver::getSerial() {
        unsigned char *message = getReceivedMessage();
        // Captured:
        // H: 3
        // M: 2
        unsigned char result = 0;
        result = (result | getNibble(message, 3)) << 4;
        result = (result | getNibble(message, 2));
        return result;
}

unsigned char WTLMReceiver::getLowBattery() {
        unsigned char *message = getReceivedMessage();
        // Captured:
        // L: 9
        unsigned char result = 0;
        result = (result | getNibble(message, 9));
        return result;
}

unsigned char WTLMReceiver::getTransmitInterval() {
        unsigned char *message = getReceivedMessage();
        // Captured:
        // L: 11
        unsigned char result = 0;
        unsigned char tInterval = 255;
        result = (result | getNibble(message, 11));
        if ((result & 0x07) == 1) {
                tInterval = 5;
        } else if ((result & 0x08) == 1 ) {
                tInterval = 30;
        }
        else if (result == 0) {
                tInterval = 180;
        }
        return tInterval;
}

/**
   timing2BinData converts the timing array, into it bit representation.
    480us
 +----+    +
 |    |    |
 |    |    |  is a 0
 +    +----+

    480us
 +----+        +
 |    |        |
 |    |        |  is a 1
 +    +--------+
           960 us

    @param *timings     the array that contains the timings. A timing consists
   of times being in high or low state
    @param timingsLen   the length of the timing array
    @param *bits        the bitstring where the bits (0|1) will be stored, as
   charts
 */
void WTLMReceiver::timing2BinData(unsigned int *timings, int timingsLen,
                                  char *bits) {
        // print bin signal
        // short, short == 0
        // short, long == 1
        for (unsigned int i = 0; i < timingsLen; i = i + 2) {
                bits[i / 2] = time2bin(timings[i], timings[i + 1]);
        }
}

bool WTLMReceiver::isShort(unsigned int t) {
        if ((diff(t, PERIOD) <= 2 * DELTA))
                return true;
        return false;
}
bool WTLMReceiver::isLong(unsigned int t) {
        if ((diff(t, 2 * PERIOD) <= 4 * DELTA))
                return true;
        return false;
}

char WTLMReceiver::time2bin(unsigned int k, unsigned int l) {
        if (isShort(k) && isShort(l)) {
                return '0';
        } else if (isShort(k) && isLong(l)) {
                return '1';
        }
        return '.';
}

/*
   This function converts bits in a bitstring in an array of bytes
   where each byte consists of 8 bits of the bitstring

   int   bitlen    number of chars in bits
   char  *bits     an array of char, with each char a '0' or '1'
   byte  *result   stores in an array of bytes the converted bitstring
 */
void WTLMReceiver::bitstr2Byte(size_t bitlen, char *bits,
                               unsigned char *result) {
        for (int i = 0; i < bitlen; i++) {
                result[(i / 8)] *= 2; // double the result so far
                if (bits[i] == '1')
                        result[(i / 8)]++; // add 1 if needed
        }
}

void WTLMReceiver::initMemory() {
        memset(WTLMReceiver::sTimings, 0, sizeof(WTLMReceiver::sTimings));
        memset(bits, 0, sizeof(bits));
        memset(rawMessage, 0, sizeof(rawMessage));
        memset(message, 0, sizeof(message));
        memset(rBits, 0, sizeof(rBits));

        WTLMReceiver::sPos = 0;
        WTLMReceiver::sScanningTiming = 0;
}

void WTLMReceiver::reverseNibbles(char *bits, int bitLen, char *rBits) {
        int rBitPos = 0;
        for (int i = 0; i < bitLen / 4; i++) {
                rBits[rBitPos++] = bits[(i * 4) + 3];
                rBits[rBitPos++] = bits[(i * 4) + 2];
                rBits[rBitPos++] = bits[(i * 4) + 1];
                rBits[rBitPos++] = bits[(i * 4) + 0];
        }
}

/**
   getNibble returns the nibblePos nibble, right aligned, within the message

   @param *message        the message that contains the nibble
   @param nibblePos       the position of the nibble, starting with 0. nibblePos has to be <=sizeof(message)
   @return                the retrieved nibble, right aligned
 */
unsigned char WTLMReceiver::getNibble(unsigned char *message, unsigned int nibblePos) {
        if ((nibblePos % 2) == 0) {
                // even == high bytes
                return (message[nibblePos / 2] >> 4) & 0xf0;
        } else {
                //odd == low bytes
                return message[nibblePos / 2] & 0x0f;
        }
}

/* helper function for the receiveProtocol method */
inline unsigned int WTLMReceiver::diff(int A, int B) {
        return abs(A - B);
}

bool RECEIVE_ATTR WTLMReceiver::isSignal(int timing) {
        if ((diff(timing, PERIOD) <= DELTA) ||
            (diff(timing, 2 * PERIOD) <= 2 * DELTA))
                return true;
        return false;
}

void RECEIVE_ATTR WTLMReceiver::handleInterrupt() {
        if (!WTLMReceiver::sRunning)
                return;
        const long time = micros();
        const int diff = time - WTLMReceiver::sLastTime;
        WTLMReceiver::sLastTime = time;
        bool isASignal = isSignal(diff);

        if ((WTLMReceiver::sPhase == SCANNING_PHASE)) {
                if (WTLMReceiver::diff(diff, MIN_SCANNING_GAP) <= 3 * DELTA) {
                        WTLMReceiver::sScanningTiming = diff;
                        WTLMReceiver::sPhase = SIGNAL_PHASE;
                        WTLMReceiver::sPos = 0;
                }
        } else if ((WTLMReceiver::sPhase == SIGNAL_PHASE)) {
                WTLMReceiver::sTimings[WTLMReceiver::sPos] = diff;
                WTLMReceiver::sPos++;
                if (WTLMReceiver::sPos > WTLMRECEIVER_MAX_CHANGES - 1) {
                        WTLMReceiver::sRunning = false;
                }
        }
}

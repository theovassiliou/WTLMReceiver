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
#ifndef _WTLMReceiver_h
#define _WTLMReceiver_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#endif

#include <stdint.h>
#include <stddef.h>

#define SIGNAL_SIZE 72
#define WTLMRECEIVER_MAX_CHANGES 144
#define PERIOD 480
#define DELTA 32
#define MIN_SCANNING_GAP 21600
#define SCANNING_PHASE 1
#define SIGNAL_PHASE 2

class WTLMReceiver {

public:
WTLMReceiver();

void enableReceive(int interrupt);
void enableReceive();
void disableReceive();
bool available();
void resetAvailable();
unsigned char *getReceivedMessage();

bool checkCRC();
signed int getTemp();
unsigned int getDepth();
unsigned char getDevice();
unsigned char getSerial();
unsigned char getLowBattery();
unsigned char getTransmitInterval();

unsigned char calcCRC(unsigned char *message, int len);

private:
unsigned char rawMessage[9];           // the bytes from the bits in the captured order
unsigned char message[9];              // the bytes from the nibble reversed order
char bits[SIGNAL_SIZE];          // storing the bits in the captured order
char rBits[SIGNAL_SIZE];         // storing the bits in nibble reversed order

int receiverPin, interruptPin;
bool messageAvailable;

void timing2BinData(unsigned int *timings, int timingsLen, char *bits);
char time2bin(unsigned int k, unsigned int l);
bool isShort(unsigned int t);
bool isLong(unsigned int t);
void bitstr2Byte(size_t bitlen, char *bits, unsigned char *result);
void reverseNibbles(char* bits, int bitLen, char* rBits);
void initMemory();
unsigned char getNibble(unsigned char *message, unsigned int nibblePos);

// -- Some variables needed in the interrupt
static unsigned int sTimings[];
static unsigned long sLastTime;           // last time of the capture
static bool sRunning;
static unsigned int sScanningTiming;
static unsigned int sPhase;
static unsigned int sPos;

static void handleInterrupt();
static bool isSignal(int timing);
static unsigned int diff(int, int );

};

#endif

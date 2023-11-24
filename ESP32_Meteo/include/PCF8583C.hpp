#ifndef __PCF8583__
#define __PCF8583__

#include <Arduino.h>
#include <Wire.h>
#include <PCF8583.h>

class PCF8583C{
private:
    PCF8583* rtc;
    int address;
public:
    PCF8583C(int sda, int scl, int address);
    void reset();
    int getCount();
};

#endif
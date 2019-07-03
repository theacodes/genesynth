#ifndef _ARDUINO_H
#define _ARDUINO_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include "USBMidi.h"

enum PinMode {
    OUTPUT,
    INPUT,
    INPUT_PULLUP
};

enum PinState {
    LOW,
    HIGH
};

typedef uint8_t byte;

unsigned long micros();
unsigned long millis();
inline void delay(unsigned int) {};
inline void delayMicroseconds(unsigned int) {};
inline void pinMode(int, int) {};
inline void analogWriteFrequency(int, int) {};
inline void analogWrite(int, int) {};
inline int digitalReadFast(int) { return 0; };
void digitalWriteFast(int pin, bool value);

inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
    long divisor = (in_max - in_min);
    if(divisor == 0){
        return -1;
    }
    return (x - in_min) * (out_max - out_min) / divisor + out_min;
}

class SerialStub {
public:
    SerialStub() {};

    void begin(int) {};
    
    inline int printf(const char* format, ...) {
        va_list argptr;
        va_start(argptr, format);
        auto result = ::vprintf(format, argptr);
        va_end(argptr);
        return result;
    };

    inline int println(const char* str) {
        return ::printf("%s\n", str);
    };

    bool dtr() { return true; };
};

extern SerialStub Serial;

#endif

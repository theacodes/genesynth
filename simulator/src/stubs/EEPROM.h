#ifndef _EEPROM_H
#define _EEPROM_H

#include <stdint.h>

class _EEPROM {
public:
    _EEPROM() {};

    char read(unsigned int addr) { return 0; };
    void write(unsigned int addr, uint8_t val) {};
    void update(unsigned int addr, uint8_t val) {};
};

extern _EEPROM EEPROM;

#endif

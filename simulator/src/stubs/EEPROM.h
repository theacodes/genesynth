#ifndef _EEPROM_H
#define _EEPROM_H

#include <stdint.h>

#define EEPROM_SIZE 1024
#define EEPROM_FILENAME "EEPROM.bin"

class EEPROMStub {
public:
  EEPROMStub();

  char read(unsigned int addr);
  void write(unsigned int addr, uint8_t val);
  void update(unsigned int addr, uint8_t val);

private:
  uint8_t data[EEPROM_SIZE];
};

extern EEPROMStub EEPROM;

#endif

#include "EEPROM.h"
#include <cstdio>
#include <cstring>

EEPROMStub EEPROM;

EEPROMStub::EEPROMStub() {
  FILE *fp;
  fp = fopen(EEPROM_FILENAME, "r");
  if (fp != NULL) {
    int i = 0;
    do {
      data[i] = fgetc(fp);

      if (feof(fp)) {
        break;
      }

      i++;
    } while (i < EEPROM_SIZE);
    fclose(fp);
  } else {
    memset(data, 0, EEPROM_SIZE);
  }
}

char EEPROMStub::read(unsigned int addr) { return data[addr]; }

void EEPROMStub::write(unsigned int addr, uint8_t val) {
  data[addr] = val;
  FILE *fp;
  fp = fopen(EEPROM_FILENAME, "w");
  if (fp != NULL) {
    for (int i = 0; i < EEPROM_SIZE; i++) {
      fputc(data[i], fp);
    }
    fclose(fp);
  }
}

void EEPROMStub::update(unsigned int addr, uint8_t val) {
  if (data[addr] != val) {
    write(addr, val);
  }
}

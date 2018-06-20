#include "ym2612.h"

void ym_test() {
  /* YM2612 Test code */
  ym_set_reg(0x22, 0x00); // LFO off
  delay(1);
  ym_set_reg(0x27, 0x00); // Note off (channel 0)
  delay(1);
  ym_set_reg(0x28, 0x01); // Note off (channel 1)
  delay(1);
  ym_set_reg(0x28, 0x02); // Note off (channel 2)
  delay(1);
  ym_set_reg(0x28, 0x04); // Note off (channel 3)
  delay(1);
  ym_set_reg(0x28, 0x05); // Note off (channel 4)
  delay(1);
  ym_set_reg(0x28, 0x06); // Note off (channel 5)
  delay(1);
  ym_set_reg(0x2B, 0x00); // DAC off
  delay(1);
  ym_set_reg(0x30, 0x71); //
  delay(1);
  ym_set_reg(0x34, 0x0D); //
  delay(1);
  ym_set_reg(0x38, 0x33); //
  delay(1);
  ym_set_reg(0x3C, 0x01); // DT1/MUL
  delay(1);
  ym_set_reg(0x40, 0x23); //
  delay(1);
  ym_set_reg(0x44, 0x2D); //
  delay(1);
  ym_set_reg(0x48, 0x26); //
  delay(1);
  ym_set_reg(0x4C, 0x00); // Total level
  delay(1);
  ym_set_reg(0x50, 0x5F); //
  delay(1);
  ym_set_reg(0x54, 0x99); //
  delay(1);
  ym_set_reg(0x58, 0x5F); //
  delay(1);
  ym_set_reg(0x5C, 0x94); // RS/AR
  delay(1);
  ym_set_reg(0x60, 0x05); //
  delay(1);
  ym_set_reg(0x64, 0x05); //
  delay(1);
  ym_set_reg(0x68, 0x05); //
  delay(1);
  ym_set_reg(0x6C, 0x07); // AM/D1R
  delay(1);
  ym_set_reg(0x70, 0x02); //
  delay(1);
  ym_set_reg(0x74, 0x02); //
  delay(1);
  ym_set_reg(0x78, 0x02); //
  delay(1);
  ym_set_reg(0x7C, 0x02); // D2R
  delay(1);
  ym_set_reg(0x80, 0x11); //
  delay(1);
  ym_set_reg(0x84, 0x11); //
  delay(1);
  ym_set_reg(0x88, 0x11); //
  delay(1);
  ym_set_reg(0x8C, 0xA6); // D1L/RR
  delay(1);
  ym_set_reg(0x90, 0x00); //
  delay(1);
  ym_set_reg(0x94, 0x00); //
  delay(1);
  ym_set_reg(0x98, 0x00); //
  delay(1);
  ym_set_reg(0x9C, 0x00); // Proprietary
  delay(1);
  ym_set_reg(0xB0, 0x32); // Feedback/algorithm
  delay(1);
  ym_set_reg(0xB4, 0xC0); // Both speakers on
  delay(1);
  ym_set_reg(0x28, 0x00); // Key off
  delay(1);
  ym_set_reg(0xA4, 0x22); //
  delay(1);
  ym_set_reg(0xA0, 0x69); // Set frequency
  delay(1);
}

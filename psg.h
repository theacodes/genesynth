#pragma once

#include <util/delay.h> // For timing
#include "nanodelay.h"

// Pin 14-22 to PSG DATA (shared with YM)
#define PSG_DATA 14
#define PSG_WE 34

#define PSG_WAIT 150


void psg_setup() {
  /* Setup the PSG's pins. */
  for (int i = 0; i < 8; i++) {
    pinMode(YM_DATA + i, OUTPUT);
    digitalWriteFast(PSG_DATA + i, LOW);
  }
  pinMode(PSG_WE, OUTPUT);
  digitalWriteFast(PSG_WE, HIGH);
}

void psg_send_byte(byte data) {
  for(int i = 0; i < 8; i++)
  {
    digitalWriteFast(PSG_DATA + i, ((data >> i) & 1));
  }
  digitalWriteFast(PSG_WE, LOW);
  delay10ns(PSG_WAIT);
  digitalWriteFast(PSG_WE, HIGH);
}

void psg_reset() {
  /* Reset the PSG's state. */
  psg_send_byte(0x9f);
  psg_send_byte(0xbf);
  psg_send_byte(0xdf);
  psg_send_byte(0xff);
}

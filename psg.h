#pragma once

#include <util/delay.h> // For timing
#include "nanodelay.h"

// Pin 14-22 to PSG DATA (shared with YM)
//#define PSG_DATA 14
const byte PSG_DATA[] = {39, 38, 37, 36, 35, 27, 26, 25};
#define PSG_WE 34

#define PSG_WAIT 500


void psg_setup() {
  /* Setup the PSG's pins. */
  for (int i = 0; i < 8; i++) {
    pinMode(PSG_DATA[i], OUTPUT);
    digitalWriteFast(PSG_DATA[i], LOW);
  }
  pinMode(PSG_WE, OUTPUT);
  digitalWriteFast(PSG_WE, LOW);
}

void psg_send_byte(byte data) {
  digitalWriteFast(PSG_WE, HIGH);
  for(int i = 0; i < 8; i++)
  {
    digitalWriteFast(PSG_DATA[i], ((data >> i) & 1));
  }
  digitalWriteFast(PSG_WE, LOW);
  delay10ns(PSG_WAIT);
  //delay(1);
  digitalWriteFast(PSG_WE, HIGH);
}

void psg_reset() {
  /* Reset the PSG's state. */
  psg_send_byte(0x9f);
  psg_send_byte(0xbf);
  psg_send_byte(0xdf);
  psg_send_byte(0xff);
}

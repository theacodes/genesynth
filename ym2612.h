#pragma once

#include <util/delay.h> // For timing
#include "nanodelay.h"

// Pin 14-22 to YM DATA
#define YM_DATA 14
// Pin 33 to YM IC
#define YM_IC 33
// Pin 32 to YM A1
#define YM_A1 32
// Pin 31 to YM A0
#define YM_A0 31
// Pin 30 to YM WR
#define YM_WR 29
// Pin 29 to YM CS
#define YM_CS 28
// Pin 28 to YM RD (unused)
#define YM_RD 30
// How long to hold the WR and CS lines when writing data.
// The datasheet indicates this should be *at least* 200nS, but it appears
// that a setting of (200) ~2,000uS ensures that the chip always gets our messages.
#define YM_WRITE_WAIT 11 // * 10uS
// How long to hold the data lines after pulling the WR and CS lines.
// The datasheet indicates this should be *at least* 20ns, but it appears
// that a setting of 100 (~1,000uS) ensures that the chip always gets our messages.
#define YM_DATA_WAIT 11 // * 10uS
// How long to hold the reset line.
// According to bus timing documents, we must hold the line low for at least
// 192 cycles. At 8hmz, that's 24 microseconds.
#define YM_RESET_WAIT 24 // uS


void ym_setup() {
  /* Setup the YM's pins. */
  for (int i = 0; i < 8; i++) {
    pinMode(YM_DATA + i, OUTPUT);
    digitalWriteFast(YM_DATA + i, LOW);
  }
  pinMode(YM_IC, OUTPUT);
  pinMode(YM_A1, OUTPUT);
  pinMode(YM_A0, OUTPUT);
  pinMode(YM_WR, OUTPUT);
  pinMode(YM_CS, OUTPUT);
  pinMode(YM_RD, OUTPUT);
  digitalWriteFast(YM_IC, HIGH);
  digitalWriteFast(YM_A1, LOW);
  digitalWriteFast(YM_A0, LOW);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  digitalWriteFast(YM_RD, HIGH);
}

void ym_reset() {
  /* Reset the YM's state. */
  digitalWriteFast(YM_IC, HIGH);
  delayMicroseconds(YM_RESET_WAIT);
  digitalWriteFast(YM_IC, LOW);
  delayMicroseconds(YM_RESET_WAIT);
  digitalWriteFast(YM_IC, HIGH);
  delayMicroseconds(YM_RESET_WAIT);
}


inline static void ym_set_data(byte b)
{
  /* Sets the data lines to the YM to the given byte. */
  for (int i = 0; i < 8; i++)
  {
    digitalWriteFast(YM_DATA + i, ((b >> i) & 1));
  }
}

void ym_set_reg(byte address, byte data) {
  // Write the register address first.
  digitalWriteFast(YM_A1, LOW);
  digitalWriteFast(YM_A0, LOW);
  ym_set_data(address);
  // technically we should give *some* time between setting the bytes and
  // pulling the pins, but it seems to work fine without that.
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  // We shouldn't raise the WR and CS until at least YM_WRITE_WAIT have passed.
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  // We must wait for the chip to finish reading our data before loading in the
  // next byte.
  //delay10ns(YM_DATA_WAIT);
  delay10ns(1);

  // Same as above, but with A0 high to write the register data.
  digitalWriteFast(YM_A0, HIGH);
  ym_set_data(data);
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  delay10ns(YM_DATA_WAIT);
}

void ym_set_reg(byte address, byte data, int port) {
  // Write the register address first.
  digitalWriteFast(YM_A1, port);
  digitalWriteFast(YM_A0, LOW);
  ym_set_data(address);
  // technically we should give *some* time between setting the bytes and
  // pulling the pins, but it seems to work fine without that.
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  // We shouldn't raise the WR and CS until at least YM_WRITE_WAIT have passed.
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  // We must wait for the chip to finish reading our data before loading in the
  // next byte.
  delay10ns(YM_DATA_WAIT);

  // Same as above, but with A0 high to write the register data.
  digitalWriteFast(YM_A0, HIGH);
  ym_set_data(data);
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  delay10ns(YM_DATA_WAIT);
}

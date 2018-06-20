#pragma once

#include <util/delay.h> // For timing
#include "nanodelay.h"
#include "display.h"

// Pin 14-22 to PSG DATA (shared with YM)
//#define PSG_DATA 14
const byte PSG_DATA[] = {39, 38, 37, 36, 35, 27, 26, 25};
#define PSG_WE 34
#define PSG_WAIT 500

#define PSG_LATCH 0b10000000

const char PSG_CHANNEL_SELECT[] = {
  0b00000000,  // Tone 1
  0b00100000,  // Tone 2
  0b01000000,  // Tone 3
  0b01100000  // White Noise
};

#define PSG_VOL_REG 0b00010000
#define PSG_TONE_REG 0b00000000

#define PSG_LSB_MASK 0b00001111  // 4-bits LSB of data
#define PSG_MSB_MASK 0b00111111  // 6-bits MSB of data


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
  digitalWriteFast(PSG_WE, HIGH);
}

void psg_set_channel_freq(int channel, float freq) {
  //                       Input clock (Hz) (3579545)
  // Frequency (Hz) = ----------------------------------
  //                   2 x register value x divider (16)
  uint16_t pitch_value = 3570000 / float(32 * freq);

  if (pitch_value > 1023) {
    return;
  }

  byte cmd = PSG_LATCH | PSG_TONE_REG | PSG_CHANNEL_SELECT[channel] | (pitch_value & PSG_LSB_MASK);
  psg_send_byte(cmd);
  delay(1); // TODO
  byte data = pitch_value >> 4;
  psg_send_byte(data);
  delay(1);
}

void psg_set_channel_vol(int channel, byte vol) {
  // Map volume from 0-255 to 0-15, and then invert it.
  byte vol_value = 15 - byte(float(vol) / 255.f * 15.f);
  byte cmd = PSG_LATCH | PSG_VOL_REG | PSG_CHANNEL_SELECT[channel] | (vol_value & PSG_LSB_MASK);
  psg_send_byte(cmd);
  delay(1); // TODO
}

void psg_reset() {
  /* Reset the PSG's state. */
  psg_send_byte(0x9f);
  psg_send_byte(0xbf);
  psg_send_byte(0xdf);
  psg_send_byte(0xff);
}

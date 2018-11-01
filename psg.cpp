#include "psg.h"
#include "nanodelay.h"
#include <arduino.h>
#include <util/delay.h> // For timing

namespace thea {
namespace psg {

// Pin 14-21 to PSG DATA (shared with YM)
const uint8_t PSG_DATA[] = {14, 15, 16, 17, 18, 19, 20, 21};
// const byte PSG_DATA[] = {39, 38, 37, 36, 35, 27, 26, 25};
#define PSG_WE 34
#define PSG_WAIT 500

#define PSG_LATCH 0b10000000

const char PSG_CHANNEL_SELECT[] = {
    0b00000000, // Tone 1
    0b00100000, // Tone 2
    0b01000000, // Tone 3
    0b01100000  // White Noise
};

#define PSG_VOL_REG 0b00010000
#define PSG_TONE_REG 0b00000000

#define PSG_LSB_MASK 0b00001111 // 4-bits LSB of data
#define PSG_MSB_MASK 0b00111111 // 6-bits MSB of data

void setup() {
  /* Setup the PSG's pins. */
  for (int i = 0; i < 8; i++) {
    pinMode(PSG_DATA[i], OUTPUT);
    digitalWriteFast(PSG_DATA[i], LOW);
  }
  pinMode(PSG_WE, OUTPUT);
  digitalWriteFast(PSG_WE, LOW);
}

void reset() {
  /* Reset the PSG's state. */
  send_byte(0x9f);
  send_byte(0xbf);
  send_byte(0xdf);
  send_byte(0xff);
}

void send_byte(uint8_t data) {
  digitalWriteFast(PSG_WE, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWriteFast(PSG_DATA[i], ((data >> i) & 1));
  }
  digitalWriteFast(PSG_WE, LOW);
  delay10ns(PSG_WAIT);
  digitalWriteFast(PSG_WE, HIGH);
}

void set_channel_freq(int channel, float freq) {
  //                       Input clock (Hz) (3579545)
  // Frequency (Hz) = ----------------------------------
  //                   2 x register value x divider (16)
  uint16_t pitch_value = 3570000 / float(32 * freq);

  if (pitch_value > 1023) {
    return;
  }

  uint8_t cmd = PSG_LATCH | PSG_TONE_REG | PSG_CHANNEL_SELECT[channel] | (pitch_value & PSG_LSB_MASK);
  send_byte(cmd);
  delay(1); // TODO
  uint8_t data = pitch_value >> 4;
  send_byte(data);
  delay(1);
}

void set_channel_vol(int channel, uint8_t vol) {
  // Map volume from 0-255 to 0-15, and then invert it.
  uint8_t vol_value = 15 - byte(float(vol) / 255.f * 15.f);
  uint8_t cmd = PSG_LATCH | PSG_VOL_REG | PSG_CHANNEL_SELECT[channel] | (vol_value & PSG_LSB_MASK);
  send_byte(cmd);
  delay(1); // TODO
}

} // namespace psg
} // namespace thea
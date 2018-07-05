#ifndef THEA_PSG_H
#define THEA_PSG_H

namespace thea {
namespace psg {

void setup();

void reset();

void send_byte(uint8_t data);

void set_channel_freq(int channel, float freq);

void set_channel_vol(int channel, uint8_t vol);

} //namespace psg
} //namespace thea

#endif
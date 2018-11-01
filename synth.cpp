#include "synth.h"

namespace thea {
namespace synth {

#define YM_CHANNELS 6

thea::ym2612::ChannelPatch patch;
uint8_t active_notes[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};

void play_note(uint8_t note, float pitch) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    if (active_notes[i] == 0) {
      thea::ym2612::set_channel_freq(i, pitch);
      active_notes[i] = note;
      break;
    }
  }
}

void stop_note(uint8_t note) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    if (active_notes[i] == note) {
      // TODO: Move to ym2612.cpp
      int port = i < 3 ? 0 : 1;
      uint8_t channel_offset = (i % 3);
      uint8_t key_offset = channel_offset | (port << 2);
      thea::ym2612::set_reg(0x28, key_offset); // Key off
      active_notes[i] = 0;
    }
  }
}

void update_patch(thea::ym2612::ChannelPatch::WriteOption option) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    thea::synth::patch.write_to_channel(i, option);
  }
}

} // namespace synth
} // namespace thea

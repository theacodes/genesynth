#include <Arduino.h>
#include "synth.h"

namespace thea {
namespace synth {

#define YM_CHANNELS 6

thea::ym2612::ChannelPatch patch;
thea::ym2612::ChannelPatch::WriteOption last_write_option;
unsigned long last_patch_modify_time;
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

void modify_patch_parameter(thea::ym2612::ChannelPatch::WriteOption option, uint8_t normalized_value) {
  auto normalized_option =
      thea::ym2612::ChannelPatch::WriteOption(option % thea::ym2612::ChannelPatch::WriteOption::OP0_AM);
  uint8_t operator_no = option / thea::ym2612::ChannelPatch::WriteOption::OP0_AM;

  switch (normalized_option) {
  case thea::ym2612::ChannelPatch::WriteOption::OP0_DT1:
    patch.operators[operator_no].DT1 = map(normalized_value, 0, 127, 0, 7);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_MUL:
    patch.operators[operator_no].MUL = map(normalized_value, 0, 127, 0, 15);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_TL:
    patch.operators[operator_no].TL = normalized_value;
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_AR:
    patch.operators[operator_no].AR = map(normalized_value, 0, 127, 0, 31);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D1R:
    patch.operators[operator_no].D1R = map(normalized_value, 0, 127, 0, 31);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D2R:
    patch.operators[operator_no].D2R = map(normalized_value, 0, 127, 0, 31);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D1L:
    patch.operators[operator_no].D1L = map(normalized_value, 0, 127, 0, 31);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_RR:
    patch.operators[operator_no].RR = map(normalized_value, 0, 127, 0, 15);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_RS:
    patch.operators[operator_no].RS = map(normalized_value, 0, 127, 0, 3);
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_AM:
    patch.operators[operator_no].AM = normalized_value;
    break;
  default:
    break;
  }
}

void update_patch(thea::ym2612::ChannelPatch::WriteOption option) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    thea::synth::patch.write_to_channel(i, option);
  }
  last_write_option = option;
  last_patch_modify_time = micros();
}

} // namespace synth
} // namespace thea

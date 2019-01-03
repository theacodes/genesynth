#include "synth.h"
#include "patch_loader.h"
#include <Arduino.h>
#include <EEPROM.h>

namespace thea {
namespace synth {

#define DEBUG_EEPROM_RW 0
#define YM_CHANNELS 6
#define EEPROM_PATCH_PRESENT_ADDR 0x09
#define EEPROM_PATCH_ADDR 0x10

thea::ym2612::ChannelPatch patch;
thea::ym2612::ChannelPatch::WriteOption last_write_option;
unsigned long last_patch_modify_time;
// The active midi note numbers, used for polyphony tracking.
uint8_t active_notes[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};
float active_pitches[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};
float pitch_bend_multiplier = 1.f;

void play_note(uint8_t note, float pitch) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    if (active_notes[i] == 0) {
      thea::ym2612::stop_note(i);
      thea::ym2612::set_channel_freq(i, pitch * pitch_bend_multiplier);
      thea::ym2612::play_note(i);
      active_notes[i] = note;
      active_pitches[i] = pitch;
      break;
    }
  }
}

void stop_note(uint8_t note) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    if (active_notes[i] == note) {
      thea::ym2612::stop_note(i);
      active_notes[i] = 0;
    }
  }
}

void pitch_bend(float offset) {
  // Adjustable: a 1.0 multiplier means that the pitch wheel all the way up *doubles* the frequency.
  // A 0.5 multipler lowers the range a bit, making it a bit more musically usabe.
  offset *= 0.5f;

  pitch_bend_multiplier = 1.f + offset;
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    if (active_notes[i] != 0) {
      thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
    }
  }
}

void write_patch_to_eeprom(thea::ym2612::ChannelPatch &patch) {
  auto byte_array = (char *)&patch;
  for (size_t i = 0; i < sizeof(patch); i++) {
#if DEBUG_EEPROM_RW
    Serial.printf("%02X ", byte_array[i]);
    if (i % 16 == 15)
      Serial.printf("\n");
#endif

    EEPROM.update(EEPROM_PATCH_ADDR + i, byte_array[i]);
  }

#if DEBUG_EEPROM_RW
  Serial.printf("\n\n");
#endif

  Serial.printf("Wrote patch to EEPROM, wrote %i bytes.\n", sizeof(patch));
}

void read_patch_from_eeprom(thea::ym2612::ChannelPatch *patch) {
  auto byte_array = (char *)patch;
  for (size_t i = 0; i < sizeof(*patch); i++) {
    char value = EEPROM.read(EEPROM_PATCH_ADDR + i);

#if DEBUG_EEPROM_RW
    Serial.printf("%02X ", value);
    if (i % 16 == 15)
      Serial.printf("\n");
#endif

    byte_array[i] = value;
  }

#if DEBUG_EEPROM_RW
  Serial.printf("\n\n");
#endif

  Serial.printf("Read patch from EEPROM, read %i bytes.\n", sizeof(*patch));
  Serial.printf("Patch name: %s\n", patch->name);
}

void modify_patch_parameter(thea::ym2612::ChannelPatch::WriteOption option, uint8_t normalized_value) {
  auto normalized_option = thea::ym2612::ChannelPatch::WriteOption(option % 10);
  uint8_t operator_no = option / 10;

  // Serial.printf("Option: %i, Normalized: %i, Operator: %i\n", option, normalized_option, operator_no);

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
  write_patch_to_eeprom(patch);
}

void load_patch(SdFile &file, SdFile *folder) {
  bool success = thea::patch_loader::load_from_sd_file(file, folder, &patch);

  if (!success) {
    Serial.printf("Failed to load patch!");
  }

  update_patch();
}

void load_last_patch() {
  read_patch_from_eeprom(&patch);
  update_patch();
}

void init() { load_last_patch(); }

} // namespace synth
} // namespace thea

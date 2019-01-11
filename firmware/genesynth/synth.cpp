#include <Arduino.h>
#include <EEPROM.h>

#include "hardware_constants.h"
#include "src/theacommon/note_stack.h"
#include "synth.h"
#include "tfi_parser.h"

namespace thea {
namespace synth {

#define YM_CHANNELS 6

thea::ym2612::ChannelPatch patch;
thea::ym2612::ChannelPatch::WriteOption last_write_option;
unsigned long last_patch_modify_time;

// The note mode, used for switching between mono, unison, and poly.
NoteMode mode = NoteMode::UNISON;

// The active midi note numbers for polyphony tracking.
// TODO: Replace this with a "VoiceStack".
uint8_t active_notes[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};

// The currently set pitches for each channel. Used by all note modes.
float active_pitches[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};

// Channel spread is used exclusively by the unison mode.
float channel_spread_multipliers[YM_CHANNELS] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

// Pitch bend is applied to all modes when setting the channel frequency.
float pitch_bend_multiplier = 1.f;

// The note stack is used exclusively by the unison and mono modes.
NoteStack mono_note_stack;

inline bool approximately_equal(float a, float b, float epsilon) {
  return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

void play_note_poly(uint8_t note, float pitch) {
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

void play_note_unison(uint8_t note, float pitch, unsigned int voices) {
  bool retrigger = mono_note_stack.is_empty();

  mono_note_stack.push(note, pitch);

  if (retrigger) {
    for (unsigned int i = 0; i < voices; i++) {
      auto target_pitch = pitch * channel_spread_multipliers[i];
      active_pitches[i] = target_pitch;
      thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
      thea::ym2612::play_note(i);
    }
  }
}

void play_note(uint8_t note, float pitch) {
  switch (mode) {
  case NoteMode::POLY:
    play_note_poly(note, pitch);
    break;
  default:
    play_note_unison(note, pitch, 6);
    break;
  }
}

void stop_note_poly(uint8_t note) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    if (active_notes[i] == note) {
      thea::ym2612::stop_note(i);
      active_notes[i] = 0;
    }
  }
}

void stop_note_unison(uint8_t note) {
  // This situation shouldn't happen, but if it does, don't panic.
  if (mono_note_stack.is_empty())
    return;

  mono_note_stack.pop(note);

  if (mono_note_stack.is_empty()) {
    for (uint8_t i = 0; i < YM_CHANNELS; i++) {
      thea::ym2612::stop_note(i);
      active_notes[i] = 0;
    }
  } else {
    // update_target_pitches will change the pitch to the current top of stack
    // note.
    update_target_pitches();
  }
}

void stop_note(uint8_t note) {
  switch (mode) {
  case NoteMode::POLY:
    stop_note_poly(note);
    break;
  default:
    stop_note_unison(note);
    break;
  }
}

void stop_all_notes(uint8_t note) {
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    thea::ym2612::stop_note(i);
    active_notes[i] = 0;
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

void update_target_pitches() {
  // TODO: Only run this for mono/unison.
  if (mono_note_stack.is_empty())
    return;

  auto current_note = mono_note_stack.top();

  // TODO: Change from YM_CHANNELS to VOICES.
  for (uint8_t i = 0; i < YM_CHANNELS; i++) {
    auto target_pitch = current_note.pitch * channel_spread_multipliers[i];

    if (active_pitches[i] == target_pitch)
      continue;

    active_pitches[i] += ((target_pitch - active_pitches[i]) / 100.0f) * channel_spread_multipliers[i];

    if (approximately_equal(active_pitches[i], target_pitch, 0.01f)) {
      active_pitches[i] = target_pitch;
    }

    thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
  }
}

void set_unison_spread(float spread) {
  channel_spread_multipliers[0] = 1.0f;
  channel_spread_multipliers[1] = 1.0f + spread;
  channel_spread_multipliers[2] = 1.0f - spread;
  channel_spread_multipliers[3] = 1.0f + (spread * 0.56);
  channel_spread_multipliers[4] = 1.0f - (spread * 0.34);
  channel_spread_multipliers[5] = 1.0f + (spread * 0.23);
}

void change_mode(NoteMode mode) {
  thea::synth::mode = mode;
  stop_all_notes();
}

NoteMode get_mode() { return mode; }

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
}

void load_patch(SdFile &file, SdFile *folder) {
  bool success = thea::tfi::load(file, folder, &patch);

  if (!success) {
    Serial.printf("Failed to load patch!");
  }

  update_patch();
  write_patch_to_eeprom(patch);
}

void load_last_patch() {
  read_patch_from_eeprom(&patch);
  update_patch();
}

void init() {
  load_last_patch();
  set_unison_spread(0.02f);
}

void loop() { update_target_pitches(); }

} // namespace synth
} // namespace thea

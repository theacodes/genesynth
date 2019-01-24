#include <Arduino.h>
#include <EEPROM.h>

#include "hardware_constants.h"
#include "src/theacommon/note_stack.h"
#include "synth.h"
#include "tfi_parser.h"

namespace thea {
namespace synth {

#define YM_CHANNELS 6

/*
  Patch management and eeprom interaction.
*/

thea::ym2612::ChannelPatch patch;
thea::ym2612::ChannelPatch::WriteOption last_write_option;
unsigned long last_patch_modify_time;

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

void save_patch(SdFile &file) { thea::tfi::save(file, patch); };

/*
  Note handling
*/

inline bool approximately_equal(float a, float b, float epsilon) {
  return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

/* Encapsulates common functionality for poly, mono, and unison. */
class NoteStrategy {
public:
  virtual ~NoteStrategy(){};

  virtual void reset();

  /* Called every loop() to update pitches, etc. */
  virtual void update();

  virtual void note_on(uint8_t note, float pitch);
  virtual void note_off(uint8_t note);

  virtual void pitch_bend(float offset) {
    // Adjustable: a 1.0 multiplier means that the pitch wheel all the way up *doubles* the frequency.
    // A 0.5 multipler lowers the range a bit, making it a bit more musically usabe.
    offset *= 0.5f;

    pitch_bend_multiplier = 1.f + offset;
  }

protected:
  // Pitch bend is applied to all modes when setting the channel frequency.
  float pitch_bend_multiplier = 1.f;
};

/* Unison strategy is used for both monophonic and unison modes. Mono just
 uses one voice. */
class UnisonStrategy : public NoteStrategy {
public:
  virtual void reset() {
    note_stack.clear();
    for (uint8_t i = 0; i < YM_CHANNELS; i++) {
      thea::ym2612::stop_note(i);
    }
  }

  virtual void update() {
    // This is only needed if there's notes on the stack and if glide is enabled.
    if (note_stack.is_empty() || !glide)
      return;

    auto current_note = note_stack.top();

    // Glide is normalized (0-1.f), mulitply it by a useful time constant.
    float glide_denominator = glide_amount * 300.0f;
    // Avoid divide by 0.
    if (glide_denominator < 1.0f)
      glide_denominator = 1.0f;

    for (uint8_t i = 0; i < voices; i++) {
      auto target_pitch = current_note.pitch * channel_spread_multipliers[i];

      if (active_pitches[i] == target_pitch)
        continue;

      active_pitches[i] += ((target_pitch - active_pitches[i]) / glide_denominator) * channel_spread_multipliers[i];

      if (approximately_equal(active_pitches[i], target_pitch, 0.01f)) {
        active_pitches[i] = target_pitch;
      }

      thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
    }
  }

  virtual void note_on(uint8_t note, float pitch) {
    bool retrigger = !glide || note_stack.is_empty();

    // If glide is off, we'll need to stop
    // the previous note.
    bool stop_previous = !glide;

    note_stack.push(note, pitch);

    if (retrigger) {
      for (uint8_t i = 0; i < voices; i++) {
        auto target_pitch = pitch * channel_spread_multipliers[i];
        active_pitches[i] = target_pitch;

        if (stop_previous)
          thea::ym2612::stop_note(i);

        thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
        thea::ym2612::play_note(i);
      }
    }
  }

  virtual void note_off(uint8_t note) {
    // This situation shouldn't happen, but if it does, don't panic.
    if (note_stack.is_empty())
      return;

    note_stack.pop(note);

    if (note_stack.is_empty()) {
      for (uint8_t i = 0; i < YM_CHANNELS; i++) {
        thea::ym2612::stop_note(i);
      }
    } else {
      if (!glide) {
        // The note at the top of the stack needs to be re-triggered.
        // The easiest way to do this is just pop it off and re-push it with
        // note_on.
        auto next_note = note_stack.top();
        note_stack.pop(next_note.note);
        note_on(next_note.note, next_note.pitch);
      } else {
        // update() will change the pitch to the current top of stack
        // note.
      }
    }
  }

  void pitch_bend(float offset) {
    NoteStrategy::pitch_bend(offset);
    for (uint8_t i = 0; i < voices; i++) {
      thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
    }
  }

  void set_spread(float spread) {
    // Spread is a normalized value between 0-1.f, reduce it to a usable value.
    auto spread_amount = spread * 0.1f;
    channel_spread_multipliers[0] = 1.0f;
    channel_spread_multipliers[1] = 1.0f + spread_amount;
    channel_spread_multipliers[2] = 1.0f - spread_amount;
    channel_spread_multipliers[3] = 1.0f + (spread_amount * 0.56);
    channel_spread_multipliers[4] = 1.0f - (spread_amount * 0.34);
    channel_spread_multipliers[5] = 1.0f + (spread_amount * 0.23);
  }

  float get_spread() { return channel_spread_multipliers[1] - 1.0f / 0.1f; }

  void set_voices(uint8_t voices) { this->voices = voices; }

  uint8_t get_voices() { return voices; }

  void enable_glide() { glide = true; }

  void disable_glide() { glide = false; }

  bool glide_enabled() { return glide; }

  void set_glide_amount(float glide_amount) { this->glide_amount = glide_amount; }

  float get_glide_amount() { return glide_amount; }

private:
  uint8_t voices = 6;
  bool glide = false;
  float glide_amount = 0.1f;
  NoteStack note_stack;
  /* Track each channel's pitches separately. This allows the spread multiplier
    to be applied to each channel's frequency as we interpolate through the
    glide parameter (see update()). */
  float active_pitches[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};
  float channel_spread_multipliers[YM_CHANNELS] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
};

class PolyStrategy : public NoteStrategy {
public:
  virtual void reset() {
    for (uint8_t i = 0; i < YM_CHANNELS; i++) {
      thea::ym2612::stop_note(i);
      active_notes[i] = 0;
    }
  }

  virtual void update() {}

  virtual void note_on(uint8_t note, float pitch) {
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

  virtual void note_off(uint8_t note) {
    for (uint8_t i = 0; i < YM_CHANNELS; i++) {
      if (active_notes[i] == note) {
        thea::ym2612::stop_note(i);
        active_notes[i] = 0;
      }
    }
  }

  void pitch_bend(float offset) {
    NoteStrategy::pitch_bend(offset);
    for (uint8_t i = 0; i < YM_CHANNELS; i++) {
      if (active_notes[i] != 0) {
        thea::ym2612::set_channel_freq(i, active_pitches[i] * pitch_bend_multiplier);
      }
    }
  }

private:
  // The active midi note numbers for each channel.
  uint8_t active_notes[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};
  // The currently set pitches for each channel.
  float active_pitches[YM_CHANNELS] = {0, 0, 0, 0, 0, 0};
};

UnisonStrategy unison;
UnisonStrategy mono;
PolyStrategy poly;
NoteMode current_note_mode = NoteMode::POLY;
NoteStrategy *current_note_strategy = &poly;

void play_note(uint8_t note, float pitch) { current_note_strategy->note_on(note, pitch); }

void stop_note(uint8_t note) { current_note_strategy->note_off(note); }

void stop_all_notes() { current_note_strategy->reset(); }

void pitch_bend(float offset) { current_note_strategy->pitch_bend(offset); }

void set_note_mode(NoteMode mode) {
  current_note_strategy->reset();
  current_note_mode = mode;
  switch (mode) {
  case NoteMode::UNISON:
    current_note_strategy = &unison;
    break;
  case NoteMode::MONO:
    current_note_strategy = &mono;
    break;
  case NoteMode::POLY:
    current_note_strategy = &poly;
    break;
  default:
    current_note_strategy = &poly;
    break;
  }
  current_note_strategy->reset();
}

NoteMode get_note_mode() { return current_note_mode; }

// Set the glide amount.
void enable_glide() {
  mono.enable_glide();
  unison.enable_glide();
}

void disable_glide() {
  mono.disable_glide();
  unison.disable_glide();
}

bool glide_enabled() { return mono.glide_enabled(); }

void set_glide_amount(float glide) {
  mono.set_glide_amount(glide);
  unison.set_glide_amount(glide);
}

float get_glide_amount() { return mono.get_glide_amount(); }

// Set the number of unison voices.
void set_unison_voices(int voices) { unison.set_voices(voices); }

int get_unison_voices() { return unison.get_voices(); }

// Set the unison spread
void set_unison_spread(float spread) { unison.set_spread(spread); }

float get_unison_spread() { return unison.get_spread(); }

/*
  Init() and Loop() functions.
*/

void init() {
  mono.set_voices(1);
  unison.set_spread(0.01f);
  load_last_patch();
}

void loop() { current_note_strategy->update(); }

} // namespace synth
} // namespace thea

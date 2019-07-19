#include <Arduino.h>

#include "filesystem.h"
#include "midi_interface.h"
#include "param_mapping.h"
#include "synth.h"
#include "ym2612.h"

namespace thea {
namespace midi_interface {

#define SYSEX_SAVE_PATCH_COMMAND_BYTE 0x1A

struct NRPNMessage {
  uint16_t parameter = 0;
  uint16_t value = 0;
  bool ready = false;
};

NRPNMessage nrpn_message;
uint8_t last_cc;

uint8_t get_last_cc() { return last_cc; }

void handle_custom_cc_mapping(byte control, byte value);

void handle_note_on(byte channel, byte note, byte velocity) {
  float pitch = pow(2, float(note - 69) / 12) * 440;

  if (channel == 1) {
    thea::synth::play_note(note, pitch);
  }

  // Send velocity as a custom CC mapping.
  handle_custom_cc_mapping(thea::params::VELOCITY, velocity);

  // Set the last CC to the special value. This allows MIDI learn
  // to map params to velocity.
  last_cc = thea::params::VELOCITY;
}

void handle_note_off(byte channel, byte note, byte velocity) {
  if (channel == 1) {
    thea::synth::stop_note(note);
  }
}

void handle_pitch_bend(byte channel, int amount) {
  if (channel != 1)
    return;

  float offset = float(amount) / 8192.0f;

  thea::synth::pitch_bend(offset);
}

void handle_nrpn_message(const NRPNMessage nrpn_message) {

  /* Messages 10 through 49 are used to change operator parameters (9 per operator). */
  if (nrpn_message.parameter >= 10 && nrpn_message.parameter < 50) {
    auto option = thea::ym2612::ChannelPatch::WriteOption(nrpn_message.parameter - 10);
    thea::synth::modify_patch_parameter(option, nrpn_message.value);
    thea::synth::update_patch(option);
    return;
  }

  auto option = thea::ym2612::ChannelPatch::WriteOption::ALL;

  switch (nrpn_message.parameter) {
  /* Message 8 controls the FM algorithm. */
  case 8:
    option = thea::ym2612::ChannelPatch::WriteOption::ALGORITHM;
    thea::synth::modify_patch_parameter(option, nrpn_message.value);
    thea::synth::update_patch(option);
    break;

  /* Message 9 controls the feedback. */
  case 9:
    option = thea::ym2612::ChannelPatch::WriteOption::FEEDBACK;
    thea::synth::modify_patch_parameter(option, nrpn_message.value);
    thea::synth::update_patch(option);
    break;

  /* Messages 50-5x are used to control LFO parameters. */
  case 50:
    if (nrpn_message.value <= 63) {
      thea::synth::disable_lfo();
    } else {
      thea::synth::enable_lfo();
    }
    break;

  case 51:
    thea::synth::set_lfo_freq(nrpn_message.value);
    break;

  case 52:
    thea::synth::set_lfo_fms(nrpn_message.value);
    break;

  case 53:
    thea::synth::set_lfo_ams(nrpn_message.value);
    break;

  default:
    Serial.printf("Unknown NRPN message param: %i, value: %i\n", nrpn_message.parameter, nrpn_message.value);
    break;
  }
};

void handle_custom_cc_mapping(byte control, byte value) {
  thea::params::ParamMapping mapping = thea::params::get_mapping_for_cc(control);
  if (mapping.midi_cc == 0)
    return;

  /* Map this to a NRPN message, since it's the easiest way to deal with this
  stuff. */
  NRPNMessage msg;
  msg.ready = true;

  switch (mapping.param) {
  // Alg
  case 0:
    msg.parameter = 8;
    break;
  // Feedback
  case 1:
    msg.parameter = 9;
    break;
  // LFO Enable
  case 2:
    msg.parameter = 50;
    break;
  // LFO Freq
  case 3:
    msg.parameter = 51;
    break;
  // LFO FMS
  case 4:
    msg.parameter = 52;
    break;
  // LFO AMS
  case 5:
    msg.parameter = 53;
    break;
  default:
    break;
  }

  if (mapping.param > 5) {
    // Operator parameters, map directly into the NRPN 10+ range.
    msg.parameter = 10 + mapping.param - 5;
  }

  // Failed to map it to anything, so just return.
  if (msg.parameter == 0)
    return;

  // Map the value using the easing function.
  float normalized = float(value) / 127.0f;
  normalized = thea::params::map_value(mapping.curve, normalized);

  msg.value = (uint8_t)(normalized * 127.0f);
  handle_nrpn_message(msg);
}

void handle_control_change(byte channel, byte control, byte value) {
  if (channel != 1)
    return;

  switch (control) {
  case 5: // Portamento Time (Glide)
    thea::synth::set_glide_amount(value / 127.0f);
    break;
  case 25: // Portamento Time LSB (Glide)
    // Ignored.
    break;
  case 41: // Portamento On/Off (<=63 off, >=64 on)
    if (value <= 63) {
      thea::synth::disable_glide();
    } else {
      thea::synth::enable_glide();
    }
    break;

  case 28: // Spread amount for unison voices / unison detune.
    thea::synth::set_unison_spread(value / 127.0f);
    break;

  case 126: // Set Mono/Unison mode (& number of voices.)
    // Mono: 0 or 1, Unison: >= 1.
    if (value <= 1) {
      thea::synth::set_note_mode(thea::synth::NoteMode::MONO);
    } else {
      thea::synth::set_note_mode(thea::synth::NoteMode::UNISON);
      // Cap at the number of YM2612 channels.
      if (value > 6)
        value = 6;
      thea::synth::set_unison_voices(value);
    }
    break;
  case 127: // Set Poly mode.
    thea::synth::set_note_mode(thea::synth::NoteMode::POLY);
    break;

  case 123:
  case 120: // All notes off.
    thea::synth::stop_all_notes();
    break;

  /* NRPN Handling. */
  case 99: // NRPN Parameter MSB
    nrpn_message.parameter = value << 7;
    break;
  case 98: // NRPN Parameter LSB
    nrpn_message.parameter = nrpn_message.parameter + value;
    break;
  case 6: // NRPN Parameter value MSB
    nrpn_message.value = value << 7;
    break;
  case 38: // NRPN Parameter value LSB
    nrpn_message.value = nrpn_message.value + value;
    nrpn_message.ready = true;
    handle_nrpn_message(nrpn_message);
    nrpn_message.ready = false;
    break;
  default:
    last_cc = control;
    handle_custom_cc_mapping(control, value);
    break;
  }
}

void save_patch(const char *filename) {
  SdFile root;
  root.openRoot(thea::filesystem::sd().vol());

  SdFile dir;
  auto success = dir.open(&root, "user", O_READ);

  if (!success || !dir.isDir()) {
    Serial.printf("Creating user patch directory.\n");
    dir.mkdir(&root, "user");
    dir.open(&root, "user", O_READ);
  } else {
    Serial.printf("User directory present.\n");
  }

  SdFile patch_file;

  if (!patch_file.open(&dir, filename, O_WRITE | O_TRUNC | O_CREAT)) {
    Serial.printf("Creating patch file failed!\n");
    return;
  }

  thea::synth::save_patch(patch_file);

  // Immediately load the patch so that the UI shows the new filename.
  if (!patch_file.open(&dir, filename, O_READ)) {
    Serial.printf("Saving the patch file failed!\n");
    return;
  }

  thea::synth::load_patch(patch_file, &dir);
}

void handle_system_exclusive(byte *data, unsigned int length) {
  Serial.printf("Got SysEx, first byte: 0x%02X, length: %i\n", data[0], length);

  if (length < 3) {
    // The message must contain the start byte (0xF0), any valid manufacturer byte,
    // and a command byte. If it's not long enough, return.
    return;
  }

  char command = data[2];
  auto payload_length = length - 4; // 3 for the header, 1 for the stop byte.
  byte *payload = data + 4;

  Serial.printf("Command: 0x%02X, payload length: %i\n", command, payload_length);

  if (command == SYSEX_SAVE_PATCH_COMMAND_BYTE) {
    char filename[128];
    strncpy(filename, (char *)payload, payload_length > 128 ? 128 : payload_length);
    Serial.printf("Got patch save command with filename: %s\n", filename);
    save_patch(filename);
  }
}

void setup() {
  usbMIDI.setHandleNoteOn(handle_note_on);
  usbMIDI.setHandleNoteOff(handle_note_off);
  usbMIDI.setHandlePitchChange(handle_pitch_bend);
  usbMIDI.setHandleControlChange(handle_control_change);
  usbMIDI.setHandleSystemExclusive(handle_system_exclusive);

  // Initiate MIDI communications, listen to all channels
  usbMIDI.begin();
}

void loop() {
  int count = 0;
  while (usbMIDI.read()) {
    count++;
  };
  // if (count)
  //   Serial.printf("Processed %i MIDI messages.\n", count);
}

} // namespace midi_interface
} // namespace thea

#include <Arduino.h>

#include "midi_interface.h"
#include "synth.h"
#include "ym2612.h"

namespace thea {
namespace midi_interface {

struct NRPNMessage {
  uint16_t parameter = 0;
  uint16_t value = 0;
  bool ready = false;
};

NRPNMessage nrpn_message;

void handleNoteOn(byte channel, byte note, byte velocity) {
  float pitch = pow(2, float(note - 69) / 12) * 440;

  if (channel == 1) {
    thea::synth::play_note(note, pitch);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (channel == 1) {
    thea::synth::stop_note(note);
  }
}

void handlePitchBend(byte channel, int amount) {
  if (channel != 1)
    return;

  float offset = float(amount) / 8192.0f;

  thea::synth::pitch_bend(offset);
}

void handle_nrpn_message(const NRPNMessage nrpn_message) {

  /* Messages 10 through 49 are used to change operator parameters (9 per operator). */
  if (nrpn_message.parameter >= 10 && nrpn_message.parameter < 50) {
    auto option = thea::ym2612::ChannelPatch::WriteOption::ALL;
    option = thea::ym2612::ChannelPatch::WriteOption(nrpn_message.parameter - 10);
    thea::synth::modify_patch_parameter(option, nrpn_message.value);
    thea::synth::update_patch(option);
    return;
  }

  Serial.printf("Unknown NRPN message param: %i, value: %i\n", nrpn_message.parameter, nrpn_message.value);
};

void handleControlChange(byte channel, byte control, byte value) {
  if (channel != 1)
    return;

  switch (control) {
  case 1: // Modulation wheel.
    // TODO
    break;
  case 21: // Modulation wheel LSB.
    // TODO
    break;

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
      // Leave the value unmodified.
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
    break;
  default:
    Serial.printf("Unmapped CC %i: %i.\n", control, value);
    break;
  }

  if (nrpn_message.ready) {
    handle_nrpn_message(nrpn_message);
    nrpn_message.ready = false;
  }
}

void handleSystemExclusive(byte *data, unsigned int length) {
  Serial.printf("Got SysEx, first byte: 0x%02X, length: %i\n", data[0], length);
}

void setup() {
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleNoteOff(handleNoteOff);
  usbMIDI.setHandlePitchChange(handlePitchBend);
  usbMIDI.setHandleControlChange(handleControlChange);
  usbMIDI.setHandleSystemExclusive(handleSystemExclusive);

  // Initiate MIDI communications, listen to all channels
  usbMIDI.begin();
}

void loop() {
  while (usbMIDI.read()) {
  };
}

} // namespace midi_interface
} // namespace thea

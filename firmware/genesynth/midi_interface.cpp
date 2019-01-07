#include <Arduino.h>

#include "midi_interface.h"
#include "synth.h"
#include "ym2612.h"

namespace thea {
namespace midi_interface {

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

void handleControlChange(byte channel, byte control, byte value) {
  if (channel != 1)
    return;

  auto option = thea::ym2612::ChannelPatch::WriteOption::ALL;

  /* Controllers 20-29 map to OP1, 30-39 to OP2, etc. parameters */
  if (control >= 20 && control <= 59) {
    option = thea::ym2612::ChannelPatch::WriteOption(control - 20);
  } else {
    Serial.printf("Unmapped CC %i: %i.\n", control, value);
    return; // Revisit
  }

  thea::synth::modify_patch_parameter(option, value);
  thea::synth::update_patch(option);
}

void handleSystemExclusive(byte *data, unsigned int length) {
  // We expect sysex messages to be two bytes long (excluding the start and end bytes).
  if (length - 2 != 2) {
    Serial.printf("Got SysEx of unexpected length: %i\n", length);
    return;
  }

  Serial.printf("Got SysEx: param %i value %i\n", data[1], data[2]);
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

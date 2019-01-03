#include "midi_interface.h"
#include "patch_loader.h"
#include "synth.h"
#include "ym2612.h"
#include <Arduino.h>

namespace thea {
namespace midi_interface {

int patch_no = 0;
int bank_no = 0;

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

void handleProgramChange(byte channel, byte program) {
  if (channel != 1)
    return;

  thea::patch_loader::load_nth_program(program, &thea::synth::patch);
  patch_no = program;

  thea::synth::update_patch();
}

void handleBankChange(byte channel, byte bank) {
  if (channel != 1)
    return;

  thea::patch_loader::load_nth_bank(bank);
  bank_no = bank;

  handleProgramChange(channel, 0);
}

void handleControlChange(byte channel, byte control, byte value) {
  if (channel != 1)
    return;

  if (control == 0) {
    // Bank change
    handleBankChange(channel, value);
    return;
  }

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
  usbMIDI.setHandleProgramChange(handleProgramChange);
  usbMIDI.setHandleControlChange(handleControlChange);
  usbMIDI.setHandleSystemExclusive(handleSystemExclusive);

  // Initiate MIDI communications, listen to all channels
  usbMIDI.begin();

  // // Load up first patch, if needed.
  // handleBankChange(1, 0);
  // handleProgramChange(1, 0);
}

void loop() {
  while (usbMIDI.read()) {
  };
}

} // namespace midi_interface
} // namespace thea

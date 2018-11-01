#include "midi_interface.h"
#include "buttons.h"
#include "display.h"
#include "patch_loader.h"
#include "synth.h"
#include "ym2612.h"
#include <Arduino.h>
#include <MIDI.h>
#include <midi_UsbTransport.h>

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;
UsbTransport sUsbTransport;
MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);

namespace thea {
namespace midi_interface {

int patch_no = 0;

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

void handleProgramChange(byte channel, byte program) {
  if (channel != 1)
    return;

  thea::patch_loader::load_nth(program, &thea::synth::patch);
  patch_no = program;

  thea::synth::update_patch();
}

void handleControlChange(byte channel, byte control, byte value) {
  if (channel != 1)
    return;

  // Serial.printf("Got control change %i: %i\n", control, value);

  auto option = thea::ym2612::ChannelPatch::WriteOption::ALL;
  auto screen = thea::display::Screen::OPEDIT;

  /* Controllers 20-29 map to OP1 parameters */
  if (control >= 20 && control <= 29) {
    option = thea::ym2612::ChannelPatch::WriteOption(control - 20);
  } else {
    Serial.printf("Unmapped controller %i.\n", control);
    return; // Revisit
  }

  thea::synth::modify_patch_parameter(option, value);
  thea::synth::update_patch(option);

  thea::display::display_state.write_option = option;
  thea::display::show(screen, 10 * 100000);
}

// -----------------------------------------------------------------------------

void button_press_callback(int button) {
  Serial.printf("Press: %i\n", button);

  switch (button) {
  case 0:
    handleProgramChange(1, (patch_no + 1) % 127);
    break;
  case 2:
    handleProgramChange(1, (patch_no - 1) % 127);
    break;
  default:
    break;
  }
}

void button_release_callback(int button) { Serial.printf("Release: %i\n", button); }

void setup() {
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleProgramChange(handleProgramChange);
  MIDI.setHandleControlChange(handleControlChange);
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Write our patch up to the display, so it can show edits.
  thea::display::display_state.patch = &thea::synth::patch;

  // Load the first patch.
  handleProgramChange(1, 0);

  // Wire up button callbacks
  thea::buttons::on_button_press(&button_press_callback);
  thea::buttons::on_button_release(&button_release_callback);
}

void loop() {
  while (MIDI.read()) {
  };
}

} // namespace midi_interface
} // namespace thea

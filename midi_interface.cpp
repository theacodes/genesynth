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
  auto &patch = thea::synth::patch;

  switch (control) {
  case 20:
    patch.operators[0].DT1 = map(value, 0, 127, 0, 7);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_DT1;
    break;
  case 21:
    patch.operators[0].MUL = map(value, 0, 127, 0, 15);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_MUL;
    break;
  case 22:
    patch.operators[0].TL = value;
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_TL;
    screen = thea::display::Screen::ENVEDIT;
    break;
  case 23:
    patch.operators[0].AR = map(value, 0, 127, 0, 31);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_AR;
    screen = thea::display::Screen::ENVEDIT;
    break;
  case 24:
    patch.operators[0].D1R = map(value, 0, 127, 0, 31);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_D1R;
    screen = thea::display::Screen::ENVEDIT;
    break;
  case 25:
    patch.operators[0].D2R = map(value, 0, 127, 0, 31);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_D2R;
    screen = thea::display::Screen::ENVEDIT;
    break;
  case 26:
    patch.operators[0].D1L = map(value, 0, 127, 0, 31);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_D1L;
    screen = thea::display::Screen::ENVEDIT;
    break;
  case 27:
    patch.operators[0].RR = map(value, 0, 127, 0, 15);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_RR;
    screen = thea::display::Screen::ENVEDIT;
    break;
  case 28:
    patch.operators[0].RS = map(value, 0, 127, 0, 3);
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_RS;
    break;
  case 29:
    patch.operators[0].AM = value;
    option = thea::ym2612::ChannelPatch::WriteOption::OP0_AM;
    break;

  default:
    Serial.printf("Unmapped controller %i.\n", control);
    return; // Revisit
    break;
  }

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

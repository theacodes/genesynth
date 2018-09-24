#include <Arduino.h>
#include <MIDI.h>
#include <midi_UsbTransport.h>
#include "midi_interface.h"
#include "psg.h"
#include "ym2612.h"
#include "display.h"
#include "patch_loader.h"

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;
UsbTransport sUsbTransport;
MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);

namespace thea {
namespace midi_interface {

thea::ym2612::ChannelPatch patch;
byte ym_channel_note[6] = {0, 0, 0, 0, 0, 0};
byte psg_channel_note[3] = {0, 0, 0};


void ymNoteOn(float pitch, byte note, byte velocity) {
  size_t i = 0;
  for(;i < sizeof(ym_channel_note); i++) {
    if(ym_channel_note[i] == 0) {
      thea::ym2612::set_channel_freq(i, pitch);
      ym_channel_note[i] = note;
      thea::display::display_state.fm_channels[i] = true;
      break;
    }
  }
}

void ymNoteOff(byte note, byte velocity) {
  for(size_t i = 0; i < sizeof(ym_channel_note); i++) {
    if(ym_channel_note[i] == note) {
      int port = i < 3 ? 0 : 1;
      uint8_t channel_offset = (i % 3);
      uint8_t key_offset = channel_offset | (port << 2);
      thea::ym2612::set_reg(0x28, key_offset); // Key off
      ym_channel_note[i] = 0;
      thea::display::display_state.fm_channels[i] = false;
    }
  }
}

void psgNoteOn(float pitch, byte note, byte velocity) {
  int i = 0;
  for(; i < 3; i++) {
    if(psg_channel_note[i] == 0) {
      thea::psg::set_channel_freq(i, pitch);
      thea::psg::set_channel_vol(i, 255);
      psg_channel_note[i] = note;
      thea::display::display_state.sq_channels[i] = true;
      break;
    }
  }
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  float pitch = pow(2, float(note - 69) / 12) * 440;

  if(channel == 1) {
    ymNoteOn(pitch, note, velocity);
  }
  else if (channel == 2) {
    psgNoteOn(pitch, note, velocity);
  }
}

void psgNoteOff(byte note, byte velocity) {
  for(int i = 0; i < 3; i++) {
    if(psg_channel_note[i] == note) {
      thea::psg::set_channel_vol(i, 0);
      psg_channel_note[i] = 0;
      thea::display::display_state.sq_channels[i] = false;
    }
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if(channel == 1) {
    ymNoteOff(note, velocity);
  }
  else if (channel == 2) {
    psgNoteOff(note, velocity);
  }

  thea::display::display.setCursor(0, 3);
  thea::display::display.print("Note off!");
}


void handleProgramChange(byte channel, byte program) {
  if(channel != 1) return;

  thea::patch_loader::load_nth(program, &patch);

  for(int i = 0; i < 6; i++) {
    patch.write_to_channel(i);
  }

  thea::display::display.setCursor(0, 0);
  thea::display::display.print("Loaded patch!");
}

void handleControlChange(byte channel, byte control, byte value) {
  if(channel != 1) return;

  //Serial.printf("Got control change %i: %i\n", control, value);

  thea::ym2612::ChannelPatch::WriteOption option = thea::ym2612::ChannelPatch::WriteOption::ALL;

  switch(control) {
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
      break;
    case 23:
      patch.operators[0].AR = map(value, 0, 127, 0, 31);
      option = thea::ym2612::ChannelPatch::WriteOption::OP0_AR;
      break;
    case 24:
      patch.operators[0].D1R = map(value, 0, 127, 0, 31);
      option = thea::ym2612::ChannelPatch::WriteOption::OP0_D1R;
      break;
    case 25:
      patch.operators[0].D2R = map(value, 0, 127, 0, 31);
      option = thea::ym2612::ChannelPatch::WriteOption::OP0_D2R;
      break;
    case 26:
      patch.operators[0].D1L = map(value, 0, 127, 0, 31);
      option = thea::ym2612::ChannelPatch::WriteOption::OP0_D1L;
      break;
    case 27:
      patch.operators[0].RR = map(value, 0, 127, 0, 15);
      option = thea::ym2612::ChannelPatch::WriteOption::OP0_RR;
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
      Serial.printf("Unmapped controller.\n");
      return; // Revisit
      break;
  }

  for(int i = 0; i < 6; i++) {
    patch.write_to_channel(i, option);
  }

  thea::display::display_state.write_option = option;
  thea::display::show(thea::display::Screen::OPEDIT, 10 * 100000);
}

// -----------------------------------------------------------------------------

void setup()
{
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleProgramChange(handleProgramChange);
    MIDI.setHandleControlChange(handleControlChange);
    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);

    // Write our patch up to the display, so it can show edits.
    thea::display::display_state.patch = &patch;
}

void loop()
{
  while(MIDI.read()) {};
}

} // namespace midi_interface
} // namespace thea

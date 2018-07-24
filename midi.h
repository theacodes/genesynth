#ifndef THEA_MIDI_H
#define THEA_MIDI_H

#include <MIDI.h>
#include <midi_UsbTransport.h>
#include "psg.h"
#include "ym2612.h"
#include "display.h"
#include "patch_loader.h"

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;

UsbTransport sUsbTransport;

MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);


byte ym_channel_note[6] = {0, 0, 0, 0, 0, 0};
byte psg_channel_note[3] = {0, 0, 0};


void ymNoteOn(float pitch, byte note, byte velocity) {
  size_t i = 0;
  for(;i < sizeof(ym_channel_note); i++) {
    if(ym_channel_note[i] == 0) {
      thea::ym2612::set_channel_freq(i, pitch);
      ym_channel_note[i] = note;
      break;
    }
  }

  thea::display::display.clear();
  thea::display::display.setCursor(0, 0);
  thea::display::display.printf(
    "Note on!\nNote: %i\nChannel: %i", note, i);
}

void ymNoteOff(byte note, byte velocity) {
  for(size_t i = 0; i < sizeof(ym_channel_note); i++) {
    if(ym_channel_note[i] == note) {
      int port = i < 3 ? 0 : 1;
      uint8_t channel_offset = (i % 3);
      uint8_t key_offset = channel_offset | (port << 2);
      ym_channel_note[i] = 0;
      thea::ym2612::set_reg(0x28, key_offset); // Key off
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
      psg_channel_note[i] = 0;
      thea::psg::set_channel_vol(i, 0);
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

  thea::ym2612::ChannelPatch patch;
  thea::patch_loader::load_nth(program, &patch);

  for(int i = 0; i < 6; i++) {
    patch.write_to_channel(i);
  }

  thea::display::display.setCursor(0, 0);
  thea::display::display.print("Loaded patch!");
}

// -----------------------------------------------------------------------------

void midi_setup()
{
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleProgramChange(handleProgramChange);
    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void midi_loop()
{
    MIDI.read();
}

#endif

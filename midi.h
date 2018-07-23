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



// TEMPORARY
byte ym_channel_note[6] = {0, 0, 0, 0, 0, 0};
byte psg_channel_note[3] = {0, 0, 0};


void ymNoteOn(float pitch, byte note, byte velocity) {
  // From YM2612 Datasheet:
  // Freq Number = (144 * note * 2^20 / Clock) / 2^(block-1)
  float freq = pitch * 144 * pow(2.f, 20) / 7670000;
  int block = 4;
  uint32_t freq_int = freq / pow(2.f, block-1);

  // // find the correct block
  // int block;
  // for(block = 0; block > 1; --block){
  //   if(freq & (1<<17)) break;
  //   freq <<= 1;
  // }
  // --block;
  // freq >>= 7;

  size_t i = 0;
  for(; i < sizeof(ym_channel_note); i++) {
    if(ym_channel_note[i] == 0) {
      int port = i < 3 ? 0 : 1;
      uint8_t channel_offset = (i % 3);
      uint8_t key_offset = channel_offset | (port << 2);
      thea::ym2612::set_reg(0x28, key_offset); // Key off
      delay(3);
      thea::ym2612::set_reg(0xA4 + channel_offset, (block << 3) | (freq_int >> 8), port); // freq
      delay(3);
      thea::ym2612::set_reg(0xA0 + channel_offset, freq_int & 0xFF, port);
      delay(3);
      thea::ym2612::set_reg(0x28, 0xF0 | key_offset); // Key on
      ym_channel_note[i] = note;
      Serial.printf("Sending note %i to channel %i, offset %i, key %i\n", note, i, channel_offset, key_offset);
      break;
    }
  }

  thea::display::display.setCursor(0, 5);
  thea::display::display.print(freq);
  thea::display::display.setCursor(0, 6);
  thea::display::display.print(freq_int);
  thea::display::display.setCursor(0, 7);
  thea::display::display.print(i);
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
  thea::display::display.print("Note on!");
  float pitch = pow(2, float(note - 69) / 12) * 440;

  if(channel == 1) {
    ymNoteOn(pitch, note, velocity);
  }
  else if (channel == 2) {
    psgNoteOn(pitch, note, velocity);
  }

  thea::display::display.clear();
  thea::display::display.setCursor(0, 3);
  thea::display::display.print("Note on!");
  thea::display::display.setCursor(0, 4);
  thea::display::display.print(pitch);
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

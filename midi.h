#ifndef THEA_MIDI_H
#define THEA_MIDI_H

#include <MIDI.h>
#include <midi_UsbTransport.h>
#include "psg.h"
#include "ym2612.h"
#include "display.h"

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;

UsbTransport sUsbTransport;

MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);



// TEMPORARY
byte ym_channel_note[3] = {0, 0, 0};
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

  int i = 0;
  for(; i < 3; i++) {
    if(ym_channel_note[i] == 0) {
      thea::ym2612::set_reg(0x28, i); // Key off
      delay(1);
      thea::ym2612::set_reg(0xA4 + i, (block << 3) | (freq_int >> 8));
      delay(1);
      thea::ym2612::set_reg(0xA0 + i, freq_int & 0xFF);
      delay(1);
      thea::ym2612::set_reg(0x28, 0xF0+i); // Key on
      ym_channel_note[i] = note;
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

void ymNoteOff(byte note, byte velocity) {
  for(int i = 0; i < 3; i++) {
    if(ym_channel_note[i] == note) {
      ym_channel_note[i] = 0;
      thea::ym2612::set_reg(0x28, i); // Key off
    }
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

// -----------------------------------------------------------------------------

void midi_setup()
{
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void midi_loop()
{
    MIDI.read();
}

#endif
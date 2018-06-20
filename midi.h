#pragma once
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
byte midi_channel_note[3] = {0, 0, 0};


void handleNoteOn(byte channel, byte note, byte velocity) {
  display.print("Note on!");
  float pitch = pow(2, float(note - 69) / 12) * 440;

  //psg_set_channel_freq(channel-1, pitch);
  //psg_set_channel_vol(channel-1, 255);

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

  // ym_set_reg(0xA4, (freq >> 8) | (block << 3));
  // delay(1);
  // ym_set_reg(0xA0, (freq & 0xFF));
  // delay(1);

  ym_set_reg(0xA4, (block << 3) | (freq_int >> 8));
  delay(1);
  ym_set_reg(0xA0, freq_int & 0xFF);
  delay(1);

  ym_set_reg(0x28, 0x00); // Key off
  delay(1);
  ym_set_reg(0x28, 0xF0); // Key on
  midi_channel_note[channel-1] = note;

  display.clear();
  display.setCursor(0, 3);
  display.print("Note on!");
  display.setCursor(0, 4);
  display.print(pitch);
  display.setCursor(0, 5);
  display.print(freq);
  display.setCursor(0, 6);
  display.print(freq_int);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if(midi_channel_note[channel-1] != note) return;

  display.setCursor(0, 3);
  display.print("Note off!");
  ym_set_reg(0x28, 0x00); // Key off
  //psg_set_channel_vol(channel-1, 0);
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

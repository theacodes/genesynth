#pragma once
#include <MIDI.h>
#include <midi_UsbTransport.h>
#include "psg.h"
#include "display.h"

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;

UsbTransport sUsbTransport;

MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);



// TEMPORARY
byte midi_channel_note[3] = {0, 0, 0};


void handleNoteOn(byte channel, byte note, byte velocity) {
  display.print("Note on!");
  //                       Input clock (Hz) (3579545)
  // Frequency (Hz) = ----------------------------------
  //                   2 x register value x divider (16)
  // adjust the octive down one
  float pitch = pow(2, float(note - 69) / 12) * 440;

  display.setCursor(0, 3);
  display.print("Note on!");

  psg_set_channel_freq(channel-1, pitch);
  psg_set_channel_vol(channel-1, 255);
  midi_channel_note[channel-1] = note;
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if(midi_channel_note[channel-1] != note) return;

  display.setCursor(0, 3);
  display.print("Note off!");
  psg_set_channel_vol(channel-1, 0);
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

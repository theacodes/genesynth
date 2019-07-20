#include "USBMidi.h"
#include "simulator.h"

USBMidi usbMIDI;

int get_type_from_status(uint8_t status) { return status & 0xF0; }

int get_channel_from_status(uint8_t status) { return status & 0X0F; }

bool USBMidi::read() {
  std::vector<uint8_t> message;
  simulator::get_midi_message(&message);
  int len = message.size();

  if (len == 0)
    return false;

  uint8_t type = get_type_from_status(message[0]);
  uint8_t channel = get_channel_from_status(message[0]) + 1;

  switch (type) {
  case 0x80:
    note_off_handler(channel, message[1], message[2]);
    break;

  case 0x90:
    // MidiKeys will send NoteOn with velocity = 0 for NoteOff.
    if (message[2] == 0) {
      note_off_handler(channel, message[1], message[2]);
    } else {
      note_on_handler(channel, message[1], message[2]);
    }
    break;

  case 0xB0:
    // CC
    control_change_handler(channel, message[1], message[2]);
    break;

  default:
    printf("Unknown MIDI message %02x\n", type);
    for (auto i = 0; i < len; i++) {
      printf("%02x ", message[i]);
    }
    printf("\n");
    break;
  }

  return true;
}

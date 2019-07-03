#ifndef _USB_MIDI_H
#define _USB_MIDI_H

#include <stdint.h>


typedef void (*note_on_handler)(uint8_t, uint8_t, uint8_t);
typedef void (*note_off_handler)(uint8_t, uint8_t, uint8_t);
typedef void (*pitch_change_handler)(uint8_t, int);
typedef void (*control_change_handler)(uint8_t, uint8_t, uint8_t);
typedef void (*sysex_handler)(uint8_t*, unsigned int);


class USBMidi {
public:
    USBMidi() {};

    void setHandleNoteOn(note_on_handler) {};
    void setHandleNoteOff(note_off_handler) {};
    void setHandlePitchChange(pitch_change_handler) {};
    void setHandleControlChange(control_change_handler) {};
    void setHandleSystemExclusive(sysex_handler) {};
    void begin() {};
    bool read() { return false; }
};


extern USBMidi usbMIDI;

#endif

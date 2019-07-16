#ifndef THEA_MIDI_H
#define THEA_MIDI_H

#include <stdint.h>

namespace thea {
namespace midi_interface {

uint8_t get_last_cc();
void setup();
void loop();

} // namespace midi_interface
} // namespace thea

#endif

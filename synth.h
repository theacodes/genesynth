#ifndef THEA_SYNTH_H
#define THEA_SYNTH_H

#include "ym2612.h"

/*
Synthesizer core. This handles all of the "management" of the synth engine -
polyphony, triggering notes, patches, etc.

This exposes interfaces used by the MIDI interface and display to interact
with the synth core.
*/

namespace thea {
namespace synth {

extern thea::ym2612::ChannelPatch patch;

void init();
void loop();

void play_note(uint8_t note, float pitch);
void stop_note(uint8_t note);
void update_patch(thea::ym2612::ChannelPatch::WriteOption option);
inline void update_patch() { update_patch(thea::ym2612::ChannelPatch::WriteOption::ALL); }

} // namespace synth
} // namespace thea

#endif

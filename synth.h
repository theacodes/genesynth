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
extern thea::ym2612::ChannelPatch::WriteOption last_write_option;
extern unsigned long last_patch_modify_time;

void init();
void loop();

void play_note(uint8_t note, float pitch);
void stop_note(uint8_t note);
/* Offset is between -1.0 and 1.0. */
void pitch_bend(float offset);

/* Normalized value is between 0-127 */
void modify_patch_parameter(thea::ym2612::ChannelPatch::WriteOption option, uint8_t normalized_value);
void update_patch(thea::ym2612::ChannelPatch::WriteOption option);
inline void update_patch() { update_patch(thea::ym2612::ChannelPatch::WriteOption::ALL); }

} // namespace synth
} // namespace thea

#endif

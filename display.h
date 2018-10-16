#ifndef THEA_DISPLAY_H
#define THEA_DISPLAY_H

#include <U8x8lib.h>
#include "ym2612.h"

namespace thea {
namespace display {

struct DisplayState {
    bool fm_channels[6] = {false, false, false, false, false};
    bool sq_channels[3] = {false, false, false};
    char patch_name[20];
    thea::ym2612::ChannelPatch* patch;
    thea::ym2612::ChannelPatch::WriteOption write_option;
};

enum Screen {
    THEA,
    NOTES,
    OPEDIT,
    ENVEDIT,
};

extern DisplayState display_state;

void init();
void loop();
void show(Screen screen, unsigned long duration);

} // namespace thea
} // namespace display

#endif

#ifndef THEA_DISPLAY_H
#define THEA_DISPLAY_H

#include <U8x8lib.h>
#include "ym2612.h"

namespace thea {
namespace display {

extern U8X8_SH1106_128X64_NONAME_4W_HW_SPI display;


struct DisplayState {
    bool fm_channels[6] = {false, false, false, false, false};
    bool sq_channels[3] = {false, false, false};
    thea::ym2612::ChannelPatch* patch;
    thea::ym2612::ChannelPatch::WriteOption write_option;
};

enum Screen {
    NOTES,
    OPEDIT,
};

extern DisplayState display_state;

void init();
void loop();
void show(Screen screen, unsigned long duration);

} // namespace thea
} // namespace display

#endif

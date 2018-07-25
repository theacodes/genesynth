#ifndef THEA_DISPLAY_H
#define THEA_DISPLAY_H

#include <U8x8lib.h>

namespace thea {
namespace display {

extern U8X8_SH1106_128X64_NONAME_4W_HW_SPI display;


struct DisplayState {
    bool fm_channels[6] = {false, false, false, false, false};
    bool sq_channels[3] = {false, false, false};
};

extern DisplayState display_state;

void init();
void loop();

} // namespace thea
} // namespace display

#endif

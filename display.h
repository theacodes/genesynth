#ifndef THEA_DISPLAY_H
#define THEA_DISPLAY_H

#include <U8x8lib.h>

namespace thea {
namespace display {

extern U8X8_SH1106_128X64_NONAME_4W_HW_SPI display;

void init(void);

} // namespace thea
} // namespace display

#endif
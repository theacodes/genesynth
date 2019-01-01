#ifndef THEA_AMBIENT_UI_H
#define THEA_AMBIENT_UI_H

#include "ym2612.h"
#include <U8g2lib.h>

namespace thea {
namespace ambient_ui {

void display(U8G2 &, thea::ym2612::ChannelPatch &, thea::ym2612::ChannelPatch::WriteOption,
             unsigned long last_patch_modify_time);

} // namespace ambient_ui
} // namspace thea

#endif

#ifndef THEA_OPERATOR_GFX_H
#define THEA_OPERATOR_GFX_H

#include "ym2612.h"
#include <U8g2lib.h>

namespace thea {
namespace operator_gfx {

void draw_parameter_edit_screen(U8G2 &, const thea::ym2612::ChannelPatch &, thea::ym2612::ChannelPatch::WriteOption);
void draw_envelope_edit_screen(U8G2 &, const thea::ym2612::ChannelPatch &, thea::ym2612::ChannelPatch::WriteOption);

} // namespace operator_gfx
} // namespace thea

#endif

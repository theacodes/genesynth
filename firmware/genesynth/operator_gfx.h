#ifndef THEA_OPERATOR_GFX_H
#define THEA_OPERATOR_GFX_H

#include <U8g2lib.h>

#include "ym2612.h"

namespace thea {
namespace operator_gfx {

void draw_parameter_edit_screen(U8G2 &u8g2, const thea::ym2612::ChannelPatch &patch,
                                thea::ym2612::ChannelPatch::WriteOption write_option);
void draw_envelope_edit_screen(U8G2 &u8g2, const thea::ym2612::ChannelPatch &patch,
                               thea::ym2612::ChannelPatch::WriteOption write_option);

} // namespace operator_gfx
} // namespace thea

#endif

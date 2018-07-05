#ifndef THEA_PATCH_LOADER_H
#define THEA_PATCH_LOADER_H

#include "ym2612.h"

namespace thea {
namespace patch_loader {

void init();

void load_prev();
bool load_next(thea::ym2612::ChannelPatch* patch);

} // namespace opn
} // namespace thea

#endif
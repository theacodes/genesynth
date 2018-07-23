#ifndef THEA_PATCH_LOADER_H
#define THEA_PATCH_LOADER_H

#include "ym2612.h"

namespace thea {
namespace patch_loader {

void init();

bool load_nth(int n, thea::ym2612::ChannelPatch* patch);

} // namespace patch_loader
} // namespace thea

#endif

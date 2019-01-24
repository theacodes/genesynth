#ifndef THEA_FILESYSTEM_H
#define THEA_FILESYSTEM_H

#include <SdFat.h>

namespace thea {
namespace filesystem {

void init();
SdFatSdio &sd();

} // namespace filesystem
} // namespace thea

#endif

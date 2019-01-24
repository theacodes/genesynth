#include "filesystem.h"

namespace thea {
namespace filesystem {

SdFatSdio sdfs;

void init() { sdfs.begin(); }

SdFatSdio &sd() { return sdfs; }

} // namespace filesystem
} // namespace thea

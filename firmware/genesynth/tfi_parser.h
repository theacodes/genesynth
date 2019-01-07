#ifndef THEA_TFI_PARSER_H
#define THEA_TFI_PARSER_H

#include <SdFat.h>
#include <fstream>

#include "ym2612.h"

/* TFI Patch parser

See http://vgmrips.net/wiki/TFI_File_Format
*/

namespace thea {
namespace tfi {

bool parse(SdFile &file, thea::ym2612::ChannelPatch *patch);

} // namespace tfi
} // namespace thea

#endif

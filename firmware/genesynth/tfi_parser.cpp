#include <Arduino.h>

#include "tfi_parser.h"
#include "ym2612.h"

namespace thea {
namespace tfi {

bool parse(SdFile &file, thea::ym2612::ChannelPatch *patch) {
  if (!file.isOpen()) {
    Serial.println("Bad file, not open.");
    return false;
  }

  char rawdata[42];

  file.read(rawdata, 42);

  patch->algorithm = rawdata[0];
  patch->feedback = rawdata[1];

  for (int i = 0; i < 4; i++) {
    size_t offset = 2 + (10 * i);
    patch->operators[i].MUL = rawdata[offset + 0];
    patch->operators[i].DT1 = rawdata[offset + 1];
    patch->operators[i].TL = rawdata[offset + 2];
    patch->operators[i].RS = rawdata[offset + 3];
    patch->operators[i].AR = rawdata[offset + 4];
    patch->operators[i].D1R = rawdata[offset + 5];
    patch->operators[i].D2R = rawdata[offset + 6];
    patch->operators[i].RR = rawdata[offset + 7];
    patch->operators[i].D1L = rawdata[offset + 8];
  }

  return true;
}

} // namespace tfi
} // namespace thea

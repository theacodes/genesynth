#include <Arduino.h>

#include "tfi_parser.h"
#include "ym2612.h"

namespace thea {
namespace tfi {

bool load(SdFile &file, SdFile *folder, thea::ym2612::ChannelPatch *patch) {
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

  char filename[MAX_PATCH_NAME_SIZE];

  if (folder != nullptr) {
    folder->getName(filename, MAX_PATCH_NAME_SIZE);
  } else {
    file.getName(filename, MAX_PATCH_NAME_SIZE);
  }
  strncpy(patch->bank, filename, 32);

  file.getName(filename, MAX_PATCH_NAME_SIZE);
  strncpy(patch->name, filename, 32);

  Serial.printf("Loaded patch %s from %s.\n", patch->name, patch->bank);

  return true;
}

void save(SdFile &file, thea::ym2612::ChannelPatch &patch) {
  if (!file.isOpen()) {
    Serial.println("Bad file, not open.");
    return;
  }

  uint8_t data[42] = {0};

  data[0] = patch.algorithm;
  data[1] = patch.feedback;

  for (int i = 0; i < 4; i++) {
    size_t offset = 2 + (10 * i);
    data[offset + 0] = patch.operators[i].MUL;
    data[offset + 1] = patch.operators[i].DT1;
    data[offset + 2] = patch.operators[i].TL;
    data[offset + 3] = patch.operators[i].RS;
    data[offset + 4] = patch.operators[i].AR;
    data[offset + 5] = patch.operators[i].D1R;
    data[offset + 6] = patch.operators[i].D2R;
    data[offset + 7] = patch.operators[i].RR;
    data[offset + 8] = patch.operators[i].D1L;
  }

  file.write(data, 42);
  file.sync();
  file.close();
}

} // namespace tfi
} // namespace thea

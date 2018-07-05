#include <arduino.h>
#include <cstring>
#include "SdFat.h"

#include "patch_loader.h"

namespace thea {
namespace patch_loader {

#define MAX_FILE_NAME_SIZE 256

SdFatSdio sd;
SdFile current_file;

void init() {
  sd.begin();
  sd.vwd()->rewind();
}

const char* get_extension(const char* filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot) return "\0\0\0";
    return dot + 1;
}

bool load_next(thea::ym2612::ChannelPatch* patch) {
  char filename[MAX_FILE_NAME_SIZE];

  current_file.getName(filename, MAX_FILE_NAME_SIZE);

  Serial.println(filename);

  while(!strncmp("opn", get_extension(filename), 3)) {
    bool success = current_file.openNext(sd.vwd(), O_READ);
    /* Todo: wrap-around */
    if(!success) return false;
  }

  return true;
};

} // namespace opn
} // namespace thea
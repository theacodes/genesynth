#include <arduino.h>
#include <cstring>
#include "SdFat.h"

#include "opn_parser.h"
#include "patch_loader.h"

namespace thea {
namespace patch_loader {

#define MAX_FILE_NAME_SIZE 256

SdFatSdio sd;
SdFile current_file;

void init() {
  if (!sd.begin()) {
    Serial.println("Patch loader failed to start SD.");
    return;
  }
  sd.vwd()->rewind();
}

const char* get_extension(const char* filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot) return "\0\0\0";
    return dot + 1;
}

bool load_next_file_with_extension(SdFile* file, const char* extension) {
  char filename[MAX_FILE_NAME_SIZE];

  do {
    if(file->isOpen()) file->close();
    bool success = file->openNext(sd.vwd(), O_READ);
    /* Todo: wrap-around */
    if(!success) return false;
    file->getName(filename, MAX_FILE_NAME_SIZE);
  }
  while(strncmp(extension, get_extension(filename), 3) != 0);

  return true;
}

bool load_next(thea::ym2612::ChannelPatch* patch) {
  if(!load_next_file_with_extension(&current_file, "opn")) {
    return false;
  }

  char filename[MAX_FILE_NAME_SIZE];
  current_file.getName(filename, MAX_FILE_NAME_SIZE);

  Serial.printf("Loading patch %s\n", filename);

  current_file.close(); // It will be re-opened as a stream.

  thea::opn::parse(filename, patch);

  // TODO: Move this out of here.
  patch->write_to_channel(0);
  patch->write_to_channel(1);
  patch->write_to_channel(2);

  return true;
};

} // namespace opn
} // namespace thea

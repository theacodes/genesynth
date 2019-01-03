#include "SdFat.h"
#include <arduino.h>
#include <cstring>

#include "patch_loader.h"
#include "tfi_parser.h"

namespace thea {
namespace patch_loader {

#define MAX_FILE_NAME_SIZE 256

SdFatSdio sd;
SdFile current_bank;
SdFile current_file;

const char *get_extension(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot)
    return "\0\0\0";
  return dot + 1;
}

bool load_next_file_with_extension(SdFile *dir, SdFile *file, const char *extension) {
  char filename[MAX_FILE_NAME_SIZE];

  do {
    if (file->isOpen())
      file->close();
    bool success = file->openNext(dir, O_READ);
    if (!success)
      dir->rewind(); // wrap around.
    // TODO: Fix degenerative case where there isn't any files
    file->getName(filename, MAX_FILE_NAME_SIZE);
  } while (strncmp(extension, get_extension(filename), 3) != 0);

  return true;
}

bool load_next_directory(SdFile *file) {
  char filename[MAX_FILE_NAME_SIZE];

  while (true) {
    if (file->isOpen())
      file->close();
    bool success = file->openNext(sd.vwd(), O_READ);

    if (!success) {
      sd.vwd()->rewind(); // wrap around.
      continue;
    }

    if (!file->isDir()) {
      continue;
    }

    file->getName(filename, MAX_FILE_NAME_SIZE);

    if (strcmp(filename, "System Volume Information") == 0) {
      continue;
    }

    break;

    // TODO: Fix degenerative case where there aren't any folders
  }

  return true;
}

bool load_nth_bank(int n) {
  sd.vwd()->rewind();
  for (; n >= 0; n--) {
    load_next_directory(&current_bank);
  }

  if (!current_bank.isOpen()) {
    Serial.println("Directory is not open.");
    return false;
  }

  char dirname[MAX_FILE_NAME_SIZE];
  current_bank.getName(dirname, MAX_FILE_NAME_SIZE);

  Serial.printf("Working with bank %s\n", dirname);
};

bool load_nth_program(int n, thea::ym2612::ChannelPatch *patch) {
  current_bank.rewind();
  for (; n >= 0; n--) {
    load_next_file_with_extension(&current_bank, &current_file, "tfi");
  }

  return load_from_sd_file(current_file, &current_bank, patch);
}

bool load_from_sd_file(SdFile &file, SdFile *folder, thea::ym2612::ChannelPatch *patch) {
  if (!file.isOpen()) {
    Serial.println("File is not open.");
    return false;
  }

  char filename[MAX_FILE_NAME_SIZE];

  if (folder != nullptr) {
    folder->getName(filename, MAX_FILE_NAME_SIZE);
  } else {
    file.getName(filename, MAX_FILE_NAME_SIZE);
  }
  strncpy(patch->bank, filename, 32);

  file.getName(filename, MAX_FILE_NAME_SIZE);
  strncpy(patch->name, filename, 32);

  thea::tfi::parse(file, patch);

  Serial.printf("Loaded patch %s from %s.\n", patch->name, patch->bank);

  return true;
}

void init() {
  if (!sd.begin()) {
    Serial.println("Patch loader failed to start SD.");
    return;
  }
  sd.vwd()->rewind();

  load_nth_bank(0);
}

} // namespace patch_loader
} // namespace thea

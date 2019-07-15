#include "SdFat.h"
#include <cstdio>
#include <string>
#include <sys/stat.h>

bool SdFile::openRoot(FatVolume *vol) {
  if ((directory_dirp = opendir(".")) == NULL) {
    printf("couldn't opendir '.'");
    return false;
  }
  if ((directory_direntp = readdir(directory_dirp)) == NULL) {
    printf("couldn't readdir '.'");
    return false;
  }

  sprintf(fullpath, ".");
  return true;
}

bool SdFile::isDir() {
  struct stat statbuf;
  if (stat(fullpath, &statbuf) != 0)
    return false;
  return S_ISDIR(statbuf.st_mode);
}

void SdFile::rewind() {
  if (directory_dirp == NULL)
    return;
  rewinddir(directory_dirp);
  directory_direntp = readdir(directory_dirp);
  parent_dir_index = 0;
}

bool SdFile::openNext(SdFile *dir, int flag) {
  if (dir->directory_dirp == NULL)
    dir->openRoot(nullptr);

  dir->directory_direntp = readdir(dir->directory_dirp);
  file_direntp = dir->directory_direntp;

  if (file_direntp == NULL) {
    return false;
  }

  dir_index = dir->parent_dir_index++;

  sprintf(fullpath, "%s/%s", dir->fullpath, file_direntp->d_name);

  directory_dirp = opendir(fullpath);

  return true;
}

bool SdFile::getName(char *dst, int len) {
  if (!file_direntp)
    return false;
  strncpy(dst, file_direntp->d_name, len);
  return true;
}

int SdFile::read(void *buf, size_t nbyte) {
  unsigned char *bytebuf = (unsigned char *)buf;
  FILE *fp = fopen(fullpath, "r");

  if (fp == NULL) {
    printf("Couldn't open file %s.\n", fullpath);
    return -1;
  }

  int i = 0;
  do {
    bytebuf[i] = fgetc(fp);

    if (feof(fp)) {
      break;
    }

    i++;
  } while (i < nbyte);

  fclose(fp);

  return i;
};

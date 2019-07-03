#ifndef _SDFAT_H
#define _SDFAT_H

#include <cstddef>
#include <stdint.h>
#include <dirent.h>

enum {
    O_READ,
    O_WRITE,
    O_TRUNC,
    O_CREAT,
};

class FatVolume {};

class SdFatSdio {
public:
    SdFatSdio() {};
    FatVolume* vol() { return nullptr; };
    bool begin() { return true; };
};

class SdFile {
public:
    bool mkdir(SdFile* dir, const char* name) { return true; }
    bool open(SdFile* dir, const char* name, int mode) { return true; }
    bool openRoot(FatVolume*);
    bool openNext(SdFile* dir, int flag);
    bool isOpen() { return true; };
    bool isDir() { return true; };
    void close() {};
    void rewind();
    bool getName(char*, int);
    int dirIndex() { return dir_index; };
    int read(void *buf, size_t nbyte);
    size_t write(uint8_t *buf, size_t nbyte) { return 0; };
    bool sync() { return true; };

private:
    char fullpath[1024];
    int parent_dir_index = 0;
    int dir_index = 0;
    DIR *file_dirp = nullptr;
    struct dirent *file_direntp = nullptr;
    DIR *directory_dirp = nullptr;
    struct dirent *directory_direntp = nullptr;
};


#endif

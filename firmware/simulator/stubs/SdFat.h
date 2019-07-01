#ifndef _SDFAT_H
#define _SDFAT_H

#include <cstddef>
#include <stdint.h>

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
    bool openRoot(FatVolume*) { return true; };
    int openNext(SdFile* dir, int) { return 0; };
    bool isOpen() { return true; };
    bool isDir() { return true; };
    void close() {};
    void rewind() {};
    void getName(char*, int) {};
    int dirIndex() { return 0; };
    int read(void *buf, size_t nbyte) { return nbyte; };
    size_t write(uint8_t *buf, size_t nbyte) { return 0; };
    bool sync() { return true; };
};


#endif

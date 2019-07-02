#ifndef _u8g2_sdl_h
#define _u8g2_sdl_h

#include "U8g2lib.h"

class U8G2_SDL : public U8G2 {
public:
    U8G2_SDL(const u8g2_cb_t *rotation);
};

#endif

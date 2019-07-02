#ifndef _SIMULATOR_CONTEXT_H
#define _SIMULATOR_CONTEXT_H

#include <SDL2/SDL.h>

namespace thea {
namespace simulator_context
{
    struct Context {
        SDL_Renderer* renderer;
    };

    extern Context context;
}
}

#endif

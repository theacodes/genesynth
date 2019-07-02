#include <SDL2/SDL.h>
#include "Arduino.h"

unsigned long millis() {
    return SDL_GetTicks();
}

unsigned long micros() {
    return SDL_GetTicks() * 1000;
}

SerialStub Serial;

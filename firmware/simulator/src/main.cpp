#include <cstdio>
#include <SDL2/SDL.h>

#include "simulator_context.h"
#include "Arduino.h"

#include "genesynth.ino"

#define RENDER_SCALE 4
#define SCREEN_W 128
#define SCREEN_H 64

typedef void (*loop_func)(void);

void sdl_event_loop(SDL_Renderer* renderer, loop_func loop) {
    // Target used for rendering the u8g2 stuff.
    SDL_Texture *u8g2Target = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_W,
        SCREEN_H);

    // Where the u8g2 screen will be rendered.
    SDL_Rect u8g2DestRect;
    u8g2DestRect.x = 0;
    u8g2DestRect.y = 0;
    u8g2DestRect.w = SCREEN_W * RENDER_SCALE;
    u8g2DestRect.h = SCREEN_H * RENDER_SCALE;

    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_UP:
                            thea::simulator_context::context.up = true;
                            break;
                        case SDLK_RIGHT:
                            thea::simulator_context::context.right = true;
                            break;
                        case SDLK_DOWN:
                            thea::simulator_context::context.down = true;
                            break;
                        case SDLK_LEFT:
                            thea::simulator_context::context.left = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(e.key.keysym.sym) {
                        case SDLK_UP:
                            thea::simulator_context::context.up = false;
                            break;
                        case SDLK_RIGHT:
                            thea::simulator_context::context.right = false;
                            break;
                        case SDLK_DOWN:
                            thea::simulator_context::context.down = false;
                            break;
                        case SDLK_LEFT:
                            thea::simulator_context::context.left = false;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        // Render the loop into the u8g2 texture.
        SDL_SetRenderTarget(renderer, u8g2Target);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Run the Arduino loop
        loop();

        // TODO: Render UI for buttons.
        SDL_SetRenderTarget(renderer, NULL);

        SDL_SetRenderDrawColor(renderer, 200, 30, 140, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, u8g2Target, NULL, &u8g2DestRect);
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char const *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Genesynth Simulator", 100, 100, SCREEN_W * RENDER_SCALE, SCREEN_H * RENDER_SCALE, SDL_WINDOW_SHOWN);
    if (win == nullptr){
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (ren == nullptr){
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    thea::simulator_context::context.renderer = ren;

    printf("Done with SDL setup.\n");

    printf("Running setup.\n");
    setup();

    printf("Starting loop.\n");
    sdl_event_loop(ren, &loop);

    return 0;
}

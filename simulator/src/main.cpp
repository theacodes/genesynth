#include <cstdio>
#include <SDL2/SDL.h>

#include "ym2612core/ym2612.h"
#include "simulator_context.h"
#include "Arduino.h"

#include "genesynth.ino"



void ym_write(unsigned int addr, unsigned int val) {
    YM2612Write(0, addr);
    YM2612Write(1, val);
}

void sound_test() {
    ym_write(0x22, 0x00); // LFO off
    ym_write(0x27, 0x00); // Note off (channel 0)
    ym_write(0x28, 0x01); // Note off (channel 1)
    ym_write(0x28, 0x02); // Note off (channel 2)
    ym_write(0x28, 0x04); // Note off (channel 3)
    ym_write(0x28, 0x05); // Note off (channel 4)
    ym_write(0x28, 0x06); // Note off (channel 5)
    ym_write(0x2B, 0x00); // DAC off
    ym_write(0x30, 0x71); //
    ym_write(0x34, 0x0D); //
    ym_write(0x38, 0x33); //
    ym_write(0x3C, 0x01); // DT1/MUL
    ym_write(0x40, 0x23); //
    ym_write(0x44, 0x2D); //
    ym_write(0x48, 0x26); //
    ym_write(0x4C, 0x00); // Total level
    ym_write(0x50, 0x5F); //
    ym_write(0x54, 0x99); //
    ym_write(0x58, 0x5F); //
    ym_write(0x5C, 0x94); // RS/AR
    ym_write(0x60, 0x05); //
    ym_write(0x64, 0x05); //
    ym_write(0x68, 0x05); //
    ym_write(0x6C, 0x07); // AM/D1R
    ym_write(0x70, 0x02); //
    ym_write(0x74, 0x02); //
    ym_write(0x78, 0x02); //
    ym_write(0x7C, 0x02); // D2R
    ym_write(0x80, 0x11); //
    ym_write(0x84, 0x11); //
    ym_write(0x88, 0x11); //
    ym_write(0x8C, 0xA6); // D1L/RR
    ym_write(0x90, 0x00); //
    ym_write(0x94, 0x00); //
    ym_write(0x98, 0x00); //
    ym_write(0x9C, 0x00); // Proprietary
    ym_write(0xB0, 0x32); // Feedback/algorithm
    ym_write(0xB4, 0xC0); // Both speakers on
    ym_write(0x28, 0x00); // Key off
    ym_write(0xA4, 0x22); //
    ym_write(0xA0, 0x69); // Set frequency
    ym_write(0x28, 0xF0);
}

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
                            ym_write(0x28, 0x00); // Key off
                            ym_write(0x28, 0xF0); // Key on
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

void audio_callback(void* userdata, uint8_t *stream, int len) {
    len /= 2; // 16 bits
    int16_t* stream_buf = (int16_t*)(stream);

    int ym_buf[len];

    YM2612Update(ym_buf, len / 2); // Update fills 2 * len samples.

    for(auto i = 0; i < len; i++) {
        stream_buf[i] = ym_buf[i];
    }
}

int main(int argc, char const *argv[]) {
    YM2612Init();
    YM2612ResetChip();
    YM2612Config(YM2612_DISCRETE);
    sound_test();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof(want));
    want.freq = 44100;
    want.format = AUDIO_S16;
    want.samples = 1024 * 2;
    want.callback = audio_callback;
    want.userdata = NULL;
    want.channels = 2;

    auto audiodev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if ( audiodev == 0 ) {
        printf("SDL_OpenAudio Error: %s\n", SDL_GetError());
        return 1;
    }
    printf("Initialized SDL Audio device: %i\n", audiodev);
    SDL_PauseAudioDevice(audiodev, 0);

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

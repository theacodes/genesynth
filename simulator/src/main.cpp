#include <cstdio>
#include <SDL2/SDL.h>
#include <RtMidi.h>

#include "ym2612core/ym2612.h"
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

        // Render the screen.
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

int get_and_open_midi_port(RtMidiIn* midiin) {
    unsigned int num_ports = midiin->getPortCount();
    printf("There are %i MIDI input sources available.\n", num_ports);
    std::string port_name;
    for ( unsigned int i=0; i<num_ports; i++ ) {
        try {
            printf(" #%i: %s\n", i, midiin->getPortName(i).c_str());
        }
        catch ( RtMidiError &error ) {
            error.printMessage();
            return -1;
        }
        
    }

    int port_num = 0;
    if(num_ports > 1) {
        std::cout << "Which port? " << std::flush;
        std::cin >> port_num;
    }

    midiin->openPort(port_num);

    return port_num;
}

int main(int argc, char const *argv[]) {
    YM2612Init();
    YM2612Config(YM2612_DISCRETE);
    YM2612ResetChip();

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

    // Use "Loopback Audio" as the first argument to do screen recording.
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

    printf("Setting up RtMidi.\n");

    RtMidiIn *midiin = nullptr;
    try {
        midiin = new RtMidiIn();
    } catch (RtMidiError &error) {
        // Handle the exception here
        error.printMessage();
        return 1;
    }

    get_and_open_midi_port(midiin);
    thea::simulator_context::context.midiin = midiin;


    printf("Running Arduino setup.\n");
    setup();

    printf("Starting Arduino loop.\n");
    sdl_event_loop(ren, &loop);

    return 0;
}

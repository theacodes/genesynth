#include "RtMidi.h"
#include "ym2612core/ym2612.h"
#include <SDL2/SDL.h>

#include "hardware_constants.h"

#include "simulator.h"

#define RENDER_SCALE 4
#define SCREEN_W 128
#define SCREEN_H 64

namespace simulator {

static SDL_Window *sdl_window = nullptr;
static SDL_Renderer *sdl_renderer = nullptr;
static SDL_AudioDeviceID sdl_audiodev = 0;
static std::vector<uint8_t> simulated_midi_message;
static RtMidiIn *rt_midiin = nullptr;
static bool button_up, button_right, button_down, button_left;

inline static bool sdl_error(const char *func_name) {
  printf("%s Error: %s\n", func_name, SDL_GetError());
  SDL_Quit();
  return false;
}

static bool initialize_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return false;
  }

  sdl_window = SDL_CreateWindow("Genesynth Simulator", 100, 100, SCREEN_W * RENDER_SCALE, SCREEN_H * RENDER_SCALE,
                                SDL_WINDOW_SHOWN);
  if (sdl_window == nullptr) {
    return sdl_error("SDL_CreateWindow");
  }

  sdl_renderer = SDL_CreateRenderer(sdl_window, -1,
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
  if (sdl_renderer == nullptr) {
    SDL_DestroyWindow(sdl_window);
    return sdl_error("SDL_CreateRenderer");
  }

  printf("SDL setup complete.\n");
  return true;
};

static bool initialize_sound_chip() {
  YM2612Init();
  YM2612Config(YM2612_DISCRETE);
  YM2612ResetChip();

  return true;
}

static void audio_callback(void *userdata, uint8_t *stream, int len) {
  len /= 2; // 16 bits
  int16_t *stream_buf = (int16_t *)(stream);

  int ym_buf[len];

  YM2612Update(ym_buf, len / 2); // Update fills 2 * len samples.

  for (auto i = 0; i < len; i++) {
    stream_buf[i] = ym_buf[i];
  }
}

static bool initialize_audio() {
  SDL_AudioSpec want, have;
  SDL_memset(&want, 0, sizeof(want));
  want.freq = 44100;
  want.format = AUDIO_S16;
  want.samples = 1024 * 2;
  want.callback = audio_callback;
  want.userdata = NULL;
  want.channels = 2;

  // Use "Loopback Audio" as the first argument to do screen recording.
  sdl_audiodev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (sdl_audiodev == 0) {
    sdl_error("SDL_OpenAudio Error");
    return 1;
  }
  printf("Initialized SDL Audio device: %i\n", sdl_audiodev);

  // Start playback.
  SDL_PauseAudioDevice(sdl_audiodev, 0);

  return true;
};

static bool initialize_midi() {
  try {
    rt_midiin = new RtMidiIn();
  } catch (RtMidiError &error) {
    // Handle the exception here
    error.printMessage();
    return false;
  }

  unsigned int num_ports = rt_midiin->getPortCount();
  int port_num = 0;

  if (num_ports != 1) {
    printf("There are %i MIDI input sources available.\n", num_ports);
    std::string port_name;
    for (unsigned int i = 0; i < num_ports; i++) {
      try {
        printf(" #%i: %s\n", i, rt_midiin->getPortName(i).c_str());
      } catch (RtMidiError &error) {
        error.printMessage();
        return false;
      }
    }
    printf("Which port?: ");
    std::cin >> port_num;
  }

  rt_midiin->openPort(port_num);

  return true;
};

bool initialize() { return (initialize_sdl() && initialize_sound_chip() && initialize_audio() && initialize_midi()); }

bool teardown() {
  if (rt_midiin != nullptr) {
    delete rt_midiin;
    rt_midiin = nullptr;
  }
  if (sdl_audiodev != 0) {
    SDL_CloseAudioDevice(sdl_audiodev);
    sdl_audiodev = 0;
  }
  if (sdl_renderer != nullptr) {
    SDL_DestroyRenderer(sdl_renderer);
    sdl_renderer = nullptr;
  }
  if (sdl_window != nullptr) {
    SDL_DestroyWindow(sdl_window);
    sdl_window = nullptr;
  }

  SDL_Quit();
  return true;
}

void set_pixel(int x, int y, bool color) {
  if (color) {
    SDL_SetRenderDrawColor(sdl_renderer, 200, 185, 255, SDL_ALPHA_OPAQUE);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  }

  SDL_RenderDrawPoint(sdl_renderer, x, y);
}

void get_midi_message(std::vector<uint8_t> *dst) {
  if (!simulated_midi_message.empty()) {
    *dst = simulated_midi_message;
    simulated_midi_message.clear();
    return;
  }
  rt_midiin->getMessage(dst);
}

bool get_button_state(int button) {
  switch (button) {
  case BUTTON_ONE:
    return button_up;
    break;
  case BUTTON_ONE - 1:
    return button_right;
    break;
  case BUTTON_ONE - 2:
    return button_down;
    break;
  case BUTTON_ONE - 3:
    return button_left;
    break;
  default:
    return false;
    break;
  }
}

static bool process_sdl_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      return false;
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      bool key_state;
      key_state = e.type == SDL_KEYDOWN ? true : false;
      switch (e.key.keysym.sym) {
      case SDLK_ESCAPE:
        return false;
        break;
      case SDLK_UP:
        button_up = key_state;
        break;
      case SDLK_RIGHT:
        button_right = key_state;
        break;
      case SDLK_DOWN:
        button_down = key_state;
        break;
      case SDLK_LEFT:
        button_left = key_state;
        break;
      case SDLK_1:
        if (key_state) {
          simulated_midi_message.push_back(0xB0);
          simulated_midi_message.push_back(0x01);
          simulated_midi_message.push_back(0x50);
        }
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return true;
}

void loop(loop_func_t loop_func) {
  // Target used for rendering the u8g2 stuff.
  SDL_Texture *u8g2Target =
      SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);

  // Where the u8g2 screen will be rendered.
  SDL_Rect u8g2DestRect;
  u8g2DestRect.x = 0;
  u8g2DestRect.y = 0;
  u8g2DestRect.w = SCREEN_W * RENDER_SCALE;
  u8g2DestRect.h = SCREEN_H * RENDER_SCALE;

  while (process_sdl_events()) {
    // Render the loop into the u8g2 texture.
    SDL_SetRenderTarget(sdl_renderer, u8g2Target);
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(sdl_renderer);

    // Run the Arduino loop
    loop_func();

    // Render the screen.
    SDL_SetRenderTarget(sdl_renderer, NULL);
    SDL_SetRenderDrawColor(sdl_renderer, 200, 30, 140, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, u8g2Target, NULL, &u8g2DestRect);
    SDL_RenderPresent(sdl_renderer);
  }
}

} // namespace simulator

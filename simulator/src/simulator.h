#pragma once

#include <cstdint>
#include <vector>

namespace simulator {


typedef void (*loop_func_t)(void);

bool initialize();
bool teardown();
void loop(loop_func_t);
bool get_button_state(int);
void set_pixel(int x, int y, bool color);
void get_midi_message(std::vector<uint8_t>* dst);

}

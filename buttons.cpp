#include "buttons.h"
#include <Arduino.h>
#include <Bounce2.h>

namespace thea {
namespace buttons {

#define BUTTON_BOUNCE_MS 25
// Buttons use descending pins from the first button's pin.
#define BUTTON_ONE 27

Bounce buttons[4];

inline void noop_callback(int button) {}

callback press_callback = &noop_callback;
callback release_callback = &noop_callback;

void on_button_press(callback cb) { press_callback = cb; }

void on_button_release(callback cb) { release_callback = cb; }

bool is_pressed(int button) { return !buttons[button].read(); }

void init() {
  for (int i = 0; i < 4; i++) {
    pinMode(BUTTON_ONE - i, INPUT_PULLUP);
    Bounce &button = buttons[i];
    button.attach(BUTTON_ONE - i);
    button.interval(BUTTON_BOUNCE_MS);
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    Bounce &button = buttons[i];
    button.update();

    if (button.fell()) {
      press_callback(i);
    } else if (button.rose()) {
      release_callback(i);
    }
  }
}

} // namespace thea
} // namespace buttons

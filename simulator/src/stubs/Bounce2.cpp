#include "Bounce2.h"

#include "hardware_constants.h"
#include "simulator_context.h"

using thea::simulator_context::context;

void Bounce::update() {
  last = state;
  switch (pin) {
  case BUTTON_ONE:
    state = context.up;
    break;
  case BUTTON_ONE - 1:
    state = context.right;
    break;
  case BUTTON_ONE - 2:
    state = context.down;
    break;
  case BUTTON_ONE - 3:
    state = context.left;
    break;
  default:
    break;
  }
}

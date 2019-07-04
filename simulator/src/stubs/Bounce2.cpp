#include "Bounce2.h"
#include "hardware_constants.h"
#include "simulator.h"

void Bounce::update() {
  last = state;
  state = simulator::get_button_state(pin);
}

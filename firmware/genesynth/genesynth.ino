#include "hardware_constants.h"
#include "midi_interface.h"
#include "src/theacommon/buttons.h"
#include "synth.h"
#include "ui.h"
#include "vgm.h"
#include "ym2612.h"

// the setup routine runs once when you press reset:
void setup() {
  // Initialize the display & buttons.
  // thea::display::init();
  thea::buttons::init();

  // Wait for serial monitoring if the down button is pressed on boot.
  Serial.begin(9600);

  // Loop a few times to let the buttons settle, and if the down button is
  // pressed, wait for the serial debug monitor.
  for (auto i = 0; i < 100; i++) {
    thea::buttons::loop();
  }
  bool wait_for_serial = thea::buttons::is_pressed(2);

  // Initialize the user interface
  thea::ui::init(wait_for_serial);

  Serial.println("Early initialization done.");

  // Setup sound chips.
  thea::ym2612::setup();
  thea::ym2612::reset();

  // Setup synth core.
  thea::synth::init();

  // Setup MIDI
  thea::midi_interface::setup();

  Serial.println("Sound chips and MIDI interface up.");
}

void loop() {
  thea::buttons::loop();
  thea::midi_interface::loop();
  thea::ui::loop();
}

#include <util/delay.h>

#include "buttons.h"
#include "display.h"
#include "midi_interface.h"
#include "patch_loader.h"
#include "psg.h"
#include "vgm.h"
#include "ym2612.h"

#define STATUS_LED 13
#define YM_CLOCK 4
#define PSG_CLOCK 5

static void wait_for_serial_monitor() {
  // Show on the screen that we're waiting for the monitor.
  thea::display::show(thea::display::Screen::DEBUG, 1);
  thea::display::loop();

  /* Waits for the serial monitor to be opened. */
  while (!Serial.dtr()) {
    delay(10);
  }
}

static void setup_ym_clock() {
  /* Uses PWM to generate an 7.67Mhz clock for the YM2612 */
  pinMode(YM_CLOCK, OUTPUT);
  analogWriteFrequency(YM_CLOCK, 7670000);
  analogWrite(YM_CLOCK, 128);
}

// the setup routine runs once when you press reset:
void setup() {
  // Initialize the display & buttons.
  thea::display::init();
  thea::buttons::init();

  // Wait for serial monitoring if the down button is pressed on boot.
  Serial.begin(9600);

  if (thea::buttons::is_pressed(2)) {
    wait_for_serial_monitor();
  }

  Serial.println("Started");

  // Setup clocks
  setup_ym_clock();
  delay(50); // wait a moment for the clock.

  // Setup sound chips.
  thea::ym2612::setup();
  thea::ym2612::reset();
  thea::psg::setup();
  thea::psg::reset();

  // Setup patch loading. The MIDI interface will load the
  // initial patch.
  thea::patch_loader::init();

  // Setup MIDI
  thea::midi_interface::setup();
}

void loop() {
  thea::buttons::loop();
  thea::midi_interface::loop();
  thea::display::loop();
}

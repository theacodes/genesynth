#include <util/delay.h>
#include "ym2612.h"
#include "song.h"
#include "ym_test.h"
#include "vgm.h"
#include "display.h"

#define STATUS_LED 13
#define YM_CLOCK 4

static void wait_for_serial_monitor() {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
  /* Waits for the serial monitor to be opened. */
  while (!Serial.dtr()) {
    delay(10);
    digitalWrite(STATUS_LED, HIGH);
    delay(10);
    digitalWrite(STATUS_LED, LOW);
  }
}

static void setup_ym_clock() {
  /* Temporary. Uses PWM to generate an 8Mhz clock for the YM2612 */
  pinMode(YM_CLOCK, OUTPUT);
  analogWriteFrequency(YM_CLOCK, 8000000);
  analogWrite(YM_CLOCK, 128);
}

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  //wait_for_serial_monitor();
  Serial.println("Started");

  // Initialize the display
  display_init();

  // Setup clocks
  setup_ym_clock();
  delay(50); // wait a second for the clock.

  // Setup sound chips.
  ym_setup();
  ym_reset();

  // Trigger the YM test code.
  //ym_test();

  // Output a single note to say hello.
  // elapsedMicros timer;
  // setreg(0x28, 0xF0); // Key on
  // Serial.println(timer);
  // delay(1000);

  // Initialize the VGM player.
  vgm_init();
}

void loop() {
  vgm_loop();
  //song();
  //delay(100);
}

#include "hardware_constants.h"
#include "midi_interface.h"
#include "src/theacommon/buttons.h"
#include "src/theacommon/tasks.h"
#include "synth.h"
#include "ui.h"
#include "vgm.h"
#include "ym2612.h"

thea::TaskManager taskmgr;
thea::Task button_task("Btns", &thea::buttons::loop, 0);
thea::Task ui_task("UI", &thea::ui::loop, DISPLAY_RATE);
thea::Task midi_task("MIDI", &thea::midi_interface::loop, 0);
thea::Task synth_task("Synth", &thea::synth::loop, 0);

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

  // Add tasks to the task manager.
  taskmgr.add(&button_task);
  taskmgr.add(&ui_task);
  taskmgr.add(&midi_task);
  taskmgr.add(&synth_task);
  thea::ui::set_task_manager(&taskmgr);

  Serial.println("Setup complete.");
}

void loop() { taskmgr.run(); }

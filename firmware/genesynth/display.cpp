#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

#include "algorithm_gfx.h"
#include "display.h"
#include "operator_gfx.h"
#include "synth.h"
#include "thea_easter_egg.h"

namespace thea {
namespace display {

#define DISPLAY_RATE 66666 // 1/15th of a second.
#define ENV_SCREEN_DISPLAY_TIME 1000000

Screen screen = Screen::THEA;
unsigned long last_display_time = micros();
unsigned long next_display_time = last_display_time;
unsigned long screen_start_time = last_display_time;
unsigned long screen_time = last_display_time + 1600000;

U8G2_SH1106_128X64_NONAME_2_4W_HW_SPI u8g2(/* rotation=*/U8G2_R2, /* cs=*/10, /* dc=*/9, /* reset=*/8);

inline void use_small_font() { u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f); }

inline void use_big_font() { u8g2.setFont(u8g2_font_fub14_tf); }

void screen_notes() {
  u8g2.setDrawColor(1);
  u8g2.setCursor(0, 0);

  if ((micros() / 2500000 % 2) == 0) {
    u8g2.printf("> %s", thea::synth::patch.name);
  } else {
    u8g2.printf("# %s", thea::synth::patch.bank);
  }

  switch (thea::synth::patch.algorithm) {
  case 0:
    thea::algorithm_gfx::draw_alg_0(u8g2);
    break;
  case 1:
    thea::algorithm_gfx::draw_alg_1(u8g2);
    break;
  case 2:
    thea::algorithm_gfx::draw_alg_2(u8g2);
    break;
  case 3:
    thea::algorithm_gfx::draw_alg_3(u8g2);
    break;
  case 4:
    thea::algorithm_gfx::draw_alg_4(u8g2);
    break;
  case 5:
    thea::algorithm_gfx::draw_alg_5(u8g2);
    break;
  case 6:
    thea::algorithm_gfx::draw_alg_6(u8g2);
    break;
  case 7:
    thea::algorithm_gfx::draw_alg_7(u8g2);
    break;
  default:
    u8g2.setCursor(0, 9);
    u8g2.printf("Unkwn alg 0x%x\n", thea::synth::patch.algorithm);
  }
};

void screen_opedit() {
  thea::operator_gfx::draw_parameter_edit_screen(u8g2, thea::synth::patch, thea::synth::last_write_option);
}

void sreen_envedit() {
  thea::operator_gfx::draw_envelope_edit_screen(u8g2, thea::synth::patch, thea::synth::last_write_option);
}

void screen_debug() {
  u8g2.setCursor(0, 0);
  u8g2.print("Waiting for serial monitor.");
}

void loop(void) {
  auto now = micros();
  // Don't display more often than needed.
  if (now < next_display_time) {
    return;
  }

  // Switch the screen based on the synth state, if necessary.
  // This should happen when the last patch modify time is recent enough.
  if (screen == Screen::NOTES) {
    auto was_modified_recently = thea::synth::last_patch_modify_time > (now - ENV_SCREEN_DISPLAY_TIME);
    if (was_modified_recently && thea::synth::last_write_option != thea::ym2612::ChannelPatch::WriteOption::ALL) {
      auto normalized_option = thea::synth::last_write_option / 10;
      auto is_env = (normalized_option >= thea::ym2612::ChannelPatch::WriteOption::OP0_TL &&
                     normalized_option <= thea::ym2612::ChannelPatch::WriteOption::OP0_RR);
      if (is_env) {
        show(Screen::ENVEDIT, ENV_SCREEN_DISPLAY_TIME);
      } else {
        show(Screen::OPEDIT, ENV_SCREEN_DISPLAY_TIME);
      }
    }
  }

  // Draw the screen.
  u8g2.firstPage();
  do {
    /* all graphics commands have to appear within the loop body. */
    switch (screen) {
    case Screen::THEA:
      thea::show_thea(&u8g2, now - screen_start_time);
      break;
    case Screen::DEBUG:
      screen_debug();
      break;
    case Screen::NOTES:
      screen_notes();
      break;
    case Screen::OPEDIT:
      screen_opedit();
      break;
    case Screen::ENVEDIT:
      sreen_envedit();
      break;
    }
  } while (u8g2.nextPage());

  last_display_time = micros();
  next_display_time = now + DISPLAY_RATE;

  if (screen != Screen::NOTES && last_display_time > screen_time) {
    screen = Screen::NOTES;
  }
}

void init(void) {
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setFontPosTop();
  use_small_font();
}

void show(Screen screen, unsigned long duration) {
  thea::display::screen = screen;
  screen_start_time = micros();
  screen_time = screen_start_time + duration;
}

} // namespace thea
} // namespace display

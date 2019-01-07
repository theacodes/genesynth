#include <Arduino.h>

#include "algorithm_gfx.h"
#include "ambient_ui.h"
#include "operator_gfx.h"
#include "src/theacommon/thea_easter_egg.h"

namespace thea {
namespace ambient_ui {

#define ENV_SCREEN_DISPLAY_TIME 1000000

enum Screen {
  THEA,
  IDLE,
  OPEDIT,
  ENVEDIT,
};

Screen screen = Screen::THEA;
unsigned long last_display_time = micros();
unsigned long screen_start_time = last_display_time;
unsigned long screen_time = last_display_time + 1600000;

inline void use_small_font(U8G2 &u8g2) { u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f); }
inline void use_big_font(U8G2 &u8g2) { u8g2.setFont(u8g2_font_fub14_tf); }

void show(Screen screen, unsigned long duration) {
  thea::ambient_ui::screen = screen;
  screen_start_time = micros();
  screen_time = screen_start_time + duration;
}

void screen_idle(U8G2 &u8g2, thea::ym2612::ChannelPatch &patch) {
  u8g2.setDrawColor(1);
  u8g2.setCursor(0, 0);

  if ((micros() / 2500000 % 2) == 0) {
    u8g2.printf("> %s", patch.name);
  } else {
    u8g2.printf("# %s", patch.bank);
  }

  switch (patch.algorithm) {
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
    u8g2.printf("Unkwn alg 0x%x\n", patch.algorithm);
  }
};

void display(U8G2 &u8g2, thea::ym2612::ChannelPatch &patch, thea::ym2612::ChannelPatch::WriteOption write_option,
             unsigned long last_patch_modify_time) {
  auto now = micros();

  // Switch the screen based on the synth state, if necessary.
  // This should happen when the last patch modify time is recent enough.
  if (screen == Screen::IDLE) {
    auto was_modified_recently = last_patch_modify_time > (now - ENV_SCREEN_DISPLAY_TIME);
    if (was_modified_recently && write_option != thea::ym2612::ChannelPatch::WriteOption::ALL) {
      auto normalized_option = write_option % 10;
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
  switch (screen) {
  case Screen::THEA:
    thea::show_thea(&u8g2, now - screen_start_time);
    break;
  case Screen::IDLE:
    screen_idle(u8g2, patch);
    break;
  case Screen::OPEDIT:
    thea::operator_gfx::draw_parameter_edit_screen(u8g2, patch, write_option);
    break;
  case Screen::ENVEDIT:
    thea::operator_gfx::draw_envelope_edit_screen(u8g2, patch, write_option);
    break;
  }

  // Reset to IDLE if needed
  last_display_time = micros();
  if (screen != Screen::IDLE && last_display_time > screen_time) {
    screen = Screen::IDLE;
  }
}

} // namespace ambient_ui
} // namespace thea

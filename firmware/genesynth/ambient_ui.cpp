#include <Arduino.h>

#include "algorithm_gfx.h"
#include "ambient_ui.h"
#include "operator_gfx.h"
#include "src/theacommon/thea_easter_egg.h"
#include "synth.h"

namespace thea {
namespace ambient_ui {

#define ENV_SCREEN_DISPLAY_TIME 1000000

enum Screen {
  THEA,
  IDLE,
  OPEDIT,
  ENVEDIT,
  LFO,
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

  u8g2.drawLine(0, 64 - 9, 7, 64 - 9);
  u8g2.setCursor(0, 64 - 9);
  u8g2.setDrawColor(0);
  switch (thea::synth::get_note_mode()) {
  case thea::synth::NoteMode::POLY:
    u8g2.printf("P");
    break;
  case thea::synth::NoteMode::MONO:
    u8g2.printf("M");
    break;
  case thea::synth::NoteMode::UNISON:
    u8g2.printf("U");
    u8g2.setDrawColor(1);
    u8g2.setCursor(9, 64 - 9);
    u8g2.printf("x%i\xB1%.0f%%", thea::synth::get_unison_voices(), thea::synth::get_unison_spread() * 100.0f);
    break;
  }
};

void screen_lfo(U8G2 &u8g2, thea::ym2612::ChannelPatch &patch) {
  u8g2.setDrawColor(1);
  u8g2.setCursor(0, 0);

  u8g2.printf("> LFO");

  u8g2.setCursor(8 + 64, 32 - 9);
  if (thea::synth::lfo_enabled()) {
    u8g2.drawLine(8 + 64, 32 - 9, 8 + 64 + 15, 32 - 9);
    u8g2.setDrawColor(0);
    u8g2.printf("ON");
  } else {
    u8g2.printf("OFF");
  }
  u8g2.setDrawColor(1);

  auto lfo_freq = 0.0f;
  switch (thea::synth::get_lfo_freq()) {
  case 0:
    lfo_freq = 3.98f;
    break;
  case 1:
    lfo_freq = 5.56f;
    break;
  case 2:
    lfo_freq = 6.02f;
    break;
  case 3:
    lfo_freq = 6.37f;
    break;
  case 4:
    lfo_freq = 6.88f;
    break;
  case 5:
    lfo_freq = 9.63;
    break;
  case 6:
    lfo_freq = 48.1;
    break;
  case 7:
    lfo_freq = 72.2;
    break;
  default:
    break;
  }

  auto waveoffset = (millis() / 1000.0f) * lfo_freq;
  auto offsetx = 4;
  auto offsety = 16;
  auto height = 30;
  auto width = 64;

  for (auto x = 0; x < width; x++) {
    auto wave = sin(float(x) / width * 2 * M_PI + waveoffset);
    u8g2.drawPixel(offsetx + x, offsety + (height / 2) + (wave * height / 2));
  }

  u8g2.setCursor(8 + 64, 32);
  u8g2.printf("%.2fHz", lfo_freq);

  auto lfo_ams = 0.0f;
  switch (patch.lfo_ams) {
  case 0:
    break;
  case 1:
    lfo_ams = 1.4f;
    break;
  case 2:
    lfo_ams = 5.9f;
    break;
  case 3:
    lfo_ams = 11.8f;
    break;
  default:
    break;
  }

  u8g2.drawLine(0, 64 - 9, 7, 64 - 9);
  u8g2.setDrawColor(0);
  u8g2.setCursor(0, 64 - 9);
  u8g2.printf("A");

  u8g2.setDrawColor(1);
  u8g2.setCursor(9, 64 - 9);
  u8g2.printf("%1.fdB", lfo_ams);

  auto lfo_fms = 0.0f;
  switch (patch.lfo_fms) {
  case 0:
    break;
  case 1:
    lfo_fms = 3.4f;
    break;
  case 2:
    lfo_fms = 6.7f;
    break;
  case 3:
    lfo_fms = 10.0f;
    break;
  case 4:
    lfo_fms = 14.0f;
    break;
  case 5:
    lfo_fms = 20.0f;
    break;
  case 6:
    lfo_fms = 40.0f;
    break;
  case 7:
    lfo_fms = 80.0f;
    break;
  default:
    break;
  }

  u8g2.drawLine(64, 64 - 9, 64 + 7, 64 - 9);
  u8g2.setDrawColor(0);
  u8g2.setCursor(64, 64 - 9);
  u8g2.printf("F");

  u8g2.setDrawColor(1);
  u8g2.setCursor(64 + 9, 64 - 9);
  u8g2.printf("%.1f%%", lfo_fms);
}

void display(U8G2 &u8g2, thea::ym2612::ChannelPatch &patch, thea::ym2612::ChannelPatch::WriteOption write_option,
             unsigned long last_patch_modify_time) {
  auto now = micros();

  // Switch the screen based on the synth state, if necessary.
  // This should happen when the last patch modify time is recent enough.
  if (screen == Screen::IDLE) {
    auto was_modified_recently = last_patch_modify_time > (now - ENV_SCREEN_DISPLAY_TIME);
    if (was_modified_recently && write_option != thea::ym2612::ChannelPatch::WriteOption::ALL) {
      auto is_op = (write_option >= thea::ym2612::ChannelPatch::WriteOption::OP0_DT1 &&
                    write_option <= thea::ym2612::ChannelPatch::WriteOption::OP3_AM);
      if (is_op) {
        auto normalized_option = write_option % 10;
        auto is_env = (normalized_option >= thea::ym2612::ChannelPatch::WriteOption::OP0_TL &&
                       normalized_option <= thea::ym2612::ChannelPatch::WriteOption::OP0_RR);
        if (is_env) {
          show(Screen::ENVEDIT, ENV_SCREEN_DISPLAY_TIME);
        } else {
          show(Screen::OPEDIT, ENV_SCREEN_DISPLAY_TIME);
        }
      }

      if (write_option == thea::ym2612::ChannelPatch::WriteOption::LFO_AMS ||
          write_option == thea::ym2612::ChannelPatch::WriteOption::LFO_FMS) {
        show(Screen::LFO, ENV_SCREEN_DISPLAY_TIME);
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
  case Screen::LFO:
    screen_lfo(u8g2, patch);
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

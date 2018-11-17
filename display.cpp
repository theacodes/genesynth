#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

#include "display.h"
#include "synth.h"
#include "thea.h"

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

inline void draw_op_symbol(uint8_t num, uint8_t x, uint8_t y, bool shaded) {
  if (shaded) {
    u8g2.setDrawColor(0);
    u8g2.drawBox(x, y, 12, 12);
    u8g2.setDrawColor(1);
    u8g2.drawFrame(x, y, 12, 12);
    u8g2.setDrawColor(1);
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(x, y, 12, 12);
    u8g2.setDrawColor(0);
  }
  u8g2.setCursor(x + 2, y + 2);
  u8g2.printf("%i", num);
}

inline void draw_out_symbol(uint8_t x, uint8_t y) {
  u8g2.setDrawColor(1);
  u8g2.setFontMode(1);
  u8g2.drawGlyph(x - 2, y + 2, '>');
  u8g2.setFontMode(0);
}

inline void draw_alg_0() {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t total_width = (width * 4);
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;

  u8g2.drawLine(start_x, start_y + half_height, start_x + width * 4, start_y + half_height);
  draw_op_symbol(1, start_x, start_y, true);
  draw_op_symbol(2, start_x + width, start_y, true);
  draw_op_symbol(3, start_x + width * 2, start_y, true);
  draw_op_symbol(4, start_x + width * 3, start_y, false);
  draw_out_symbol(start_x + width * 4, start_y);
}

inline void draw_alg_1() {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t gap = 5;
  const uint8_t total_width = (width * 3) + gap;
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;

  // vertical line connecting 1 & 2
  u8g2.drawLine(start_x + width, start_y - 3, start_x + width, start_y + height + 3);
  // Short horizontal lines connecting 1 & 2 to the vertical line.
  u8g2.drawLine(start_x, start_y - 3, start_x + width, start_y - 3);
  u8g2.drawLine(start_x, start_y + height + 3, start_x + width, start_y + height + 3);
  // horizontal line connecting 3 & 4
  u8g2.drawLine(start_x + width, start_y + half_height, start_x + width * 3 + gap, start_y + half_height);

  draw_op_symbol(1, start_x, start_y - half_height - 1, true);
  draw_op_symbol(2, start_x, start_y + half_height + 2, true);
  draw_op_symbol(3, start_x + width + gap, start_y, true);
  draw_op_symbol(4, start_x + width * 2 + gap, start_y, false);

  draw_out_symbol(start_x + width * 3 + gap, start_y);
}

inline void draw_alg_2() {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t gap = 5;
  const uint8_t total_width = (width * 3) + gap;
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;
  const uint8_t upper_row_y = start_y - half_height - 1;
  const uint8_t lower_row_y = start_y + half_height + 2;

  // vertical line connecting 1 & 3
  u8g2.drawLine(start_x + width * 2, start_y - 3, start_x + width * 2, start_y + height + 3);
  // Short horizontal lines connecting 1 & 3 to the vertical line.
  u8g2.drawLine(start_x, start_y - 3, start_x + width * 2, start_y - 3);
  u8g2.drawLine(start_x, start_y + height + 3, start_x + width * 2, start_y + height + 3);
  // horizontal line connecting the vertical line to 4.
  u8g2.drawLine(start_x + width * 2, start_y + half_height, start_x + width * 3 + gap, start_y + half_height);

  draw_op_symbol(1, start_x, upper_row_y, true);
  draw_op_symbol(2, start_x, lower_row_y, true);
  draw_op_symbol(3, start_x + width, lower_row_y, true);
  draw_op_symbol(4, start_x + width * 2 + gap, start_y, false);

  draw_out_symbol(start_x + width * 3 + gap, start_y);
}

inline void draw_alg_3() {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t gap = 5;
  const uint8_t total_width = (width * 3) + gap;
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;
  const uint8_t upper_row_y = start_y - half_height - 1;
  const uint8_t lower_row_y = start_y + half_height + 2;

  // vertical line connecting 1 & 3
  u8g2.drawLine(start_x + width * 2, start_y - 3, start_x + width * 2, start_y + height + 3);
  // Short horizontal lines connecting 1 & 3 to the vertical line.
  u8g2.drawLine(start_x, start_y - 3, start_x + width * 2, start_y - 3);
  u8g2.drawLine(start_x + width, start_y + height + 3, start_x + width * 2, start_y + height + 3);
  // horizontal line connecting the vertical line to 4.
  u8g2.drawLine(start_x + width * 2, start_y + half_height, start_x + width * 3 + gap, start_y + half_height);

  draw_op_symbol(1, start_x, upper_row_y, true);
  draw_op_symbol(2, start_x + width, upper_row_y, true);
  draw_op_symbol(3, start_x + width, lower_row_y, true);
  draw_op_symbol(4, start_x + width * 2 + gap, start_y, false);

  draw_out_symbol(start_x + width * 3 + gap, start_y);
}

inline void draw_alg_4() {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t gap = 5;
  const uint8_t total_width = (width * 2) + gap;
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;
  const uint8_t upper_row_y = start_y - half_height - 1;
  const uint8_t lower_row_y = start_y + half_height + 2;

  // vertical line connecting 1 & 3
  u8g2.drawLine(start_x + width * 2, start_y - 3, start_x + width * 2, start_y + height + 3);
  // Short horizontal lines connecting 1 & 3 to the vertical line.
  u8g2.drawLine(start_x, start_y - 3, start_x + width * 2, start_y - 3);
  u8g2.drawLine(start_x, start_y + height + 3, start_x + width * 2, start_y + height + 3);
  // horizontal line connecting the vertical line to output.
  u8g2.drawLine(start_x + width * 2, start_y + half_height, start_x + width * 2 + gap, start_y + half_height);

  draw_op_symbol(1, start_x, upper_row_y, true);
  draw_op_symbol(2, start_x + width, upper_row_y, false);
  draw_op_symbol(3, start_x, lower_row_y, true);
  draw_op_symbol(4, start_x + width, lower_row_y, false);

  draw_out_symbol(start_x + width * 2 + gap, start_y);
}

inline void draw_alg_5() {
  const uint8_t height = 16;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t gap = 5;
  const uint8_t total_width = (width * 2) + (gap * 2);
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;

  // horizontal line connecting 1, 3, and the output.
  u8g2.drawLine(start_x, start_y + half_height - 2, start_x + (width * 2) + (gap * 2), start_y + half_height - 2);
  // horizontal line connecting 2 and 4 to the net.
  u8g2.drawLine(start_x + width, start_y - half_height - 2, start_x + width * 2 + gap, start_y - half_height - 2);
  u8g2.drawLine(start_x + width, start_y + height + half_height - 2, start_x + width * 2 + gap,
                start_y + height + half_height - 2);
  // Vertical lines on either side of 2, 3, and 4.
  u8g2.drawLine(start_x + width, start_y - half_height - 2, start_x + width, start_y + height + half_height - 2);
  u8g2.drawLine(start_x + width * 2 + gap, start_y - half_height - 2, start_x + width * 2 + gap,
                start_y + height + half_height - 2);

  draw_op_symbol(1, start_x, start_y, true);
  draw_op_symbol(2, start_x + gap + width, start_y - height, false);
  draw_op_symbol(3, start_x + gap + width, start_y, false);
  draw_op_symbol(4, start_x + gap + width, start_y + height, false);

  draw_out_symbol(start_x + (width * 2) + (gap * 2), start_y);
}

inline void draw_alg_6() {
  const uint8_t height = 16;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t gap = 5;
  const uint8_t total_width = (width * 2) + gap;
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;

  // horizontal line connecting 3 and the output.
  u8g2.drawLine(start_x + width + gap, start_y + half_height - 2, start_x + (width * 2) + gap,
                start_y + half_height - 2);
  // horizontal line connecting 2 and 4 to the net.
  u8g2.drawLine(start_x, start_y - half_height - 2, start_x + width * 2, start_y - half_height - 2);
  u8g2.drawLine(start_x + width, start_y + height + half_height - 2, start_x + width * 2,
                start_y + height + half_height - 2);
  // Vertical lines on right side of 2, 3, and 4.
  u8g2.drawLine(start_x + width * 2, start_y - half_height - 2, start_x + width * 2,
                start_y + height + half_height - 2);

  draw_op_symbol(1, start_x, start_y - height, true);
  draw_op_symbol(2, start_x + width, start_y - height, false);
  draw_op_symbol(3, start_x + width, start_y, false);
  draw_op_symbol(4, start_x + width, start_y + height, false);

  draw_out_symbol(start_x + (width * 2) + gap, start_y);
}

inline void draw_alg_7() {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t half_width = width / 2;
  const uint8_t total_width = (width * 4);
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;

  // Horizontal line connecting output.
  u8g2.drawLine(start_x + half_width - 2, start_y - half_height, start_x + width * 4, start_y - half_height);
  // Vertical lines connecting each operator.
  for (int i = 0; i < 4; i++) {
    uint8_t x = start_x + (width * i) + half_width - 2;
    u8g2.drawLine(x, start_y, x, start_y - half_height);
  }

  draw_op_symbol(1, start_x, start_y, false);
  draw_op_symbol(2, start_x + width, start_y, false);
  draw_op_symbol(3, start_x + width * 2, start_y, false);
  draw_op_symbol(4, start_x + width * 3, start_y, false);
  draw_out_symbol(start_x + width * 4, start_y - height);
}

void screen_notes() {
  u8g2.setDrawColor(1);
  u8g2.setCursor(0, 0);
  u8g2.printf("> %s", thea::synth::patch.name);

  switch (thea::synth::patch.algorithm) {
  case 0:
    draw_alg_0();
    break;
  case 1:
    draw_alg_1();
    break;
  case 2:
    draw_alg_2();
    break;
  case 3:
    draw_alg_3();
    break;
  case 4:
    draw_alg_4();
    break;
  case 5:
    draw_alg_5();
    break;
  case 6:
    draw_alg_6();
    break;
  case 7:
    draw_alg_7();
    break;
  default:
    u8g2.setCursor(0, 9);
    u8g2.printf("Unkwn alg 0x%x\n", thea::synth::patch.algorithm);
  }
};

void screen_opedit() {
  uint8_t op_no = thea::synth::last_write_option / 10;
  auto op = thea::synth::patch.operators[op_no];

  bool detune_negative = op.DT1 & 0x4;
  signed int detune = op.DT1 & 0x3;

  u8g2.setCursor(0, 9 * 0);
  u8g2.printf("> Operator %i\n", op_no + 1);

  auto x = 32;
  auto y = 22;
  auto spacing = 45;
  u8g2.setCursor(x, y + 6);
  u8g2.printf("x");
  u8g2.setCursor(x + spacing, y + 6);
  u8g2.printf(detune_negative ? "+" : "-");

  use_big_font();
  u8g2.setCursor(x + 8, y);
  if (op.MUL == 0) {
    u8g2.printf("%c", 0xbd); // "1/2" glyph
  } else {
    u8g2.printf("%i", op.MUL);
  }
  u8g2.setCursor(x + spacing + 8, y);
  u8g2.printf("%i", detune);

  use_small_font();

  y += 20;

  if (op.RS > 0) {
    u8g2.setDrawColor(1);
    u8g2.drawBox(x, y, 30, 12);
    u8g2.setDrawColor(0);
    u8g2.setCursor(x + 3, y + 2);
    u8g2.printf("RS%i", op.RS);
    u8g2.setDrawColor(1);
  };

  if (op.AM) {
    u8g2.setDrawColor(1);
    u8g2.drawBox(x + 35, y, 30, 12);
    u8g2.setDrawColor(0);
    u8g2.setCursor(x + 35 + 7, y + 2);
    u8g2.printf("AM");
    u8g2.setDrawColor(1);
  }
}

void sreen_envedit() {
  uint8_t op_no = thea::synth::last_write_option / 10;
  auto op = thea::synth::patch.operators[op_no];

  float w = 128;
  float y_offset = 18;
  float h = 64 - y_offset;

  // Calculate normalized (0-1.0) floats for each envelope parameters
  float level = 1.0f - (op.TL / 128.0f); // inverted.
  float attack = op.AR / 32.0;
  float decay = op.D1R / 32.0f;
  float sustain = 1.0f - (op.D1L / 16.0f);
  float release = op.RR / 16.0f;

  // Attack can occupy up to 25% of the screen.
  float attack_percentage = attack * 0.25f;
  // Decay likewise
  float delay_percentage = decay * 0.25f;
  // Sustain is height, so it gets the full range.
  float sustain_percentage = sustain;
  // Release can occupy at most 25% of the screen.
  float release_percentage = release * 0.25f;

  // level reduces the total height of the graph. It's inverted, though.
  y_offset += h - (h * level);
  h *= level;
  // Calculate graph line points.
  int attack_x_end = attack_percentage * w;
  int delay_x_end = attack_x_end + delay_percentage * w;
  int sustain_y = h - sustain_percentage * h;
  int release_x_start = w - release_percentage * w;

  // Draw envelope graph
  u8g2.setDrawColor(1);
  u8g2.drawLine(0, y_offset + h, attack_x_end, y_offset);
  u8g2.drawLine(attack_x_end, y_offset, delay_x_end, y_offset + sustain_y);
  u8g2.drawLine(delay_x_end, y_offset + sustain_y, release_x_start, y_offset + sustain_y);
  u8g2.drawLine(release_x_start, y_offset + sustain_y, w, y_offset + h);

  // Draw text
  u8g2.setCursor(0, 0);
  u8g2.printf("> Op %i Envelope", op_no + 1);
  u8g2.setCursor(0, 9);
  switch (thea::synth::last_write_option) {
  case thea::ym2612::ChannelPatch::WriteOption::OP0_TL:
    u8g2.printf("Level: %i", int(level * 100));
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_AR:
    u8g2.printf("Attack: %i", int(attack * 100));
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D1R:
    u8g2.printf("Decay: %i", int(decay * 100));
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D2R:
    u8g2.printf("TODO");
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D1L:
    u8g2.printf("Sustain: %i", int(sustain * 100));
    break;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_RR:
    u8g2.printf("Release: %i", int(release * 100));
    break;
  default:
    break;
  }

  // Debug: show write latency
  u8g2.setCursor(0, 45);
  u8g2.printf("Latency: %i", thea::ym2612::get_latency());
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

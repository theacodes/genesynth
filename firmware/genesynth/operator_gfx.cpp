#include "operator_gfx.h"

namespace thea {
namespace operator_gfx {

inline void use_small_font(U8G2 &u8g2) { u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f); }
inline void use_big_font(U8G2 &u8g2) { u8g2.setFont(u8g2_font_fub14_tf); }

void draw_parameter_edit_screen(U8G2 &u8g2, const thea::ym2612::ChannelPatch &patch,
                                thea::ym2612::ChannelPatch::WriteOption write_option) {
  uint8_t op_no = write_option / 10;
  auto op = patch.operators[op_no];

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

  use_big_font(u8g2);
  u8g2.setCursor(x + 8, y);
  if (op.MUL == 0) {
    u8g2.printf("%c", 0xbd); // "1/2" glyph
  } else {
    u8g2.printf("%i", op.MUL);
  }
  u8g2.setCursor(x + spacing + 8, y);
  u8g2.printf("%i", detune);

  use_small_font(u8g2);

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

void draw_envelope_edit_screen(U8G2 &u8g2, const thea::ym2612::ChannelPatch &patch,
                               thea::ym2612::ChannelPatch::WriteOption write_option) {
  uint8_t op_no = write_option / 10;
  auto op = patch.operators[op_no];

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

  switch (write_option % 10) {
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
};

} // namespace operator_gfx
} // namespace thea

#include "algorithm_gfx.h"
#include <stdint.h>

namespace thea {
namespace algorithm_gfx {

void draw_op_symbol(U8G2 &u8g2, uint8_t num, uint8_t x, uint8_t y, bool shaded) {
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

void draw_out_symbol(U8G2 &u8g2, uint8_t x, uint8_t y) {
  u8g2.setDrawColor(1);
  u8g2.setFontMode(1);
  u8g2.drawGlyph(x - 2, y + 2, '>');
  u8g2.setFontMode(0);
}

void draw_alg_0(U8G2 &u8g2) {
  const uint8_t height = 12;
  const uint8_t half_height = height / 2;
  const uint8_t width = 16;
  const uint8_t total_width = (width * 4);
  const uint8_t start_x = (128 - total_width) / 2;
  const uint8_t start_y = 30;

  u8g2.drawLine(start_x, start_y + half_height, start_x + width * 4, start_y + half_height);
  draw_op_symbol(u8g2, 1, start_x, start_y, true);
  draw_op_symbol(u8g2, 2, start_x + width, start_y, true);
  draw_op_symbol(u8g2, 3, start_x + width * 2, start_y, true);
  draw_op_symbol(u8g2, 4, start_x + width * 3, start_y, false);
  draw_out_symbol(u8g2, start_x + width * 4, start_y);
}

void draw_alg_1(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, start_y - half_height - 1, true);
  draw_op_symbol(u8g2, 2, start_x, start_y + half_height + 2, true);
  draw_op_symbol(u8g2, 3, start_x + width + gap, start_y, true);
  draw_op_symbol(u8g2, 4, start_x + width * 2 + gap, start_y, false);

  draw_out_symbol(u8g2, start_x + width * 3 + gap, start_y);
}

void draw_alg_2(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, upper_row_y, true);
  draw_op_symbol(u8g2, 2, start_x, lower_row_y, true);
  draw_op_symbol(u8g2, 3, start_x + width, lower_row_y, true);
  draw_op_symbol(u8g2, 4, start_x + width * 2 + gap, start_y, false);

  draw_out_symbol(u8g2, start_x + width * 3 + gap, start_y);
}

void draw_alg_3(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, upper_row_y, true);
  draw_op_symbol(u8g2, 2, start_x + width, upper_row_y, true);
  draw_op_symbol(u8g2, 3, start_x + width, lower_row_y, true);
  draw_op_symbol(u8g2, 4, start_x + width * 2 + gap, start_y, false);

  draw_out_symbol(u8g2, start_x + width * 3 + gap, start_y);
}

void draw_alg_4(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, upper_row_y, true);
  draw_op_symbol(u8g2, 2, start_x + width, upper_row_y, false);
  draw_op_symbol(u8g2, 3, start_x, lower_row_y, true);
  draw_op_symbol(u8g2, 4, start_x + width, lower_row_y, false);

  draw_out_symbol(u8g2, start_x + width * 2 + gap, start_y);
}

void draw_alg_5(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, start_y, true);
  draw_op_symbol(u8g2, 2, start_x + gap + width, start_y - height, false);
  draw_op_symbol(u8g2, 3, start_x + gap + width, start_y, false);
  draw_op_symbol(u8g2, 4, start_x + gap + width, start_y + height, false);

  draw_out_symbol(u8g2, start_x + (width * 2) + (gap * 2), start_y);
}

void draw_alg_6(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, start_y - height, true);
  draw_op_symbol(u8g2, 2, start_x + width, start_y - height, false);
  draw_op_symbol(u8g2, 3, start_x + width, start_y, false);
  draw_op_symbol(u8g2, 4, start_x + width, start_y + height, false);

  draw_out_symbol(u8g2, start_x + (width * 2) + gap, start_y);
}

void draw_alg_7(U8G2 &u8g2) {
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

  draw_op_symbol(u8g2, 1, start_x, start_y, false);
  draw_op_symbol(u8g2, 2, start_x + width, start_y, false);
  draw_op_symbol(u8g2, 3, start_x + width * 2, start_y, false);
  draw_op_symbol(u8g2, 4, start_x + width * 3, start_y, false);
  draw_out_symbol(u8g2, start_x + width * 4, start_y - height);
}

} // namespace algorithm_gfx
} // namespace thea

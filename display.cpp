#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#include "display.h"

namespace thea {
namespace display {

DisplayState display_state;
Screen screen = Screen::NOTES;
#define DISPLAY_RATE 66666  // 1/15th of a second.
unsigned long last_display_time = micros();
unsigned long next_display_time = last_display_time;
unsigned long screen_time = last_display_time;

#define ONOFF(x) x ? 'O' : '-'

U8G2_SH1106_128X64_NONAME_2_4W_HW_SPI u8g2(/* rotation=*/ U8G2_R2, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

void init(void) {
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);
  u8g2.setFontPosTop();
}

void screen_notes(){
  u8g2.setCursor(0, 0);
  u8g2.printf("> %s", display_state.patch->name);
  u8g2.setCursor(0, 9);
  u8g2.printf(
    "fm: %c %c %c %c %c %c",
    ONOFF(display_state.fm_channels[0]),
    ONOFF(display_state.fm_channels[1]),
    ONOFF(display_state.fm_channels[2]),
    ONOFF(display_state.fm_channels[3]),
    ONOFF(display_state.fm_channels[4]),
    ONOFF(display_state.fm_channels[5]));
  u8g2.setCursor(0, 18);
  u8g2.printf("sq: %c %c %c",
    ONOFF(display_state.sq_channels[0]),
    ONOFF(display_state.sq_channels[1]),
    ONOFF(display_state.sq_channels[2]));
};

void screen_opedit(){
  auto op = display_state.patch->operators[0];
  u8g2.setCursor(0, 9 * 0);
  u8g2.printf("> Operator 1\n");
  u8g2.setCursor(0, 9 * 1);
  u8g2.printf("Detune %#04x\n", op.DT1);
  u8g2.setCursor(0, 9 * 2);
  u8g2.printf("Multiplier %#04x\n", op.MUL);
  u8g2.setCursor(0, 9 * 3);
  u8g2.printf("Total Level %#04x\n", op.TL);
  u8g2.setCursor(0, 9 * 4);
  u8g2.printf("Attack Rate %#04x\n", op.AR);
  u8g2.setCursor(0, 9 * 5);
  u8g2.printf("Decay Rate 1 %#04x\n", op.D1R);
  u8g2.setCursor(0, 9 * 6);
  u8g2.printf("Decay Rate 2 %#04x\n", op.D2R);
  u8g2.setCursor(0, 9 * 7);
  u8g2.printf("Decay Level %#04x\n", op.D1L);
  u8g2.setCursor(0, 9 * 8);
  u8g2.printf("RR %#04x\n", op.RR);
  u8g2.setCursor(0, 9 * 9);
  u8g2.printf("Rate Scaling %#04x\n", op.RS);
  u8g2.setCursor(0, 9 * 10);
  u8g2.printf("Amplitude Mod %#04x\n", op.AM);
}

void sreen_envedit() {
  auto op = display_state.patch->operators[0];

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
  int delay_x_end = attack_x_end + delay_percentage* w;
  int sustain_y = h - sustain_percentage * h;
  int release_x_start = w - release_percentage * w;

  // Draw envelope graph
  u8g2.setDrawColor(1);
  u8g2.drawLine(0, y_offset+h, attack_x_end, y_offset);
  u8g2.drawLine(attack_x_end, y_offset, delay_x_end, y_offset+sustain_y);
  u8g2.drawLine(delay_x_end, y_offset + sustain_y, release_x_start, y_offset + sustain_y);
  u8g2.drawLine(release_x_start, y_offset + sustain_y, w, y_offset + h);

  // Draw text
  u8g2.setCursor(0, 0);
  u8g2.printf("> Op 1 Envelope");
  u8g2.setCursor(0, 9);
  switch(display_state.write_option) {
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
}

void loop(void) {
  // Don't display more often than needed.
  if(micros() < next_display_time) {
    return;
  }

  u8g2.firstPage();
  do {
    /* all graphics commands have to appear within the loop body. */
    switch(screen) {
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
  } while ( u8g2.nextPage() );

  last_display_time = micros();
  next_display_time = micros() + DISPLAY_RATE;

  if(screen != Screen::NOTES && last_display_time > screen_time) {
    screen = Screen::NOTES;
  }
}

void show(Screen screen, unsigned long duration) {
  thea::display::screen = screen;
  screen_time = micros() + duration;
}

} // namespace thea
} // namespace display

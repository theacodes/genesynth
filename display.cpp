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

U8X8_SH1106_128X64_NONAME_4W_HW_SPI display(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
U8G2_SH1106_128X64_NONAME_2_4W_HW_SPI u8g2(/* rotation=*/ U8G2_R2, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

void init(void) {
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);
  u8g2.setFontPosTop();
}

void screen_notes(){
  u8g2.setCursor(0, 0);
  u8g2.print("> hydrocity1");
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

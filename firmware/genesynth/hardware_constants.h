#ifndef THEA_HARDWARE_CONSTANTS_H
#define THEA_HARDWARE_CONSTANTS_H

#define STATUS_LED 13

/* YM2612 Constants */

// Pin 14-21 to YM DATA
// NOTE: The PCB motherboard reverses the order!
#define YM_DATA 21
// Pin 33 to YM IC
#define YM_IC 33
// Pin 32 to YM A1
#define YM_A1 32
// Pin 31 to YM A0
#define YM_A0 31
// Pin 30 to YM WR
#define YM_WR 29
// Pin 29 to YM CS
#define YM_CS 28
// Pin 28 to YM RD
#define YM_RD 30
// How long to hold the WR and CS lines when writing data.
// The datasheet indicates this should be *at least* 200nS, but it appears
// that a setting of (200) ~2,000uS ensures that the chip always gets our messages.
#define YM_WRITE_WAIT 11 // * 10uS
// The maximum about of uC cycles to wait for the YM2612 to be ready to receive data.
// This is around 10,000uS.
#define YM_MAX_WAIT_CYCLES 10000
// How long to hold the reset line.
// According to bus timing documents, we must hold the line low for at least
// 192 cycles. At 8hmz, that's 24 microseconds.
#define YM_RESET_WAIT 24 // uS
// The YM2612 clock is driven by a PWN pin
#define YM_CLOCK 4
/*
Ideal clock is 7.67, but it seems the teensy can't do that accurately.
Use 7.5, which is accurate & stable (verified by external tuner) & more
than close enough.
*/
#define YM_CLOCK_FREQ 7500000

/* Display & button constants */
// Display is on SPI.
#define DISPLAY_CS 10
#define DISPLAY_DC 9
#define DISPLAY_RESET 8
#ifndef DISPLAY_RATE
#define DISPLAY_RATE 66666 // 1/15th of a second in microseconds.
#endif
#ifndef U8G2_INITIALIZATION
#define U8G2_INITIALIZATION                                                                                            \
  U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(/* rotation=*/U8G2_R2, /* cs=*/DISPLAY_CS, /* dc=*/DISPLAY_DC,            \
                                             /* reset=*/DISPLAY_RESET);
#endif
// Buttons use descending pins from the first button's pin.
#define BUTTON_ONE 27

/* Synth core constants */
#define DEBUG_EEPROM_RW 0
#define EEPROM_PATCH_PRESENT_ADDR 0x09
#define EEPROM_PATCH_ADDR 0x10

#ifdef HARDWARE_CONSTANTS_OVERRIDE
#include "hardware_constants_override.h"
#endif

#endif

#include <arduino.h>
#include <util/delay.h> // For timing
#include "nanodelay.h"

#include "ym2612.h"

namespace thea {
namespace ym2612 {

// Pin 14-21 to YM DATA
#define YM_DATA 14
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
// Pin 28 to YM RD (unused)
#define YM_RD 30
// How long to hold the WR and CS lines when writing data.
// The datasheet indicates this should be *at least* 200nS, but it appears
// that a setting of (200) ~2,000uS ensures that the chip always gets our messages.
#define YM_WRITE_WAIT 11 // * 10uS
// How long to hold the data lines after pulling the WR and CS lines.
// The datasheet indicates this should be *at least* 20ns, but it appears
// that a setting of 100 (~1,000uS) ensures that the chip always gets our messages.
#define YM_DATA_WAIT 11 // * 10uS
// How long to hold the reset line.
// According to bus timing documents, we must hold the line low for at least
// 192 cycles. At 8hmz, that's 24 microseconds.
#define YM_RESET_WAIT 24 // uS


void setup() {
  /* Setup the YM's pins. */
  for (int i = 0; i < 8; i++) {
    pinMode(YM_DATA + i, OUTPUT);
    digitalWriteFast(YM_DATA + i, LOW);
  }
  pinMode(YM_IC, OUTPUT);
  pinMode(YM_A1, OUTPUT);
  pinMode(YM_A0, OUTPUT);
  pinMode(YM_WR, OUTPUT);
  pinMode(YM_CS, OUTPUT);
  pinMode(YM_RD, OUTPUT);
  digitalWriteFast(YM_IC, HIGH);
  digitalWriteFast(YM_A1, LOW);
  digitalWriteFast(YM_A0, LOW);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  digitalWriteFast(YM_RD, HIGH);
}

void reset() {
  /* Reset the YM's state. */
  digitalWriteFast(YM_IC, HIGH);
  delayMicroseconds(YM_RESET_WAIT);
  digitalWriteFast(YM_IC, LOW);
  delayMicroseconds(YM_RESET_WAIT);
  digitalWriteFast(YM_IC, HIGH);
  delayMicroseconds(YM_RESET_WAIT);
}


inline static void set_data_lines(byte b)
{
  /* Sets the data lines to the YM to the given byte. */
  for (int i = 0; i < 8; i++)
  {
    digitalWriteFast(YM_DATA + i, ((b >> i) & 1));
  }
}

void set_reg(uint8_t address, uint8_t data, int port) {
  // Write the register address first.
  digitalWriteFast(YM_A1, port);
  digitalWriteFast(YM_A0, LOW);
  set_data_lines(address);
  // technically we should give *some* time between setting the bytes and
  // pulling the pins, but it seems to work fine without that.
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  // We shouldn't raise the WR and CS until at least YM_WRITE_WAIT have passed.
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  // We must wait for the chip to finish reading our data before loading in the
  // next byte.
  delay10ns(YM_DATA_WAIT);

  // Same as above, but with A0 high to write the register data.
  digitalWriteFast(YM_A0, HIGH);
  set_data_lines(data);
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  delay10ns(YM_DATA_WAIT);
}

void set_reg(uint8_t address, uint8_t data){
  return set_reg(address, data, 0);
}

void ChannelPatch::write_to_channel(uint8_t channel) {
  // Setup operators.
  for(int i = 0; i < 4; i++) {
    uint8_t dt1_mul_byte = (operators[i].DT1 << 4) & (operators[i].MUL & 0xF);
    uint8_t tl_byte = operators[i].TL & 0x7F;
    uint8_t rs_ar_byte = (operators[i].RS << 6) | (operators[i].AR & 0x1F);
    uint8_t am_d1r_byte = (operators[i].AM << 7) | (operators[i].D1R & 0x1F);
    uint8_t d2r_byte = operators[i].D2R & 0x1F;
    uint8_t d1l_rr_byte = (operators[i].D1L << 4) | (operators[i].RR & 0xF);

    uint8_t operator_offset = 4 * i;
    set_reg(0x30 + operator_offset + channel, dt1_mul_byte);
    delay(1);
    set_reg(0x40 + operator_offset + channel, tl_byte);
    delay(1);
    set_reg(0x50 + operator_offset + channel, rs_ar_byte);
    delay(1);
    set_reg(0x60 + operator_offset + channel, am_d1r_byte);
    delay(1);
    set_reg(0x70 + operator_offset + channel, d2r_byte);
    delay(1);
    set_reg(0x80 + operator_offset + channel, d1l_rr_byte);
    delay(1);
  }

  // Setup channel
  uint8_t feedback_algorithm_byte = (feedback << 3) | (algorithm & 0x7);
  set_reg(0xB0 + channel, feedback_algorithm_byte);
  delay(1);
  set_reg(0xB4 + channel, 0xC0); // Enable output on both speakers (for now)
  delay(1);
};

void load_test_patch() {
  ChannelPatch test_patch;
  //MiOPMdrv sound bank Paramer Ver2002.04.22
  //LFO: LFRQ AMD PMD WF NFRQ
  //@:[Num] [Name]
  //CH: PAN   FL(feedback) CON(algorithm) AMS(?) PMS(Phase mod?) SLOT(?) NE(noise)
  //[OPname]: AR D1R D2R  RR D1L  TL  KS(RS) MUL DT1 DT2(ignored) AMS-EN(AM)

  // @:0 Instrument 0
  // LFO: 0 0 0 0 0
  // CH: 64 6 6 0 0 120 0
  // M1: 31 18 0 15 15 24 0 15 3 0 0
  // C1: 31 17 10 15 0 18 0 1 3 0 0
  // M2: 31 14 7 15 1 18 0 1 3 0 0
  // C2: 31 0 9 15 0 18 0 1 3 0 0
  test_patch.algorithm = 2;
  test_patch.feedback = 6;
  test_patch.operators[0].AR = 31;
  test_patch.operators[0].D1R = 18;
  test_patch.operators[0].D2R = 0;
  test_patch.operators[0].RR = 15;
  test_patch.operators[0].D1L = 15;
  test_patch.operators[0].TL = 24;
  test_patch.operators[0].RS = 0;
  test_patch.operators[0].MUL = 15;
  test_patch.operators[0].DT1 = 3;
  test_patch.operators[0].AM = 0;
  test_patch.operators[1].AR = 31;
  test_patch.operators[1].D1R = 17;
  test_patch.operators[1].D2R = 10;
  test_patch.operators[1].RR = 15;
  test_patch.operators[1].D1L = 0;
  test_patch.operators[1].TL = 18;
  test_patch.operators[1].RS = 0;
  test_patch.operators[1].MUL = 1;
  test_patch.operators[1].DT1 = 3;
  test_patch.operators[1].AM = 0;
  test_patch.operators[2].AR = 31;
  test_patch.operators[2].D1R = 14;
  test_patch.operators[2].D2R = 7;
  test_patch.operators[2].RR = 15;
  test_patch.operators[2].D1L = 1;
  test_patch.operators[2].TL = 18;
  test_patch.operators[2].RS = 0;
  test_patch.operators[2].MUL = 1;
  test_patch.operators[2].DT1 = 3;
  test_patch.operators[2].AM = 0;
  test_patch.operators[3].AR = 31;
  test_patch.operators[3].D1R = 0;
  test_patch.operators[3].D2R = 9;
  test_patch.operators[3].RR = 15;
  test_patch.operators[3].D1L = 0;
  test_patch.operators[3].TL = 18;
  test_patch.operators[3].RS = 0;
  test_patch.operators[3].MUL = 1;
  test_patch.operators[3].DT1 = 3;
  test_patch.operators[3].AM = 0;
  test_patch.write_to_channel(0);
  test_patch.write_to_channel(1);
  test_patch.write_to_channel(2);
}

void load_test_patch2() {
  ChannelPatch test_patch;
  //MiOPMdrv sound bank Paramer Ver2002.04.22
  //LFO: LFRQ AMD PMD WF NFRQ
  //@:[Num] [Name]
  //CH: PAN   FL(feedback) CON(algorithm) AMS(?) PMS(Phase mod?) SLOT(?) NE(noise)
  //[OPname]: AR D1R D2R  RR D1L  TL  KS(RS) MUL DT1 DT2(ignored) AMS-EN(AM)

  // @:1 Instrument 1
  // LFO: 0 0 0 0 0
  // CH: 64 6 5 0 0 120 0
  // M1: 16  7  1  8  2  29  0  2  3 0 0
  // C1: 10  4 10 11  1  22  0  4  3 0 0
  // M2: 12  4 10 11  1  21  0  2  3 0 0
  // C2: 14  4 10 11  1  22  0  1  3 0 0
  test_patch.feedback = 6;
  test_patch.algorithm = 5;
  test_patch.operators[0].AR = 16;
  test_patch.operators[0].D1R = 7;
  test_patch.operators[0].D2R = 1;
  test_patch.operators[0].RR = 8;
  test_patch.operators[0].D1L = 2;
  test_patch.operators[0].TL = 29;
  test_patch.operators[0].RS = 0;
  test_patch.operators[0].MUL = 2;
  test_patch.operators[0].DT1 = 3;
  test_patch.operators[0].AM = 0;
  test_patch.operators[1].AR = 10;
  test_patch.operators[1].D1R = 4;
  test_patch.operators[1].D2R = 10;
  test_patch.operators[1].RR = 11;
  test_patch.operators[1].D1L = 1;
  test_patch.operators[1].TL = 22;
  test_patch.operators[1].RS = 0;
  test_patch.operators[1].MUL = 4;
  test_patch.operators[1].DT1 = 3;
  test_patch.operators[1].AM = 0;
  test_patch.operators[2].AR = 12;
  test_patch.operators[2].D1R = 4;
  test_patch.operators[2].D2R = 10;
  test_patch.operators[2].RR = 11;
  test_patch.operators[2].D1L = 1;
  test_patch.operators[2].TL = 21;
  test_patch.operators[2].RS = 0;
  test_patch.operators[2].MUL = 2;
  test_patch.operators[2].DT1 = 3;
  test_patch.operators[2].AM = 0;
  test_patch.operators[3].AR = 14;
  test_patch.operators[3].D1R = 4;
  test_patch.operators[3].D2R = 10;
  test_patch.operators[3].RR = 11;
  test_patch.operators[3].D1L = 1;
  test_patch.operators[3].TL = 22;
  test_patch.operators[3].RS = 0;
  test_patch.operators[3].MUL = 1;
  test_patch.operators[3].DT1 = 3;
  test_patch.operators[3].AM = 0;
  test_patch.write_to_channel(0);
  test_patch.write_to_channel(1);
  test_patch.write_to_channel(2);
}

}; //namespace ym2612
}; //namespace thea
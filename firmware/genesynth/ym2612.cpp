#include <Arduino.h>

#include "hardware_constants.h"
#include "nanodelay.h"
#include "ym2612.h"

namespace thea {
namespace ym2612 {

LatencyInfo latency;

void initialize_clock() {
  /* Uses PWM to generate clock for the YM2612 */
  pinMode(YM_CLOCK, OUTPUT);
  analogWriteFrequency(YM_CLOCK, YM_CLOCK_FREQ);
  analogWrite(YM_CLOCK, 128);
  delay(50); // wait a moment for the clock.
}

void setup() {
  /* Setup the YM's pins. */
  for (int i = 0; i < 8; i++) {
    pinMode(YM_DATA - i, OUTPUT);
    digitalWriteFast(YM_DATA - i, LOW);
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

  initialize_clock();
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

inline static void input_enable() {
  for (int i = 0; i < 8; i++) {
    pinMode(YM_DATA - i, INPUT);
  }
}

inline static void output_enable() {
  for (int i = 0; i < 8; i++) {
    pinMode(YM_DATA - i, OUTPUT);
  }
}

inline static uint8_t read_data_lines() {
  uint8_t out = 0;
  for (int i = 0; i < 8; i++) {
    out |= digitalReadFast(YM_DATA - i) << i;
  }
  return out;
}

inline static void set_data_lines(byte b) {
  /* Sets the data lines to the YM to the given byte. */
  for (int i = 0; i < 8; i++) {
    digitalWriteFast(YM_DATA - i, ((b >> i) & 1));
  }
}

const LatencyInfo &get_latency() { return latency; }

inline static void wait_ready() {
  // Always wait a few nano seconds before toggling pin directions.
  delay10ns(YM_WRITE_WAIT);

  // Switch data bus to input YM -> uC.
  input_enable();

  // Switch A0 to inactive to read the data byte, pull CS and RD low again.
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_A1, LOW);
  digitalWriteFast(YM_A0, LOW);
  digitalWriteFast(YM_RD, LOW);
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_CS, LOW);
  delay10ns(YM_WRITE_WAIT);

  // Wait until the bus' 7th bit is 0.
  int count = 0;
  uint8_t state = 0;
  while (count < YM_MAX_WAIT_CYCLES) {
    __asm__ volatile("nop");
    count++;
    // Wait at *least* 20 cycles for the pins to read accurately.
    if (count < 20)
      continue;
    state = read_data_lines();
    if ((state & (1 << 7)) == 0)
      break;
  }

  // Disable the chip and read pins.
  digitalWriteFast(YM_RD, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  delay10ns(YM_WRITE_WAIT);

  // Switch data bus back to ouput. uC -> YM.
  output_enable();

  // Debug
  if (count == YM_MAX_WAIT_CYCLES) {
    Serial.printf("Warning, waited too many cycles for ready, last state: %x, %x, %i.\n", state, (1 << 7),
                  state & (1 << 7));
    latency.hit_max_wait_cycles = true;
  }
}

void set_reg(uint8_t address, uint8_t data, int port) {
  auto start = micros();

  // Wait for any previous writes to finish before writing.
  wait_ready();

  // Write the register address first.
  digitalWriteFast(YM_A1, port);
  digitalWriteFast(YM_A0, LOW);
  set_data_lines(address);

  // technically we should give *some* time between setting the bytes and
  // pulling the pins, but it seems to work fine without that.
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);

  // We must wait for the chip to finish reading our address before loading in the
  // next byte.
  // We shouldn't raise the WR and CS until at least YM_WRITE_WAIT have passed.
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);

  // Same as above, but with A0 high to write the register data.
  digitalWriteFast(YM_A0, HIGH);
  set_data_lines(data);
  delay10ns(YM_WRITE_WAIT);
  digitalWriteFast(YM_CS, LOW);
  digitalWriteFast(YM_WR, LOW);
  delay10ns(YM_WRITE_WAIT);

  // Disable the chip and write pins.
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_CS, HIGH);
  delay10ns(YM_WRITE_WAIT);

  // Update latency calculations
  latency.last = micros() - start;
  if (latency.max < latency.last)
    latency.max = latency.last;
  latency.average = (latency.average_alpha * latency.last) + (1.f - latency.average_alpha) * latency.average;
  latency.bytes_written++;
}

void set_reg(uint8_t address, uint8_t data) { return set_reg(address, data, 0); }

void set_channel_freq(int channel, float freq) {
  // From YM2612 Datasheet:
  // Freq Number = (144 * note * 2^20 / Clock) / 2^(block-1)
  // (change to 2^20, because it seemed to be setting the wrong octaive).
  // Figure out the base frequency number first, then start iterating through
  // blocks until it's within range 0 < freq_int < 2000
  float base_freq = 144.f * freq * pow(2.f, 19) / 7500000.f;
  uint32_t freq_int = 2001;

  int block = 1;
  while (freq_int > 2000) {
    freq_int = base_freq / pow(2.f, block - 1);
    block++;
  }

  int port = channel < 3 ? 0 : 1;
  uint8_t channel_offset = (channel % 3);
  set_reg(0xA4 + channel_offset, (block << 3) | (freq_int >> 8), port);
  set_reg(0xA0 + channel_offset, freq_int & 0xFF, port);
}

void play_note(int channel) {
  int port = channel < 3 ? 0 : 1;
  uint8_t channel_offset = (channel % 3);
  uint8_t key_offset = channel_offset | (port << 2);
  set_reg(0x28, 0xF0 | key_offset);
}

void stop_note(int channel) {
  int port = channel < 3 ? 0 : 1;
  uint8_t channel_offset = (channel % 3);
  uint8_t key_offset = channel_offset | (port << 2);
  set_reg(0x28, key_offset);
}

void set_lfo(bool enable, uint8_t freq) {
  if (freq > 7)
    freq = 7; // 3 bits (0-7)
  uint8_t lfo_reg = (enable << 3) | (freq);
  set_reg(0x22, lfo_reg);
}

void ChannelPatch::write_to_channel(uint8_t channel, ChannelPatch::WriteOption option) {
  int port = channel < 3 ? 0 : 1;
  channel = channel % 3;

  // Setup operators.
  // Note: this presently updates all of the operators even if only one operator option is
  // specified in the write option.

  // wrap the write option around if its an operator specific option.
  if (option >= WriteOption::OP1_DT1 and option <= WriteOption::OP3_AM) {
    option = WriteOption(option % 10);
  }

  for (int i = 0; i < 4; i++) {
    uint8_t dt1_mul_byte = (operators[i].DT1 << 4) | (operators[i].MUL & 0xF);
    uint8_t tl_byte = operators[i].TL & 0x7F;
    uint8_t rs_ar_byte = (operators[i].RS << 6) | (operators[i].AR & 0x1F);
    uint8_t am_d1r_byte = (operators[i].AM << 7) | (operators[i].D1R & 0x1F);
    uint8_t d2r_byte = operators[i].D2R & 0x1F;
    uint8_t d1l_rr_byte = (operators[i].D1L << 4) | (operators[i].RR & 0xF);

    uint8_t operator_offset = 4 * i;

    if (option == WriteOption::ALL or option == WriteOption::OP0_DT1 or option == WriteOption::OP0_MUL) {
      set_reg(0x30 + operator_offset + channel, dt1_mul_byte, port);
    }

    if (option == WriteOption::ALL or option == WriteOption::OP0_TL) {
      set_reg(0x40 + operator_offset + channel, tl_byte, port);
    }

    if (option == WriteOption::ALL or option == WriteOption::OP0_RS or option == WriteOption::OP0_AR) {
      set_reg(0x50 + operator_offset + channel, rs_ar_byte, port);
    }

    if (option == WriteOption::ALL or option == WriteOption::OP0_AM or option == WriteOption::OP0_D1R) {
      set_reg(0x60 + operator_offset + channel, am_d1r_byte, port);
    }

    if (option == WriteOption::ALL or option == WriteOption::OP0_D2R) {
      set_reg(0x70 + operator_offset + channel, d2r_byte, port);
    }

    if (option == WriteOption::ALL or option == WriteOption::OP0_D1L or option == WriteOption::OP0_RR) {
      set_reg(0x80 + operator_offset + channel, d1l_rr_byte, port);
    }
  }

  // Setup channel
  uint8_t feedback_algorithm_byte = (feedback << 3) | (algorithm & 0x7);

  if (option == WriteOption::ALL or option == WriteOption::FEEDBACK or option == WriteOption::ALGORITHM) {
    set_reg(0xB0 + channel, feedback_algorithm_byte, port);
  }

  if (option == WriteOption::ALL or option == WriteOption::LFO) {
    // Enable output on both speakers (for now) with 0xC0
    uint8_t lr_ams_fms_byte = 0xC0 | (lfo_ams << 5) | (lfo_fms);
    set_reg(0xB4 + channel, lr_ams_fms_byte, port);
  }
};

void load_test_patch() {
  ChannelPatch test_patch;
  // MiOPMdrv sound bank Paramer Ver2002.04.22
  // LFO: LFRQ AMD PMD WF NFRQ
  //@:[Num] [Name]
  // CH: PAN   FL(feedback) CON(algorithm) AMS(?) PMS(Phase mod?) SLOT(?) NE(noise)
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
  // MiOPMdrv sound bank Paramer Ver2002.04.22
  // LFO: LFRQ AMD PMD WF NFRQ
  //@:[Num] [Name]
  // CH: PAN   FL(feedback) CON(algorithm) AMS(?) PMS(Phase mod?) SLOT(?) NE(noise)
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

}; // namespace ym2612
}; // namespace thea

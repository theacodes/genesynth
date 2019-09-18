#include <Arduino.h>

#include "hardware_constants.h"
#include "nanodelay.h"
#include "ym2612.h"

namespace thea {
namespace ym2612 {

/* Global state */

struct WriteCommand {
  uint8_t address = 0;
  uint8_t data = 0;
  uint8_t port = 0;
};

// 200 is a good size here, as it takes about 150 commands to write a full
// patch.
WriteCommand write_buffer[200];
const size_t write_buffer_size = sizeof(write_buffer) / sizeof(WriteCommand);
size_t write_buffer_len = 0;
size_t write_buffer_start = 0;
size_t write_buffer_end = 0;

LatencyInfo latency;

/* Low-level functions */

static void initialize_clock() {
  /* Uses PWM to generate clock for the YM2612 */
  pinMode(YM_CLOCK, OUTPUT);
  analogWriteFrequency(YM_CLOCK, YM_CLOCK_FREQ);
  analogWrite(YM_CLOCK, 128);
  delay(50); // wait a moment for the clock.
}

static void reset() {
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

static void write_out_reg(uint8_t address, uint8_t data, uint8_t port) {
  // Serial.printf("Writing %02x to %02x on %i\n", data, address, port);
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

/* Top-level functions */

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
  reset();
}

void loop() {
  if (!write_buffer_len)
    return;
  // Serial.printf("There are %u commands to send.\n", write_buffer_len);
  while (write_buffer_len) {
    // Serial.printf("start: %u, end: %u, len: %u\n", write_buffer_start, write_buffer_end, write_buffer_len);
    auto command = write_buffer[write_buffer_start];
    write_out_reg(command.address, command.data, command.port);
    write_buffer_start = (write_buffer_start + 1) % write_buffer_size;
    write_buffer_len--;
  }
}

const LatencyInfo &get_latency() { return latency; }

void set_reg(uint8_t address, uint8_t data, uint8_t port) {
  /* Queues a write command into the command buffer.*/

  // Is this one of the parameter modifications? If so, de-duplicate the
  // command to prevent overwhelming the ym2612.
  if (address >= 0x30 && address <= 0x90) {
    // step through the whole buffer and see if there's already a command
    // queued. If so, modify it instead of inserting this.
    for (auto i = 0; i < write_buffer_len; i++) {
      auto &command = write_buffer[(write_buffer_start + i) % write_buffer_size];
      if (command.address == address && command.port == port) {
        // Serial.printf("Deduplicated a write to %02x:%u old: %02x new: %02x\n", address, port, command.data, data);
        command.data = data;
        return;
      }
    }
  }

  // Otherwise, insert the new command.
  write_buffer[write_buffer_end].address = address;
  write_buffer[write_buffer_end].data = data;
  write_buffer[write_buffer_end].port = port;
  write_buffer_end = (write_buffer_end + 1) % write_buffer_size;
  write_buffer_len++;
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

/* Channel/OperatorPatch implementations */

static void write_operator_values(int port, int channel, int operator_num, OperatorPatch &oper,
                                  ChannelPatch::WriteOption option) {
  // wrap the write option around if its an operator specific option in *this* operator's range.
  if (option >= 10 * operator_num && option <= 10 * (operator_num + 1)) {
    option = ChannelPatch::WriteOption(option % 10);
  }
  // If this wasn't an operator-specific option and also not a option to write
  // All params, then there's nothing to do here.
  else if (option != ChannelPatch::WriteOption::ALL) {
    return;
  }

  uint8_t operator_offset = 4 * operator_num;

  uint8_t dt1_mul_byte = (oper.DT1 << 4) | (oper.MUL & 0xF);
  uint8_t tl_byte = oper.TL & 0x7F;
  uint8_t rs_ar_byte = (oper.RS << 6) | (oper.AR & 0x1F);
  uint8_t am_d1r_byte = (oper.AM << 7) | (oper.D1R & 0x1F);
  uint8_t d2r_byte = oper.D2R & 0x1F;
  uint8_t d1l_rr_byte = (oper.D1L << 4) | (oper.RR & 0xF);

  if (option == ChannelPatch::WriteOption::ALL or option == ChannelPatch::WriteOption::OP0_DT1 or
      option == ChannelPatch::WriteOption::OP0_MUL) {
    set_reg(0x30 + operator_offset + channel, dt1_mul_byte, port);
  }

  if (option == ChannelPatch::WriteOption::ALL or option == ChannelPatch::WriteOption::OP0_TL) {
    set_reg(0x40 + operator_offset + channel, tl_byte, port);
  }

  if (option == ChannelPatch::WriteOption::ALL or option == ChannelPatch::WriteOption::OP0_RS or
      option == ChannelPatch::WriteOption::OP0_AR) {
    set_reg(0x50 + operator_offset + channel, rs_ar_byte, port);
  }

  if (option == ChannelPatch::WriteOption::ALL or option == ChannelPatch::WriteOption::OP0_AM or
      option == ChannelPatch::WriteOption::OP0_D1R) {
    set_reg(0x60 + operator_offset + channel, am_d1r_byte, port);
  }

  if (option == ChannelPatch::WriteOption::ALL or option == ChannelPatch::WriteOption::OP0_D2R) {
    set_reg(0x70 + operator_offset + channel, d2r_byte, port);
  }

  if (option == ChannelPatch::WriteOption::ALL or option == ChannelPatch::WriteOption::OP0_D1L or
      option == ChannelPatch::WriteOption::OP0_RR) {
    set_reg(0x80 + operator_offset + channel, d1l_rr_byte, port);
  }
};

void ChannelPatch::write_to_channel(uint8_t channel, ChannelPatch::WriteOption option) {
  int port = channel < 3 ? 0 : 1;
  channel = channel % 3;

  for (int i = 0; i < 4; i++) {
    write_operator_values(port, channel, i, operators[i], option);
  }

  // Setup channel
  uint8_t feedback_algorithm_byte = (feedback << 3) | (algorithm & 0x7);

  if (option == WriteOption::ALL or option == WriteOption::FEEDBACK or option == WriteOption::ALGORITHM) {
    set_reg(0xB0 + channel, feedback_algorithm_byte, port);
  }

  if (option == WriteOption::ALL || option == WriteOption::LFO_AMS || option == WriteOption::LFO_FMS) {
    // Enable output on both speakers (for now) with 0xC0
    uint8_t lr_ams_fms_byte = 0xC0 | (lfo_ams << 5) | (lfo_fms);
    set_reg(0xB4 + channel, lr_ams_fms_byte, port);
  }
};

void ChannelPatch::set_parameter(ChannelPatch::WriteOption option, uint8_t value, bool normalized) {
  if (normalized)
    value = normalized_to_ym2612(value, option);

  /* Global parameters */
  switch (option) {
  case WriteOption::ALGORITHM:
    algorithm = value;
    return;
  case WriteOption::FEEDBACK:
    feedback = value;
    return;
  case WriteOption::LFO_FMS:
    lfo_fms = value;
    return;
  case WriteOption::LFO_AMS:
    lfo_ams = value;
    return;
  default:
    break;
  }

  /* Operator parameters */
  if (option > thea::ym2612::ChannelPatch::WriteOption::OP3_AM) {
    /* Unknown option */
    return;
  }

  auto normalized_option = thea::ym2612::ChannelPatch::WriteOption(option % 10);
  auto operator_no = option / 10;

  switch (normalized_option) {
  case WriteOption::OP0_DT1:
    operators[operator_no].DT1 = value;
    break;
  case WriteOption::OP0_MUL:
    operators[operator_no].MUL = value;
    break;
  case WriteOption::OP0_TL:
    operators[operator_no].TL = value;
    break;
  case WriteOption::OP0_AR:
    operators[operator_no].AR = value;
    break;
  case WriteOption::OP0_D1R:
    operators[operator_no].D1R = value;
    break;
  case WriteOption::OP0_D2R:
    operators[operator_no].D2R = value;
    break;
  case WriteOption::OP0_D1L:
    operators[operator_no].D1L = value;
    break;
  case WriteOption::OP0_RR:
    operators[operator_no].RR = value;
    break;
  case WriteOption::OP0_RS:
    operators[operator_no].RS = value;
    break;
  case WriteOption::OP0_AM:
    operators[operator_no].AM = value;
    break;
  default:
    break;
  }
}

/* Normalization / denormalization routines */

uint8_t ym2612_to_normalized(uint8_t value, ChannelPatch::WriteOption option) {
  // TODO
  return 0;
}

uint8_t normalized_to_ym2612(uint8_t value, ChannelPatch::WriteOption option) {
  // Is this a global parameter?
  switch (option) {
  case ChannelPatch::WriteOption::ALGORITHM:
    return map(value, 0, 127, 0, 7);
  case ChannelPatch::WriteOption::FEEDBACK:
    return map(value, 0, 127, 0, 7);
  case ChannelPatch::WriteOption::LFO_FMS:
    return map(value, 0, 127, 0, 7);
  case ChannelPatch::WriteOption::LFO_AMS:
    return map(value, 0, 127, 0, 3);
  default:
    break;
  }

  // This is an operator value.
  auto normalized_option = thea::ym2612::ChannelPatch::WriteOption(option % 10);
  uint8_t operator_no = option / 10;

  // All operator parameters are actually inverted - the lower values are
  // associated with what humans would think are "higher" knob values. That is
  // to say, if the parameter were "volume", 0 would be the loudest and 127
  // would be the softest. No frickin' clue why Yamaha did this.
  value = 127 - value;

  switch (normalized_option) {
  case thea::ym2612::ChannelPatch::WriteOption::OP0_DT1:
    return map(value, 0, 127, 0, 7);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_MUL:
    return map(value, 0, 127, 0, 15);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_TL:
    return value;
  case thea::ym2612::ChannelPatch::WriteOption::OP0_AR:
    return map(value, 0, 127, 0, 31);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D1R:
    return map(value, 0, 127, 0, 31);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D2R:
    return map(value, 0, 127, 0, 31);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_D1L:
    return map(value, 0, 127, 0, 15);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_RR:
    return map(value, 0, 127, 0, 15);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_RS:
    return map(value, 0, 127, 0, 3);
  case thea::ym2612::ChannelPatch::WriteOption::OP0_AM:
    return value;
  default:
    return 0;
  }

  return 0;
}

}; // namespace ym2612
}; // namespace thea

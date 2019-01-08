#ifndef THEA_YM2612_H
#define THEA_YM2612_H

#include <stdint.h>

namespace thea {
namespace ym2612 {

#define MAX_PATCH_NAME_SIZE 32

struct LatencyInfo {
  float average = 0.f;
  float average_alpha = 2.f / (1000.f + 1.f);
  int last = 0;
  int max = 0;
  bool hit_max_wait_cycles = false;
  unsigned long bytes_written = 0;
};

void setup();
void reset();
const LatencyInfo &get_latency();
void set_reg(uint8_t address, uint8_t data, int port);
void set_reg(uint8_t address, uint8_t data);
void play_note(int channel);
void stop_note(int channel);
void set_channel_freq(int channel, float freq);

class OperatorPatch {
public:
  // DT1 gives small variations from the overall frequency × MUL. The MSB of
  // DT1 is a primitive sign bit, and the two LSBs are magnitude bits.
  // 0-7 (3 bits).
  uint8_t DT1;

  // MUL ranges from 0 to 15, and multiplies the overall frequency,
  // with the exception that 0 results in multiplication by 1/2. That is,
  // MUL=0 to 15 gives ×1/2, ×1, ×2, ... ×15.
  // 0-15 (4 bits)
  uint8_t MUL;

  // Envelope specification
  // Total level, the highest amplitude of the waveform.
  // 0-127 (7 bits)
  uint8_t TL;
  // Attack rate, the angle of initial amplitude increase.
  // This can be made very steep if desired. The problem with slow
  // attack rates is that if the notes are short, the release (called
  // ‘key off’) occurs before the note has reached a reasonable level.
  // 0-31 (5 bits)
  uint8_t AR;
  // The angle of initial amplitude decrease.
  // 0-31 (5 bits)
  uint8_t D1R;
  // The angle of secondary amplitude decrease. This will continue indefinitely
  // unless ‘key off’ occurs.
  // 0-31 (5 bits)
  uint8_t D2R;
  // D1L is the secondary amplitude reached after the first period of rapid
  // decay. It should be multiplied by 8 if one wishes to compare it to TL.
  // Again as TL, the higher the number, the more attenuated the sound.
  uint8_t D1L;
  // The final angle of amplitude decrease, after ‘key off’.
  // 0-15 (4 bits)
  uint8_t RR;

  // Rate scaling, the degree to which envelopes become shorter as frequencies become higher. For example, high notes
  // on a piano fade much more quickly than low notes.
  // 0-3 (2 bits)
  uint8_t RS;

  // Amplitude Modulation enable, whether or not this operator will allow
  // itself to be modified by the LFO. Changing the amplitude of the slots
  // changes the loudness of the note; changing the amplitude of the other
  // operators changes its flavor.
  bool AM = false;
};

class ChannelPatch {
public:
  enum WriteOption {
    OP0_DT1,
    OP0_MUL,
    OP0_TL,
    OP0_AR,
    OP0_D1R,
    OP0_D2R,
    OP0_D1L,
    OP0_RR,
    OP0_RS,
    OP0_AM,

    OP1_DT1,
    OP1_MUL,
    OP1_TL,
    OP1_AR,
    OP1_D1R,
    OP1_D2R,
    OP1_D1L,
    OP1_RR,
    OP1_RS,
    OP1_AM,

    OP2_DT1,
    OP2_MUL,
    OP2_TL,
    OP2_AR,
    OP2_D1R,
    OP2_D2R,
    OP2_D1L,
    OP2_RR,
    OP2_RS,
    OP2_AM,

    OP3_DT1,
    OP3_MUL,
    OP3_TL,
    OP3_AR,
    OP3_D1R,
    OP3_D2R,
    OP3_D1L,
    OP3_RR,
    OP3_RS,
    OP3_AM,

    ALGORITHM,
    FEEDBACK,
    LFO,
    ALL,
  };

  char bank[MAX_PATCH_NAME_SIZE];
  char name[MAX_PATCH_NAME_SIZE];

  // 0-7 (3 bits).
  uint8_t algorithm;
  // Feedback is the degree to which operator 1 feeds back into itself.
  // 0-7 (3 bits).
  uint8_t feedback;
  // ???
  uint8_t lfo;
  OperatorPatch operators[4];

  void write_to_channel(uint8_t channel, WriteOption option);
  void write_to_channel(uint8_t channel) { write_to_channel(channel, WriteOption::ALL); }
};

void load_test_patch();
void load_test_patch2();

}; // namespace ym2612
}; // namespace thea

#endif

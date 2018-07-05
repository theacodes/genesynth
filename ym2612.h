#ifndef THEA_YM2612_H
#define THEA_YM2612_H

#include <stdint.h>

namespace thea {
namespace ym2612 {

void setup();
void reset();
void set_reg(uint8_t address, uint8_t data, int port);
void set_reg(uint8_t address, uint8_t data);


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
  //unless ‘key off’ occurs.
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
  // 0-7 (3 bits).
  uint8_t algorithm;
  // Feedback is the degree to which operator 1 feeds back into itself.
  // 0-7 (3 bits).
  uint8_t feedback;
  // ???
  uint8_t lfo;
  OperatorPatch operators[4];

  void write_to_channel(uint8_t channel);
};

void load_test_patch();
void load_test_patch2();

}; //namespace ym2612
}; //namespace thea

#endif
#ifndef THEA_OPN_PARSER_H
#define THEA_OPN_PARSER_H

#include "ym2612.h"

/* OPN Patch parser */
/* OPN Patch format:

  //MiOPMdrv sound bank Paramer Ver2002.04.22
  //LFO: LFRQ AMD PMD WF NFRQ
  //@:[Num] [Name]
  //CH: PAN   FL(feedback) CON(algorithm) AMS(?) PMS(Phase mod?) SLOT(?) NE(noise)
  //[OPname]: AR D1R D2R  RR D1L  TL  KS(RS) MUL DT1 DT2(ignored) AMS-EN(AM)

  @:0 Instrument 0
  LFO: 0 0 0 0 0
  CH: 64 6 6 0 0 120 0
  M1: 31 18 0 15 15 24 0 15 3 0 0
  C1: 31 17 10 15 0 18 0 1 3 0 0
  M2: 31 14 7 15 1 18 0 1 3 0 0
  C2: 31 0 9 15 0 18 0 1 3 0 0
*/

namespace thea {
namespace opn {

thea::ym2612::ChannelPatch parse();

} // namespace opn
} // namespace thea

#endif
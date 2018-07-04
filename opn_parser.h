#pragma once

#include <cstring>
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

#include "SdFat.h"


SdFatSdio parser_sd;

thea::ym2612::ChannelPatch parse_opn_patch() {
    thea::ym2612::ChannelPatch patch;
    char colon;
    int skipint;

    parser_sd.begin();
    ifstream file("patch.opn");

    if(!file.is_open()) {
        Serial.println("Bad");
        return patch;
    }

    while(!file.eof()) {
        char first = file.peek();

        // Ignore comments and blank lines.
        if(first == '/' || first == '\n' || first == ' '){
            file.ignore(65535 , '\n');
            continue;
        }

        // Also ignore the instrument name for now (will update later)
        if(first == '@'){
            file.ignore(65535 , '\n');
            continue;
        }

        // Get the register name
        char reggie[4];

        file.get(reggie, sizeof(reggie), ':');

        if(strncmp(reggie, "LFO", 3) == 0){
            // ignore lfo for now.
            file.ignore(65535 , '\n');
            continue;
        }

        if(strncmp(reggie, "CH", 2) == 0){
            //PAN   FL(feedback) CON(algorithm) AMS(Amp mod) PMS(Phase mod?) SLOT(?) NE(noise)
            file >> colon
                >> skipint
                >> patch.feedback
                >> patch.algorithm
                >> skipint
                >> skipint
                >> skipint
                >> skipint;
            continue;
        }

        int oper = -1;
        if(strncmp(reggie, "M1", 2) == 0){
            oper = 0;
        }
        if(strncmp(reggie, "C1", 2) == 0){
            oper = 1;
        }
        if(strncmp(reggie, "M2", 2) == 0){
            oper = 2;
        }
        if(strncmp(reggie, "C2", 2) == 0){
            oper = 3;
        }

        if(oper == -1){
            file.ignore(65535 , '\n');
            continue;
        }

        //[OPname]: AR D1R D2R  RR D1L  TL  KS(RS) MUL DT1 DT2(ignored) AMS-EN(AM)
        int ar = 0, d1r = 0, d2r = 0, rr = 0, d1l = 0, tl = 0, rs = 0, mul = 0, dt1 = 0, am = 0;

        // Can't read directly into operator values because they're characters,
        // not integers.
        file >> colon
            >> ar
            >> d1r
            >> d2r
            >> rr
            >> d1l
            >> tl
            >> rs
            >> mul
            >> dt1
            >> skipint
            >> am;

        patch.operators[oper].AR = ar;
        patch.operators[oper].D1R = d1r;
        patch.operators[oper].D2R = d2r;
        patch.operators[oper].RR = rr;
        patch.operators[oper].D1L = d1l;
        patch.operators[oper].TL = tl;
        patch.operators[oper].RS = rs;
        patch.operators[oper].MUL = mul;
        patch.operators[oper].DT1 = dt1;
        patch.operators[oper].AM = am;

        Serial.printf("%i %i %i %i %i %i %i %i %i %i %i",
            oper,
            patch.operators[oper].AR,
            patch.operators[oper].D1R,
            patch.operators[oper].D2R,
            patch.operators[oper].RR,
            patch.operators[oper].D1L,
            patch.operators[oper].TL,
            patch.operators[oper].RS,
            patch.operators[oper].MUL,
            patch.operators[oper].DT1,
            patch.operators[oper].AM);
    }

    return patch;
}

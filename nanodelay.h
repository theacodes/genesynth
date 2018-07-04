#ifndef THEA_NANODELAY_H
#define THEA_NANODELAY_H

//10 ns of delay * multiplier. Teensy 3.5 specific value due to clock speed. Adjust as needed.
inline void delay10ns(int multiplier) {
  /* Delays ~10ns * multipier.
  A single "nop" instruction should take 1 cycle
  */
  int i = multiplier;
  while (i-- > 0)
    __asm__ volatile ("nop"); //Approx. 10 ns of delay
}

#endif
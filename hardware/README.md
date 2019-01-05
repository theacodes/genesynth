# Genesynth Hardware

The hardware consists of three major components: the motherboard, the audio amplifier, and the display board.

## Motherboard

The motherboard contains:

* The Teensy 3.5. The 3.5 was chosen due to its 5V operating voltage for ease of compatibility with the YM2612. A 3.6 could be used if logic-level converters were added.
* A pair of LDO Voltage Regulators. These take 9V power and step it down to independent 5V supplies for digital and analog components. Since this powers the Teensy, the Teensy's USB voltage lines *must be cut*.
* The YM2612 itself. This chip should be socketed in case of a bad or damaged chip.

## Audio Amplifier

The audio amplifier is a relatively straightforward summing amplifier and buffer using the TL074 and is modeled after the Mega Amp mod for the Sega Genesis. This lets the Genesynth match the sound of the original hardware fairly closely. It's designed using through-hole components because that is what I'm most comfortable with at this time, but could easily be done using surface-mount. The audio board is powered by a dedicated analog 5V line provided by one of the motherboard's LDOs to completely eliminate noise.

## Display board

The display board holds a simple OLED display module and provides 4 tac switches for displaying and navigating the menu.

# Genesynth

**Genesynth** is a music synthesizer module based around the Yamaha YM2612 FM Synthesis Chip, the same chip used in the Sega Genesis / Mega Drive.

## Why?

Because I wanted to. :)

## How?

This project is designed to use the Teensy 3.5 to control the YM2612 via USB MIDI.

More details for the hardware is available under the hardware folder, and more details on the firmware is under the firmware folder.

## License

This project is under the Apache License, Version 2.0. See LICENSE for more details.

If you use anything in this for a project, please let me know! I'd love to hear what you do with it. :)

# Building your own

This repository contains everything you'll need to build your own version of this synthesizer.

## Parts

You'll need to order all of the components in the three separate hardware modules under the `hardware` directory. You can use KiCAD to generate a BOM. These parts are readily available from Mouser and other distributors with the exception of the YM2612 chip - but this is easily obtainable on ebay.

Also note that you *must* use a Teensy 3.5. The Teensy 3.6 will *not* work as it's 3.3v logic and this design uses 5v logic.

You'll also need a 9v center-positive power supply.

## Ordering the PCBs

The gerbers for the PCBs are already created and are in the respective subdirectories under `hardware`. These are designed to fit within OSHPark's design rules, but I actually used PCBWay for my boards.

## Soldering components

You should *absolutely* use a socket for the YM2612 and the Teensy. Otherwise, this is all standard through-hole construction. Solder the smaller parts first followed by the larger parts.

## Connecting the boards.

There's three boards that make up the synth - the motherboard, a display board, and an audio board. The audio board connects to the *right* side of the motherboard, the display board to the *left*. You can connect these with wires or use pin headers and dupont connectors.

## Connect audio output.

The audo board has mono line out and stereo headphone out. You'll need to wire up the appropriate jack to the *right* side of the audio board.

## Assembling the case.

There's SVGs for creating a laser-cut case under `case`. You should be able to do this at any makerspace or using Ponoko. The case simply snaps together and the boards are mounted with standard M2 and M3 hardware.

## Preparing and programming the Teensy

Before powering on the synthesizer, **be sure to cut the USB power traces on the underside of the Teensy**. If you don't do this, **you will fry your board**. Once connected, you can build and upload the `firmware` using Teensyduino.

## Place patches on the SD card.

The synth needs patches on the SD card. Patches are in TFI format and many patch libraries are readily available. You should put patches into subdirectories of the SD card.

## All done!

You should now be able to boot the synthesizer, load patches, and play notes over MIDI. Have fun!

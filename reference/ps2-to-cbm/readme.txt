Commodore to PS/2 keyboard & mouse adapter


0.0 Introduction

The Commodore to PS/2 keyboard & mouse adapter will let you use a regular PS/2 keyboard and PS/2 mouse on your Commodre 64 or VIC20. The keyboard is emulated using symbolic (not positional) mapping and the mouse emulates a 1531 mouse in proportional mode. 

0.1 Quick start guide

For those who don't like reading long documents, here is what you need:
- get all the parts listed in the "parts" section below
- assemble the circuit as per the schematic
- program the MCU
- connect C64, mouse, and keyboard


1.0 Parts

The following parts are required:

- Microcontroller PIC18F4520-I/P (40 pin DIP)
- Crosspoint switch MT8812AP1 (or MT8812AP)
- 10 MHz crystal
- 20-pin 0.100" header
- DE-9F connector 
- 6-pin min-DIN connector (qty 2)
- 220 ohm resistors (qty 15 or qty 2 8-way network)
- 180 ohm resistors (qty 16 or qty 2 8-way network)
- 150 ohm resistors (qty 3) (optional)
- 4.7 Kohm resistor (qty 4)
- 10 Kohm resistor
- LED (qty 3) (optional)
- 0.1 uF capacitor (qty 3)
- RCA connectors (qty 3) (optional)


2.0 Operation

2.1 Keyboard emulation
Almost all the PS/2 keys correspond to their C64 equivalents, so F1 is F1, CTRL is CTRL, etc.
The following five keys don't exist on the PS/2 keyboard so here are the substiutes:

C64 pound key      ==> PS/2 backslash  (\)
C64 left arrow key ==> PS/2 left quote (`)
C64 restore key	   ==> PS/2 pause/break
C64 run/stop key   ==> PS/2 esc
C64 commodore key  ==> PS/2 Windows key

Also, since three keys (@ ^ and +) require using the shift key, they themselves can't be combined with the shift key. This will only pose a problem if you're doing keyboard graphics (in which case a real C64 keyboard is probably more useful anyway since a PS/2 keyboard doesn't have the symbols on the front of the keys).

All the remaining keys on the PS/2 keyboard that don't map to a C64 equivalent will have no effect when pressed.

Caps Lock also has no effect since the typical function of a caps locks key is to only type uppercase letters, whereas the C64 "shift lock" key causes EVERY key to be shifted.

2.2 Mouse emulation

The mouse emulation simulates a 1531 mouse in proportional mode and is compatible with the 1531 driver for Geos. There is no emulation of a 1530 joystick-type mouse or of the 1531 in joystick mode.

2.3 Special functions

A few extra features are available:
If you have a Super Snapshot cartridge, connect the button to pin 2 of the CPU and pressing F12 will push the snapshot button.

If you have an A/B switch for your joystick, connect the control line to pin 23 of the CPU and pressing num lock will toggle between joystick 1 and joystick 2.

If you want to be able to reset your C64 from the keyboard, connect pin 24 of the CPU to the input of the 556 chip inside the C64 and pressing ctrl-alt-del will reset the C64.


3.0 Modifications

To compile the project, you will need the CCS compiler (www.ccsinfo.com) version 3.249. Version 4.xxx should work as well. From the "File" menu select "Open" and choose "main.c". Press F9 to compile.

Since the code only uses 22% of the ROM on the PIC18F4520, an equivalent CPU with less ROM can be used to save cost. 

To change the key mappings, edit TABLES.C and change the main translation table. Any PS/2 scancode can be mapped to any key, including the extra keys found on multimedia keyboards.


# Chip-8

Chip8 emulator written in modern C++. This emulator support various version of Chip8
 including:

* Original Chip8 that runs older games.
* Chip48 with modern instructions and quirks.
* Super-Chip with increased screen size and new instructions.
* XO-Chip, supports up to 4 different colors, new instructions and advanced sounds.

To maximize compatability it is also possible to adjust individual quirks.
With this feature you can emulate the behavior of legacy Super-Chip or 
player Chip48 or Super-Chip games on XO-Chip.

List of available quirks:
* **VF Reset** - Instructions `8XY1` `8XY2` `8XY3` set` VF` to `0` when turned on.
* **Display wait** - This quirk does not affect the emulator in any way yet.
* **Memor**y - Instructions `FX55` and `FX65` will increment index register if on.
* **Clipping** - Sprites at the edge of the screen will be clipped instead of wrapped.
* **Shifting** - ...
* **Jumping** - ...

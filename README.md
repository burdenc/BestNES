# BestNES
Cycle accurate, but some inconsistencies with reads/writes in sub-ops. Functionally and cycle accurate however.

CPU emulation mostly complete, PPU emulation next. Unofficial opcodes might be implemented soon.

Testing done against log of Nintendulator.

Current Goals (in order):
* Better management of cycle accuracy code (currently there's seemingly random memory reads in some opcodes)
* Basic name table/pattern table/mirroring output
* Run Tetris
* Run Mario
* Run any game with sound
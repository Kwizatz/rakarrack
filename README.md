AeonGames [Rakarrack](http://rakarrack.sourceforge.net/) Fork
========================

"Rakarrack is a richly featured multi-effects processor emulating a guitar effects pedalboard.  Effects include compressor, expander, noise gate, graphic equalizer, parametric equalizer, exciter, shuffle, convolotron, valve, flanger, dual flange, chorus, musicaldelay, arpie, echo with reverse playback, musical delay, reverb, digital phaser, analogic phaser, synthfilter, varyband, ring, wah-wah, alien-wah, mutromojo, harmonizer, looper and four flexible distortion modules including sub-octave modulation and dirty octave up.  Most of the effects engine is built from modules found in the excellent software synthesizer ZynAddSubFX.  Presets and user interface are optimized for guitar, but Rakarrack processes signals in stereo while it does not apply internal band-limiting filtering, and thus is well suited to all musical instruments and vocals.  Rakarrack is designed for Linux distributions with Jack Audio Connection Kit."

Original development seems to have stalled and now modern compilers do not let pass some of the things they used to in the Rakarrack code, and even when those things are brought back to modern times, they don't always fix the program. That is why this fork exists.

Building From Source
--------------------

Same as original Rakarrack, you will need autotools, as well as the libraries listed on the original [README](README) file.

Building For Arch or Manjaro
----------------------------

There is a PKGBUILD file at the [AeonGames Arch Repo](https://github.com/AeonGames/ArchRepo/tree/main/rakarrack) which you can download and run makepkg on.

Building For Windows
--------------------

Running on Windows is in the experimental stage right now, building for [MSYS2](https://www.msys2.org) is likely to be the first success, to do so, follow the same instructions as on Linux after installing the following packages:

`pacman -S --needed --noconfirm automake autoconf mingw-w64-x86_64-fltk mingw-w64-x86_64-dlfcn mingw-w64-x86_64-jack2 mingw-w64-x86_64-qjackctl mingw-w64-x86_64-xpm-nox`

This is not an exhaustive list, and the code does not currently fully compile, but It should get you past the configure phase, if not then we need to add some more libraries to those above.

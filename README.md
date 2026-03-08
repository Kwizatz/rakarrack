AeonGames [Rakarrack](http://rakarrack.sourceforge.net/) Fork
========================

[![Linux Build](https://github.com/Kwizatz/rakarrack/actions/workflows/build-linux.yml/badge.svg)](https://github.com/Kwizatz/rakarrack/actions/workflows/build-linux.yml)
[![Windows Build](https://github.com/Kwizatz/rakarrack/actions/workflows/build-windows.yml/badge.svg)](https://github.com/Kwizatz/rakarrack/actions/workflows/build-windows.yml)
[![MSYS2 Build](https://github.com/Kwizatz/rakarrack/actions/workflows/build-msys2.yml/badge.svg)](https://github.com/Kwizatz/rakarrack/actions/workflows/build-msys2.yml)
[![macOS Build](https://github.com/Kwizatz/rakarrack/actions/workflows/build-macos.yml/badge.svg)](https://github.com/Kwizatz/rakarrack/actions/workflows/build-macos.yml)

"Rakarrack is a richly featured multi-effects processor emulating a guitar effects pedalboard.  Effects include compressor, expander, noise gate, graphic equalizer, parametric equalizer, exciter, shuffle, convolotron, valve, flanger, dual flange, chorus, musicaldelay, arpie, echo with reverse playback, musical delay, reverb, digital phaser, analogic phaser, synthfilter, varyband, ring, wah-wah, alien-wah, mutromojo, harmonizer, looper and four flexible distortion modules including sub-octave modulation and dirty octave up.  Most of the effects engine is built from modules found in the excellent software synthesizer ZynAddSubFX.  Presets and user interface are optimized for guitar, but Rakarrack processes signals in stereo while it does not apply internal band-limiting filtering, and thus is well suited to all musical instruments and vocals.  Rakarrack is designed for Linux distributions with Jack Audio Connection Kit."

Original development seems to have stalled and now modern compilers do not let pass some of the things they used to in the Rakarrack code, and even when those things are brought back to modern times, they don't always fix the program. That is why this fork exists.

Platform Support
----------------

CI builds currently run on:

Note: Windows and macOS support are currently experimental.

- Linux (Ubuntu)
- Windows (MSVC)
- Windows (MSYS2 / MinGW)
- macOS

Building From Source
--------------------

Build using CMake:

```bash
cmake -B build -S .
cmake --build build
cmake --install build
```

Building For Linux (Arch, Manjaro)
----------------------------------

Install dependencies:

```bash
sudo pacman -S --needed base-devel cmake pkgconf qt6-base jack fftw libsamplerate libsndfile nlohmann-json alsa-lib
```

Then configure and build:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

There is also a PKGBUILD at the [AeonGames Arch Repo](https://github.com/AeonGames/ArchRepo/tree/main/rakarrack) if you prefer packaging via `makepkg`.

Dependencies
------------

- Qt6 (Widgets)
- JACK Audio Connection Kit
- FFTW3
- libsamplerate
- libsndfile
- nlohmann-json
- ALSA (optional, for MIDI support)

Building For Windows
--------------------

Windows support is experimental.

### MSVC (Visual Studio + vcpkg)

Install [vcpkg](https://github.com/microsoft/vcpkg), then configure using the vcpkg toolchain file:

```powershell
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
cmake --install build --config Release
```

Dependencies are provided via `vcpkg.json`.

### MSYS2 (MinGW)

Build and compilation must be done within an [MSYS2](https://www.msys2.org) MinGW 64-bit environment.

Install dependencies:

```bash
pacman -S --needed --noconfirm mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-qt6-base mingw-w64-x86_64-dlfcn mingw-w64-x86_64-jack2 mingw-w64-x86_64-nlohmann-json mingw-w64-x86_64-fftw mingw-w64-x86_64-libsamplerate mingw-w64-x86_64-libsndfile
```

Then configure and build:

```bash
cmake -G "MSYS Makefiles" -B build -S . -DCMAKE_BUILD_TYPE=Release -DENABLE_MIDI=OFF
cmake --build build
```

Building For macOS
------------------

macOS support is experimental. MIDI support (ALSA) is not available on macOS and is automatically disabled.

Install dependencies via [Homebrew](https://brew.sh):

```bash
brew install fftw jack libsamplerate libsndfile nlohmann-json pkg-config qt@6
```

Then build with CMake:

```bash
cmake -B build -S .
cmake --build build
```

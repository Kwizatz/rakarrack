# Rakarrack Copilot Instructions

## Project Overview

Rakarrack is a multi-effects processor emulating a guitar effects pedalboard, designed for Linux with Jack Audio Connection Kit. This is a fork that modernizes the original codebase for compatibility with modern compilers.

**Key Technologies:**
- C++ (file extension: `.cpp` for implementation files, `.hpp` for headers)
- FLTK (Fast Light Toolkit) for GUI
- JACK Audio Connection Kit for audio I/O
- FFTW3 for FFT operations
- libsamplerate, libsndfile for audio processing

## Build System

**CMake (sole build system):**
```bash
cmake -B build -S .
cmake --build build
cmake --install build
```

### Build Configuration Options

**CMake Options:**
- `ENABLE_MIDI=ON/OFF` - Enable MIDI support (requires ALSA, default: ON)

**Windows (MSYS2):**
Building on Windows is experimental and **must be done within an MSYS2 environment** (not native Windows Command Prompt or PowerShell). 

Required MSYS2 packages:
```bash
pacman -S --needed --noconfirm automake autoconf mingw-w64-x86_64-fltk \
  mingw-w64-x86_64-dlfcn mingw-w64-x86_64-jack2 mingw-w64-x86_64-qjackctl \
  mingw-w64-x86_64-xpm-nox mingw-w64-x86_64-nlohmann-json
```

Use the MSYS2 MinGW 64-bit shell for all build and compilation steps.

## Project Architecture

### Core Components

**Main Application Flow:**
1. `main.cpp` - Entry point, command-line parsing, initialization
2. `rakarrack.fl` - FLTK FLUID file defining GUI layout (generates `rakarrack.cxx` and `rakarrack.h`)
3. `jack.cpp` - JACK audio interface and callback setup
4. `process.cpp` - Main processing loop, global variables

**Central Classes:**
- `RKR` (in `global.hpp`) - Main effects processor, owns all effect instances and handles signal routing
- `RKRGUI` (in `rakarrack.h`) - Main GUI window and user interface management

### Effects Architecture

All effects inherit from the `Effect` base class (originally from ZynAddSubFX):
- Base class in `Effect.hpp` defines interface: `out()`, `changepar()`, `getpar()`, `setpreset()`, `cleanup()`
- Each effect is a separate class (e.g., `Reverb`, `Distorsion`, `Chorus`, `Phaser`)
- Effects process stereo float buffers in `out(float *smpsl, float *smpsr)`

**Effect Modules (40+ effects):**
- Distortion family: `Distorsion`, `NewDist`, `MBDist`, `StompBox`
- Modulation: `Chorus`, `Phaser`, `APhaser` (Analog Phaser), `Dual_Flange`, `Vibe`
- Delay/Echo: `Echo`, `RBEcho`, `MusicDelay`, `Echotron`
- Reverb: `Reverb`, `Reverbtron`
- Dynamics: `Compressor`, `Gate`, `Expander`, `FLimiter`
- Filters: `DynamicFilter` (WahWah), `Alienwah`, `RyanWah`, `Synthfilter`, `EQ`
- Pitch: `Harmonizer`, `StereoHarm`, `Shifter`
- Other: `Ring`, `Vocoder`, `Looper`, `Tuner`, `Pan`, `Valve`

### Signal Flow

```
Input → RKR::Alg() → Effect Chain → Output
         ↓
    Control_Gain() / Control_Volume()
         ↓
    Individual Effect::out() processing
```

- Effects are instantiated in `RKR` class (e.g., `efx_Rev`, `efx_Chorus`, `efx_Distorsion`)
- Signal routing handled by `RKR::Alg()` function
- Optional upsampling via `Resample` class for certain effects

### FLUID GUI Generation

The GUI is designed using FLTK's FLUID tool:
- Source: `src/rakarrack.fl`
- Generated files: `rakarrack.cxx`, `rakarrack.h` (auto-generated during build)
- **Never edit generated files directly** - modify the `.fl` file instead
- Custom widgets: `SliderW`, `NewVum`, `Scope`, `Analyzer`, `TunerLed`

## File Naming Conventions

- Implementation files use `.cpp` extension
- Headers use `.hpp` extension
- Effect files named after the effect: `Echo.cpp`/`Echo.hpp`, `Reverb.cpp`/`Reverb.hpp`
- FLUID-generated files retain their original extensions: `rakarrack.cxx`/`rakarrack.h`
- Supporting modules: `AnalogFilter`, `EffectLFO`, `FilterParams`, `delayline`

## Important Constants and Macros

Defined in `global.hpp`:
- `POLY 8` - Maximum polyphony for note tracking
- `MAX_DELAY 2` - Max delay in seconds
- `SAMPLE_RATE` - Global sample rate (runtime determined)
- `PERIOD` - Audio buffer period size
- Math helpers: `dB2rap()`, `rap2dB()`, `CLAMP()`, `F2I()`
- DSP constants: `D_PI`, `PI`, `LOG_10`, denormal guards

## Data Files

Located in `data/`:
- `.rkrb` files - Bank presets (binary format)
- `.rvb` files - Reverb IR data
- `.dly` files - Echo/delay IR data
- `.wav` files - Sample files for convolution
- `.png` files - GUI background skins
- `rakarrack.desktop` - Linux desktop entry

## Preset System

- `FPreset` class handles internal preset loading
- Banks stored as binary files (nlohmann/json library available for future migration)
- Methods: `loadfile()`, `savefile()`, `loadbank()`, `savebank()` in `RKR` class
- Legacy bank conversion: `ConvertOldFile()`, `ConvertReverbFile()`

## MIDI Support

When `ENABLE_MIDI=ON`:
- `MIDIConverter` class provides MIDI to control mapping
- ALSA sequencer used for MIDI I/O
- MIDI tables: `loadmiditable()`, `savemiditable()`
- Event handling: `jack_process_midievents()`, `process_midi_controller_events()`

## Development Notes

- **No test suite exists** - testing requires manual verification with JACK and audio input
- No linting configuration present
- Code originally from ZynAddSubFX effects engine (see `Effect.hpp` header)
- Fork purpose: Fix modern compiler compatibility issues from original stalled development
- Windows support is experimental
- **C++26 standard** is set in `cmake/compiler.cmake`
- Owned pointers use `std::unique_ptr`, dynamic buffers use `std::vector`, fixed arrays use `std::array`
- Constants in `global.hpp` use `inline constexpr` instead of `#define`

# Rakarrack Qt6 Migration Plan

## Executive Summary

Replace the entire FLTK-based GUI (~20,000 lines in a monolithic FLUID `.fl` file) with a
Qt6 Widgets UI while simultaneously decoupling the audio engine from the GUI layer.  The
migration is broken into **7 phases** designed to keep the application compilable and
testable at each checkpoint.

---

## Current State Inventory

| Area | Details |
|---|---|
| **GUI framework** | FLTK 1.4, single 18,608-line `rakarrack.fl` FLUID file |
| **Windows** | 7: Principal (main), BankWindow, Order, Settings, MIDILearn, AboutWin, Trigger |
| **Effect panels** | 47 individual effect parameter panels |
| **Utility panels** | 6: Tuner, InOut, Midi, Metro, Presets, Tap |
| **Custom widgets** | 5: Analyzer (spectrum), Scope (oscilloscope), NewVum (VU meters), TunerLed (pitch indicator), SliderW (~400+ instances, MIDI-learn-capable slider) |
| **GUI↔Engine coupling** | 3,023 `rkr->` references, 505 `changepar()` calls, `Fl_Preferences` in engine code |
| **Thread safety** | Zero — JACK RT thread and GUI share mutable state with no synchronization |
| **Settings** | `Fl_Preferences` (~90+ keys) in `process.cpp` / `fileio.cpp` |
| **Presets** | CSV `.rkr` files, binary `.rkrb` banks, `lv[70][20]` storage |
| **Build** | CMake, C++26 (GCC 15), FLUID codegen step |

### Critical Pain Points Motivating Migration

1. FLTK rendering bugs (subwindows blank at startup, damage propagation issues)
2. Third-party library memory leaks (fontconfig, Pango, FLTK/Cairo) visible under ASan
3. Monolithic FLUID file impossible to review/merge in version control
4. No separation between engine and GUI — blocks headless/plugin modes
5. Zero thread safety between JACK RT callback and GUI
6. FLTK's limited widget ecosystem (no built-in dials, knobs, or real-time plotting)

---

## Architecture Goals

### Target Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Qt6 GUI Layer                        │
│  QMainWindow ← QStackedWidget (effect panels)          │
│  Custom widgets: SpectrumAnalyzer, Oscilloscope,        │
│                  VUMeter, TunerDisplay, MidiSlider      │
│  QSettings for persistence                              │
└────────────────────┬────────────────────────────────────┘
                     │  Signals / Slots (Qt::QueuedConnection)
                     │  + lock-free ring buffer (RT → GUI)
┌────────────────────▼────────────────────────────────────┐
│               Engine Interface Layer                     │
│  EngineController: thread-safe API                       │
│  - setEffectParameter(effectId, paramId, value)          │
│  - getEffectParameter(effectId, paramId) → value         │
│  - setPreset(bankId, presetId)                           │
│  - getAudioLevels() → {left, right}                     │
│  - getTunerData() → {freq, note, cents}                  │
│  - getSpectrumData() → span<float>                       │
│  - getScopeData() → span<float>                          │
│  Lock-free ring buffers for RT→GUI data                  │
│  Atomic flags for control changes                        │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                Audio Engine (RKR)                         │
│  JACK callback → Alg() → Effect chain                    │
│  Pure DSP, zero GUI dependencies                         │
│  Unchanged effect classes (Echo, Reverb, etc.)           │
└─────────────────────────────────────────────────────────┘
```

### Key Design Principles

1. **Strict Model-View separation** — Engine classes never include Qt or GUI headers
2. **Thread-safe communication** — Lock-free ring buffers for RT→GUI audio data;
   `std::atomic` flags for parameter changes; Qt signals/slots with `Qt::QueuedConnection`
   for GUI→engine commands
3. **Modular panels** — Each effect gets its own `.ui`/`.cpp`/`.hpp` triplet
4. **Testable** — Engine can run headless; GUI can be tested with mock engine

---

## Phase 0: Engine Decoupling (Pre-Qt — FLTK still works)

**Goal:** Eliminate all GUI dependencies from the audio engine so it compiles standalone.

### 0.1 — Extract `Fl_Preferences` from engine code

| File | Action |
|---|---|
| `process.cpp` | Replace `Fl_Preferences` with a plain `AppSettings` struct loaded at init |
| `fileio.cpp` | Same — pass settings struct instead of reading `Fl_Preferences` |
| New: `AppSettings.hpp` | Key-value store backed by `nlohmann::json` (already a dependency) |

### 0.2 — Remove FLTK includes from engine headers

| File | FLTK Usage | Replacement |
|---|---|---|
| `global.hpp` | `Fl_Preferences` member | `AppSettings` reference |
| `varios.cpp` | `fl_alert()` / `fl_choice()` in `Message()` | Callback / `std::function<void(std::string)>` |
| `process.cpp` | `Fl_Preferences` | `AppSettings` |
| `fileio.cpp` | `Fl_File_Chooser`, `fl_alert` | Callbacks injected by GUI layer |

### 0.3 — Create `EngineController` interface

```cpp
// src/engine/EngineController.hpp
class EngineController {
public:
    // Parameter access (thread-safe)
    void setEffectParameter(int effectIndex, int paramId, int value);
    int  getEffectParameter(int effectIndex, int paramId) const;

    // Effect chain
    void setEffectOrder(std::span<const int> order);
    void setEffectEnabled(int effectIndex, bool enabled);

    // Presets
    void loadPreset(int bank, int index);
    void savePreset(int bank, int index);

    // Real-time data (lock-free ring buffers)
    AudioLevels   getAudioLevels() const;
    TunerData     getTunerData() const;
    SpectrumData  getSpectrumData() const;
    ScopeData     getScopeData() const;

    // Settings
    void loadSettings(const std::filesystem::path& path);
    void saveSettings(const std::filesystem::path& path);

    // Underlying engine (for advanced access)
    RKR& engine();
};
```

### 0.4 — Thread safety primitives

| Data Flow | Mechanism |
|---|---|
| GUI → Engine (param changes) | `std::atomic<int>` per parameter or a SPSC command queue |
| Engine → GUI (audio levels, tuner, spectrum) | Lock-free SPSC ring buffer (`std::atomic` read/write indices) |
| Engine → GUI (state changes) | `std::atomic` flags polled by GUI timer |

### Deliverable
- FLTK GUI still works (reads from `EngineController` instead of `rkr->` directly)
- Engine compiles without any FLTK header
- New directory structure: `src/engine/` for decoupled engine files

### Estimated Effort: 2-3 weeks

---

## Phase 1: CMake & Qt6 Scaffolding

**Goal:** Add Qt6 as an alternative build target alongside FLTK.

### 1.1 — CMake changes

```cmake
option(USE_QT6 "Build with Qt6 GUI instead of FLTK" OFF)

if(USE_QT6)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    qt_standard_project_setup()
    add_subdirectory(src/gui/qt6)
else()
    # Existing FLTK build path
    add_subdirectory(src/gui/fltk)
endif()

# Engine is always built
add_subdirectory(src/engine)
```

### 1.2 — Directory restructure

```
src/
├── engine/              # All DSP + RKR class (no GUI)
│   ├── CMakeLists.txt
│   ├── RKR.cpp/hpp      # Renamed from process.cpp + global.hpp
│   ├── EngineController.cpp/hpp
│   ├── AppSettings.cpp/hpp
│   ├── AllEffects.hpp
│   ├── Echo.cpp/hpp
│   ├── Reverb.cpp/hpp
│   └── ... (all effect files)
├── gui/
│   ├── fltk/            # Legacy FLTK GUI (preserved during transition)
│   │   ├── CMakeLists.txt
│   │   ├── rakarrack.fl
│   │   └── main_fltk.cpp
│   └── qt6/             # New Qt6 GUI
│       ├── CMakeLists.txt
│       ├── main.cpp
│       ├── MainWindow.cpp/hpp/ui
│       ├── widgets/
│       │   ├── MidiSlider.cpp/hpp
│       │   ├── SpectrumAnalyzer.cpp/hpp
│       │   ├── Oscilloscope.cpp/hpp
│       │   ├── VUMeter.cpp/hpp
│       │   └── TunerDisplay.cpp/hpp
│       ├── panels/
│       │   ├── EffectPanel.cpp/hpp    # Base class
│       │   ├── ReverbPanel.cpp/hpp
│       │   ├── EchoPanel.cpp/hpp
│       │   └── ... (47 panels)
│       ├── dialogs/
│       │   ├── BankDialog.cpp/hpp
│       │   ├── SettingsDialog.cpp/hpp
│       │   ├── MidiLearnDialog.cpp/hpp
│       │   └── OrderDialog.cpp/hpp
│       └── resources/
│           ├── rakarrack.qrc
│           └── styles/
└── common/              # Shared types, constants
    ├── Types.hpp
    └── Constants.hpp
```

### 1.3 — Qt6 dependencies

```cmake
# Required Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS
    Widgets      # Core widget framework
    # OpenGL     # Future: hardware-accelerated spectrum/scope rendering
)
```

No additional external Qt modules needed — standard Qt6 Widgets covers all requirements.

### Deliverable
- `cmake -DUSE_QT6=ON` builds a skeleton Qt6 app showing an empty `QMainWindow`
- `cmake -DUSE_QT6=OFF` builds the original FLTK app (unchanged)
- Engine compiles as a static library shared by both GUI targets

### Estimated Effort: 1 week

---

## Phase 2: Custom Widget Ports

**Goal:** Port the 5 custom FLTK widgets to Qt6 using `QPainter`.

### 2.1 — MidiSlider (replaces SliderW)

**Most critical widget** — ~400+ instances across all panels.

```cpp
class MidiSlider : public QSlider {
    Q_OBJECT
public:
    // MIDI learn support (right-click context menu)
    void setMidiController(int cc);
    int  midiController() const;

signals:
    void midiLearnRequested(MidiSlider* slider);

protected:
    void paintEvent(QPaintEvent*) override;      // Themed drawing
    void mousePressEvent(QMouseEvent*) override;  // Right-click → MIDI learn
    void wheelEvent(QWheelEvent*) override;       // Mouse wheel with Ctrl modifier
    void keyPressEvent(QKeyEvent*) override;      // Shift/Ctrl step size modifiers
};
```

**Key differences from SliderW:**
- Context menu via `QMenu` instead of FLTK callback hacks
- `QPainter` gradient fills replace `fl_rectf()` calls
- Integer value range matching FLTK convention (0-127 for most params)
- Accessibility: Qt provides free keyboard navigation and screen reader support

### 2.2 — SpectrumAnalyzer (replaces Analyzer)

```cpp
class SpectrumAnalyzer : public QWidget {
    Q_OBJECT
public:
    void setData(std::span<const float> bands);  // 28-band data from engine

protected:
    void paintEvent(QPaintEvent*) override;
    // QPainter::fillRect() replaces fl_rectf()
    // QLinearGradient for bar coloring
};
```

### 2.3 — Oscilloscope (replaces Scope)

```cpp
class Oscilloscope : public QWidget {
    Q_OBJECT
public:
    void setData(std::span<const float> left, std::span<const float> right);

protected:
    void paintEvent(QPaintEvent*) override;
    // QPainterPath for waveform lines replaces fl_line() loops
};
```

### 2.4 — VUMeter (replaces NewVum)

```cpp
class VUMeter : public QWidget {
    Q_OBJECT
public:
    void setLevel(float dB);

protected:
    void paintEvent(QPaintEvent*) override;
    // Multi-zone gradient: green → yellow → orange → red
};
```

### 2.5 — TunerDisplay (replaces TunerLed)

```cpp
class TunerDisplay : public QWidget {
    Q_OBJECT
public:
    void setTunerData(float frequency, int note, float cents);

protected:
    void paintEvent(QPaintEvent*) override;
    // Centered pitch indicator with note name label
};
```

### Deliverable
- All 5 widgets compile and render in a standalone test harness
- Visual parity with FLTK originals (same color schemes, proportions)
- Unit tests for value range mapping and MIDI learn logic

### Estimated Effort: 2-3 weeks

---

## Phase 3: Main Window & Navigation

**Goal:** Implement the main application window with effect panel switching.

### 3.1 — MainWindow

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(EngineController& engine, QWidget* parent = nullptr);

private:
    // Central layout mirrors FLTK Principal window
    QStackedWidget*    m_effectStack;    // Switches between 47 effect panels
    QWidget*           m_topBar;         // InOut levels, tuner, preset selector
    QWidget*           m_effectSelector; // Row of 10 effect-slot buttons
    QWidget*           m_bottomBar;      // Master volume, bypass, tap tempo

    // Timer replaces FLTK's 40ms tick()
    QTimer*            m_guiTimer;       // 25ms (40 Hz) poll for engine state

    EngineController&  m_engine;

private slots:
    void onGuiTick();                    // Poll engine for levels/tuner/MIDI
    void onEffectSlotClicked(int slot);
    void onPresetChanged(int bank, int index);
};
```

### 3.2 — Effect panel system

```cpp
// Base class for all 47 effect panels
class EffectPanel : public QWidget {
    Q_OBJECT
public:
    EffectPanel(EngineController& engine, int effectIndex, QWidget* parent = nullptr);

    virtual void syncFromEngine();  // Pull current params from engine
    virtual void syncToEngine();    // Push GUI values to engine

protected:
    EngineController& m_engine;
    int m_effectIndex;

    // Factory
    static std::unique_ptr<EffectPanel> create(int effectType,
                                                EngineController& engine,
                                                QWidget* parent);
};
```

### 3.3 — Top bar components

| FLTK Element | Qt6 Replacement |
|---|---|
| Input/Output level meters | 2× `VUMeter` widget |
| Tuner display | `TunerDisplay` widget |
| Preset name + bank selector | `QComboBox` + `QLineEdit` |
| Effect on/off button row | 10× `QPushButton` (checkable, colored) |
| Master volume | `MidiSlider` |

### 3.4 — GUI refresh timer

Replace FLTK's `Fl::add_timeout(0.04, tick, ...)` with:

```cpp
m_guiTimer = new QTimer(this);
connect(m_guiTimer, &QTimer::timeout, this, &MainWindow::onGuiTick);
m_guiTimer->start(25); // 40 Hz refresh
```

`onGuiTick()` reads lock-free ring buffers for levels, tuner data, spectrum, scope,
and MIDI control flags — exactly what `tick()` + `ActMIDI()` do today.

### Deliverable
- Main window with effect slot bar, tuner, levels, preset selector
- One fully functional effect panel (e.g., Reverb) as proof-of-concept
- Panel switching works via `QStackedWidget`

### Estimated Effort: 2-3 weeks

---

## Phase 4: All 47 Effect Panels

**Goal:** Port every effect panel from FLTK to Qt6.

### Strategy: Template-driven panel generation

Most effect panels follow the same pattern:
1. A preset `QComboBox` (dropdown)
2. 5-15 `MidiSlider` instances for parameters
3. An on/off toggle
4. Optional: LFO controls (type dropdown + rate/depth/phase sliders)

**Approach:**
- Create a `ParameterLayout` helper that builds a grid of labeled `MidiSlider`s from a
  parameter descriptor list
- Each panel subclass only overrides layout specifics and adds custom controls where needed
- Panels with unique elements (e.g., EQ has a curve editor, Looper has transport controls)
  get hand-crafted layouts

### Panel categories

| Category | Panels | Special widgets needed |
|---|---|---|
| **Simple slider grids** | Echo, Chorus, Phaser, Flanger, Pan, Ring, Gate, CompBand, OTrem, Vibe, ShelfBoost, Sustainer, Shuffle, Expander | None extra |
| **Distortion family** | Distorsion, NewDist, MBDist, StompBox, Valve, Exciter, CoilCrafter | Waveshaper type selector |
| **Modulation** | APhaser, Dual_Flange, Alienwah, DynamicFilter, RyanWah, Synthfilter, Infinity | LFO controls |
| **Delay/Echo** | MusicDelay, Arpie, RBEcho, Echotron, Sequence | Rhythm pattern display |
| **Reverb** | Reverb, Reverbtron, Convolotron | IR file selector |
| **Pitch** | Harmonizer, StereoHarm, Shifter | Interval/chord selector |
| **EQ** | EQ (parametric), HarmonicEnhancer | Frequency curve editor (`QPainterPath`) |
| **Vocoder** | Vocoder | Band matrix display |
| **Looper** | Looper | Transport (rec/play/stop), waveform display |

### Panel implementation checklist (per panel)

- [ ] Create `<Effect>Panel.hpp/.cpp`
- [ ] Define parameter layout (names, ranges, defaults)
- [ ] Implement `syncFromEngine()` — read all params from `EngineController`
- [ ] Implement `syncToEngine()` — write changed params to `EngineController`
- [ ] Connect `MidiSlider::valueChanged` → `syncToEngine()`
- [ ] Register in `EffectPanel::create()` factory
- [ ] Visual verification against FLTK original

### Deliverable
- All 47 panels functional
- All 6 utility panels (Tuner, InOut, Midi, Metro, Presets, Tap) ported
- Full parameter parity with FLTK version

### Estimated Effort: 4-6 weeks (bulk of the work)

---

## Phase 5: Dialogs & Ancillary Windows

**Goal:** Port all secondary windows and dialogs.

### 5.1 — Bank Window → BankDialog

| FLTK | Qt6 |
|---|---|
| 60-slot grid of preset buttons | `QGridLayout` with 60 `QPushButton`s (5 rows × 12 cols) |
| Bank file selector | `QFileDialog` |
| Import/Export | `QFileDialog` + drag-and-drop support |

### 5.2 — Effect Order → OrderDialog

| FLTK | Qt6 |
|---|---|
| 10 `Fl_Choice` dropdowns | 10 `QComboBox` + drag-and-drop reordering via `QListWidget` |

### 5.3 — Settings → SettingsDialog

| FLTK Tab | Qt6 Tab |  Key Controls |
|---|---|---|
| Look | Appearance | Background image selector, color scheme, font size |
| Audio | Audio | Sample rate display, buffer size, upsampling |
| MIDI | MIDI | MIDI channel, controller table |
| JACK | JACK | Auto-connect, port names |
| Misc | Miscellaneous | Paths, startup behavior |
| Bank | Bank Paths | Bank directory list |

Settings storage: Replace `Fl_Preferences` with `QSettings` (INI format, same key names
for migration compatibility).

### 5.4 — MIDI Learn → MidiLearnDialog

| FLTK | Qt6 |
|---|---|
| Table of MIDI CC assignments | `QTableWidget` with CC#, min, max, parameter columns |
| "Learn" button per row | Click row → next MIDI CC automatically assigned |

### 5.5 — About → AboutDialog

Simple `QDialog` with `QLabel` (rich text), version info, credits, license.

### 5.6 — Help viewer

| FLTK | Qt6 |
|---|---|
| `Fl_Help_Dialog` (built-in HTML viewer) | `QTextBrowser` or launch external browser via `QDesktopServices::openUrl()` |

### 5.7 — Trigger (ACI) → TriggerDialog

Audio-controlled interface settings. Simple slider panel in a `QDialog`.

### Deliverable
- All 7 windows/dialogs functional
- Settings migration: reads old `Fl_Preferences` values on first run, writes `QSettings`
- Help system works (embedded or external browser)

### Estimated Effort: 2 weeks

---

## Phase 6: Polish, Styling & Platform Integration

**Goal:** Visual refinement, theming, and platform integration.

### 6.1 — Theming

```cpp
// Qt6 stylesheet approach (replaces FLTK scheme + background images)
qApp->setStyleSheet(R"(
    QMainWindow { background: #2b2b2b; }
    QSlider::groove:horizontal { background: #444; border-radius: 2px; height: 6px; }
    QSlider::handle:horizontal { background: #0af; width: 14px; margin: -4px 0; border-radius: 7px; }
    QPushButton:checked { background: #0a0; color: white; }
)");
```

- Support for background skin images (8 `.png` skins → `QPixmap` backgrounds, selectable in Settings)
- Dark/light theme toggle
- Font scaling for accessibility

### 6.2 — System tray integration

```cpp
QSystemTrayIcon* tray = new QSystemTrayIcon(QIcon(":/icons/rakarrack.png"), this);
tray->setContextMenu(trayMenu);  // Show/Hide, Bypass, Quit
tray->show();
```

### 6.3 — Keyboard shortcuts

| Action | Shortcut |
|---|---|
| Bypass all effects | `Ctrl+B` |
| Next preset | `Ctrl+Right` |
| Previous preset | `Ctrl+Left` |
| Save preset | `Ctrl+S` |
| Toggle tuner | `Ctrl+T` |
| Settings | `Ctrl+,` |

### 6.4 — High-DPI support

Qt6 handles high-DPI natively via `Qt::HighDpiScaleFactorRoundingPolicy`.
Custom widgets use logical coordinates — no pixel math.

### 6.5 — Wayland support

Qt6 has native Wayland support — eliminates the `libdecor` leaks and X11 issues
that motivated this migration.

### Deliverable
- Polished, themed UI matching or exceeding FLTK visual quality
- Keyboard shortcuts functional
- Clean behavior on X11 and Wayland
- System tray icon (optional)

### Estimated Effort: 1-2 weeks

---

## Phase 7: Cleanup & FLTK Removal

**Goal:** Remove all FLTK code and dependencies.

### 7.1 — Remove files

- Delete `src/gui/fltk/` directory (including `rakarrack.fl`)
- Remove FLTK `find_package()`, includes, link directives from CMake
- Remove `USE_QT6` option (Qt6 becomes the only GUI)
- Remove XPM dependency (only used for FLTK window icon — Qt uses `.png` via `QIcon`)

### 7.2 — Remove FLTK-only code paths

- Delete `icono_rakarrack_*.xpm` files
- Remove any remaining `#ifdef USE_FLTK` guards
- Clean `global.hpp` of any residual FLTK types

### 7.3 — Update documentation

- Update `README.md` with new Qt6 build instructions
- Update `.github/copilot-instructions.md` to reflect Qt6 architecture
- Update `INSTALL` file

### 7.4 — Final dependency list

```cmake
find_package(Qt6 REQUIRED COMPONENTS Widgets)
pkg_search_module(FFTW  REQUIRED fftw3)
pkg_search_module(JACK  REQUIRED jack)
pkg_search_module(SAMPLERATE REQUIRED samplerate)
pkg_search_module(SNDFILE REQUIRED sndfile)
find_package(nlohmann_json REQUIRED)
# ALSA still needed for MIDI (when ENABLE_MIDI=ON)
```

**Removed dependencies:** FLTK, XPM (libXpm)

### Deliverable
- Clean build with zero FLTK references
- All tests pass
- README updated

### Estimated Effort: 1 week

---

## FLTK → Qt6 Component Mapping Reference

| FLTK Component | Qt6 Equivalent |
|---|---|
| `Fl_Window` | `QMainWindow` / `QDialog` |
| `Fl_Group` | `QWidget` + `QLayout` |
| `Fl_Tabs` | `QTabWidget` |
| `Fl_Choice` | `QComboBox` |
| `Fl_Counter` | `QSpinBox` |
| `Fl_Check_Button` | `QCheckBox` |
| `Fl_Light_Button` | `QPushButton` (checkable) |
| `Fl_Button` | `QPushButton` |
| `Fl_Input` | `QLineEdit` |
| `Fl_Box` (label) | `QLabel` |
| `Fl_Value_Slider` | `QSlider` + `QLabel` (or custom `MidiSlider`) |
| `Fl_Browser` | `QListWidget` |
| `Fl_Preferences` | `QSettings` |
| `Fl_File_Chooser` | `QFileDialog` |
| `Fl_Help_Dialog` | `QTextBrowser` / `QDesktopServices` |
| `Fl::add_timeout()` | `QTimer` |
| `fl_alert()` / `fl_choice()` | `QMessageBox` |
| `fl_rectf()` / `fl_line()` | `QPainter::fillRect()` / `QPainter::drawLine()` |
| `Fl::wait()` main loop | `QApplication::exec()` |
| `Fl_PNG_Image` | `QPixmap` |
| FLTK color macros | `QColor` / `QPalette` |

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| Engine decoupling breaks audio processing | Medium | High | Keep FLTK build working throughout Phase 0-1; A/B test audio output |
| Custom widget rendering differs from FLTK | Low | Medium | Screenshot comparison; QPainter is more capable than FLTK drawing |
| Qt6 event loop conflicts with JACK RT thread | Low | High | Never call Qt from JACK thread; use only lock-free primitives |
| 47 panels create tedious repetitive work | High | Medium | Template-driven `ParameterLayout` helper reduces per-panel code to ~50 lines |
| Preset compatibility broken | Medium | High | Preserve `lv[70][20]` storage format and CSV/binary parsers unchanged |
| Build time increase (Qt MOC/UIC) | Low | Low | Precompiled headers; unity builds |

---

## Estimated Timeline

| Phase | Description | Duration | Cumulative |
|---|---|---|---|
| 0 | Engine decoupling | 2-3 weeks | 2-3 weeks |
| 1 | CMake & Qt6 scaffolding | 1 week | 3-4 weeks |
| 2 | Custom widget ports | 2-3 weeks | 5-7 weeks |
| 3 | Main window & navigation | 2-3 weeks | 7-10 weeks |
| 4 | All 47 effect panels | 4-6 weeks | 11-16 weeks |
| 5 | Dialogs & ancillary windows | 2 weeks | 13-18 weeks |
| 6 | Polish & platform integration | 1-2 weeks | 14-20 weeks |
| 7 | FLTK removal & cleanup | 1 week | **15-21 weeks** |

**Total estimated effort: 15-21 weeks** (single developer, part-time hobbyist pace
may be 2× longer).

---

## Getting Started Checklist

1. [ ] Install Qt6 development packages: `sudo pacman -S qt6-base` or equivalent
2. [ ] Begin Phase 0.1: Create `AppSettings.hpp` using `nlohmann::json`
3. [ ] Begin Phase 0.2: Remove `Fl_Preferences` from `process.cpp`
4. [ ] Create `src/engine/` directory and move DSP files
5. [ ] Verify FLTK build still works after each step

---

## Open Questions

1. **Qt6 Widgets vs QML?** — This plan uses Qt6 Widgets (C++ `QWidget` subclasses).
   QML would provide smoother animations and easier styling but requires learning a new
   paradigm. **Recommendation: Widgets** — closer to FLTK's imperative model, faster port.

2. **Knob widgets?** — FLTK has no knobs (everything is a slider). Qt6 doesn't ship a
   `QDial`-derived knob either, but `QDial` exists and third-party knob widgets are
   available. Consider adding rotary knobs for certain parameters in a future enhancement.

3. **LV2/VST plugin target?** — The engine decoupling in Phase 0 is the prerequisite for
   building Rakarrack as an LV2 plugin. The Qt6 GUI could then serve as the standalone
   host while a minimal plugin GUI uses LV2 UI extensions. This is out of scope but
   enabled by this architecture.

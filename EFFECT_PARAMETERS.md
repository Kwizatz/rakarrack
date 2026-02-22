# Rakarrack Effect Parameter Reference

Complete parameter layout for all 47 effects, extracted from source code `changepar()`/`getpar()` methods.
For use in implementing Qt6 effect panels.

---

## Index 0: EQ1 (class: `EQ`)
**Source:** [EQ.cpp](src/EQ.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 10+band×5+0 | Band Type | 0–9 | choice (Off,LP1,HP1,LP2,HP2,BP,Notch,Peak,LShelf,HShelf) |
| 10+band×5+1 | Band Freq | 0–127 | slider (log frequency) |
| 10+band×5+2 | Band Gain | 0–127 | slider (center=64) |
| 10+band×5+3 | Band Q | 0–127 | slider |
| 10+band×5+4 | Band Stages | 0–MAX_FILTER_STAGES-1 | slider |

**Bands:** 0–9 (paramIDs 10–59), plus remaining bands up to 15.
**Special:** Frequency response display.

---

## Index 1: Compressor (class: `Compressor`)
**Source:** [Compressor.cpp](src/Compressor.cpp)
**Note:** Uses `Compressor_Change(np, value)` — params start at index 1.

| Param | Name | Range | Type |
|-------|------|-------|------|
| 1 | Threshold | negative dB | slider |
| 2 | Ratio | 0–127 | slider |
| 3 | Output | 0–127 | slider |
| 4 | Attack | ms | slider |
| 5 | Release | ms | slider |
| 6 | Auto Output | 0/1 | toggle |
| 7 | Knee | 0–100 | slider (%) |
| 8 | Stereo | 0/1 | toggle |
| 9 | Peak | 0/1 | toggle |

---

## Index 2: Distorsion (class: `Distorsion`)
**Source:** [Distorsion.cpp](src/Distorsion.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LR Cross | 0–127 | slider |
| 3 | Drive | 0–127 | slider |
| 4 | Level | 0–127 | slider |
| 5 | Type | choice | waveshape type |
| 6 | Negate | 0/1 | toggle |
| 7 | LPF | freq (Hz) | slider |
| 8 | HPF | freq (Hz) | slider |
| 9 | Stereo | 0/1 | toggle |
| 10 | Prefiltering | 0/1 | toggle |
| 11 | (unused) | — | — |
| 12 | Octave | 0–127 | slider |

---

## Index 3: Overdrive (class: `Distorsion`)
**Source:** Same as Distorsion — identical parameters.

---

## Index 4: Echo (class: `Echo`)
**Source:** [Echo.cpp](src/Echo.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | Delay | ms | slider |
| 3 | LR Delay | 0–127 | slider |
| 4 | LR Cross | 0–127 | slider |
| 5 | Feedback | 0–127 | slider |
| 6 | Hi Damp | 0–127 | slider |
| 7 | Reverse | 0–127 | slider |
| 8 | Direct | 0/1 | toggle |

---

## Index 5: Chorus (class: `Chorus`)
**Source:** [Chorus.cpp](src/Chorus.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Depth | 0–127 | slider |
| 7 | Delay | 0–127 | slider |
| 8 | Feedback | 0–127 | slider |
| 9 | LR Cross | 0–127 | slider |
| 10 | Flange Mode | 0/1 | toggle |
| 11 | Subtract | 0/1 | toggle |
| 12 | Awesome Mode | 0/1 | toggle |

**LFO section:** P2–P5

---

## Index 6: Phaser (class: `Phaser`)
**Source:** [Phaser.cpp](src/Phaser.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Depth | 0–127 | slider |
| 7 | Feedback | 0–127 | slider |
| 8 | Stages | 0–MAX_PHASER_STAGES-1 | slider |
| 9 | LR Cross | 0–127 | slider |
| 10 | Subtract | 0/1 | toggle |
| 11 | Phase | 0–127 | slider |

**LFO section:** P2–P5

---

## Index 7: Flanger (class: `Chorus`)
**Source:** Same as Chorus — identical parameters.

---

## Index 8: Reverb (class: `Reverb`)
**Source:** [Reverb.cpp](src/Reverb.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Pan | 0–127 | slider |
| 2 | Time | 0–127 | slider |
| 3 | Initial Delay | 0–127 | slider |
| 4 | Initial Delay Fb | 0–127 | slider |
| 5 | (unused) | — | — |
| 6 | (unused) | — | — |
| 7 | LPF | freq (Hz) | slider |
| 8 | HPF | freq (Hz) | slider |
| 9 | Lo/Hi Damp | 64–127 | slider |
| 10 | Type | choice | reverb type |
| 11 | Room Size | 0–127 | slider |

---

## Index 9: EQ2 (class: `EQ`)
**Source:** Same as EQ1 — identical parameters.

---

## Index 10: WahWah (class: `DynamicFilter`)
**Source:** [DynamicFilter.cpp](src/DynamicFilter.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Depth | 0–127 | slider |
| 7 | Amp Sensitivity | 0–127 | slider |
| 8 | Amp Sens Inverse | 0/1 | toggle |
| 9 | Amp Smooth | 0–127 | slider |

**LFO section:** P2–P5
**Special:** Also has a separate `FilterParams` object for underlying filter configuration.

---

## Index 11: Alienwah (class: `Alienwah`)
**Source:** [Alienwah.cpp](src/Alienwah.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Depth | 0–127 | slider |
| 7 | Feedback | 0–127 | slider |
| 8 | Delay | 0–MAX_ALIENWAH_DELAY | slider |
| 9 | LR Cross | 0–127 | slider |
| 10 | Phase | 0–127 | slider |

**LFO section:** P2–P5

---

## Index 12: Cabinet (class: `EQ`)
**Source:** [process.cpp](src/process.cpp) `Cabinet_setpreset()`

Uses the EQ class internally. Same parameter structure as EQ (index 0/9) but only presets differ (cabinet impulse response emulation via EQ bands). Presets configure 16 bands via `changepar(n*5+10, ...)` calls. The last value (index 80) sets volume.

Same parameters as EQ — see Index 0.

---

## Index 13: Pan (class: `Pan`)
**Source:** [Pan.cpp](src/Pan.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Extra | 0–127 | slider |
| 7 | AutoPan | 0/1 | toggle |
| 8 | Extra ON | 0/1 | toggle |

**LFO section:** P2–P5

---

## Index 14: Harmonizer (class: `Harmonizer`)
**Source:** [Harmonizer.cpp](src/Harmonizer.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | Gain | 0–127 | slider |
| 3 | Interval | 0–24 | slider (semitones, 12=unison) |
| 4 | Filter Freq | freq (Hz) | slider |
| 5 | Select | choice | scale/key selection |
| 6 | Note | choice | root note |
| 7 | Type | choice | harmonizer type |
| 8 | Filter Gain | 0–127 | slider |
| 9 | Filter Q | 0–127 | slider |
| 10 | MIDI | 0/1 | toggle |

**Special:** MIDI control, pitch shifting.

---

## Index 15: MusicDelay (class: `MusicDelay`)
**Source:** [MusicDelay.cpp](src/MusicDelay.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning 1 | 0–127 | slider |
| 2 | Delay 1 | tempo divisor | slider |
| 3 | LR Delay | tempo divisor | slider |
| 4 | LR Cross | 0–127 | slider |
| 5 | Feedback 1 | 0–127 | slider |
| 6 | Hi Damp | 0–127 | slider |
| 7 | Panning 2 | 0–127 | slider |
| 8 | Delay 2 | tempo divisor | slider |
| 9 | Feedback 2 | 0–127 | slider |
| 10 | Tempo | BPM | slider |
| 11 | Gain 1 | 0–127 | slider |
| 12 | Gain 2 | 0–127 | slider |

---

## Index 16: Gate (class: `Gate`)
**Source:** [Gate.cpp](src/Gate.cpp)
**Note:** Uses `Gate_Change(np, value)` — params start at index 1.

| Param | Name | Range | Type |
|-------|------|-------|------|
| 1 | Threshold | dB (negative) | slider |
| 2 | Range | dB | slider |
| 3 | Attack | ms | slider |
| 4 | Decay | ms | slider |
| 5 | LPF | freq (Hz) | slider |
| 6 | HPF | freq (Hz) | slider |
| 7 | Hold | ms | slider |

---

## Index 17: NewDist (class: `NewDist`)
**Source:** [NewDist.cpp](src/NewDist.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LR Cross | 0–127 | slider |
| 3 | Drive | 0–127 | slider |
| 4 | Level | 0–127 | slider |
| 5 | Type | choice | waveshape type |
| 6 | Negate | 0/1 | toggle |
| 7 | LPF | freq (Hz) | slider |
| 8 | HPF | freq (Hz) | slider |
| 9 | Resonance Freq | 0–127 | slider |
| 10 | Prefiltering | 0/1 | toggle |
| 11 | Octave | 0–127 | slider |

---

## Index 18: Analog Phaser (class: `APhaser`)
**Source:** [APhaser.cpp](src/APhaser.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Distortion | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Width | 0–127 | slider |
| 7 | Feedback | 0–127 | slider |
| 8 | Stages | 0–MAX_PHASER_STAGES | slider |
| 9 | Offset | 0–127 | slider |
| 10 | Subtract | 0/1 | toggle |
| 11 | Depth | 0–127 | slider |
| 12 | Hyper | 0/1 | toggle |

**LFO section:** P2–P5
**Special:** Barber-pole mode when LFO type=2.

---

## Index 19: Valve (class: `Valve`)
**Source:** [Valve.cpp](src/Valve.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LR Cross | 0–127 | slider |
| 3 | Drive | 0–127 | slider |
| 4 | Level | 0–127 | slider |
| 5 | Negate | 0/1 | toggle |
| 6 | LPF | freq (Hz) | slider |
| 7 | HPF | freq (Hz) | slider |
| 8 | Stereo | 0/1 | toggle |
| 9 | Prefiltering | 0/1 | toggle |
| 10 | Q | 0–127 | slider |
| 11 | Extra Distortion | 0–127 | slider |
| 12 | Presence | 0–127 | slider |

---

## Index 20: Dual Flange (class: `Dflange`)
**Source:** [Dual_Flange.cpp](src/Dual_Flange.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Wet/Dry | −64 to 64 | slider |
| 1 | Panning | −64 to 63 | slider |
| 2 | LR Cross | 0–127 | slider |
| 3 | Depth | freq (Hz) | slider |
| 4 | Width | freq (Hz) | slider |
| 5 | Offset | 0–127 | slider |
| 6 | Feedback | −64 to 64 | slider |
| 7 | Hi Damp | freq (Hz) | slider |
| 8 | Subtract | 0/1 | toggle |
| 9 | Zero | 0/1 | toggle |
| 10 | LFO Freq | 0–127 | slider |
| 11 | LFO Stereo | 0–127 | slider |
| 12 | LFO Type | choice | LFO waveform |
| 13 | LFO Randomness | 0–127 | slider |
| 14 | Intense | 0/1 | toggle |

**LFO section:** P10–P13 (non-standard order)

---

## Index 21: Ring (class: `Ring`)
**Source:** [Ring.cpp](src/Ring.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | −64 to 63 | slider |
| 1 | Panning | −64 to 63 | slider |
| 2 | LR Cross | −64 to 63 | slider |
| 3 | Level | 0–127 | slider |
| 4 | Depth % | 0–100 | slider |
| 5 | Freq | 1–20000 Hz | slider |
| 6 | Stereo | 0/1 | toggle |
| 7 | Sin | 0–127 | slider |
| 8 | Tri | 0–127 | slider |
| 9 | Saw | 0–127 | slider |
| 10 | Square | 0–127 | slider |
| 11 | Input | 0–127 | slider |
| 12 | Auto Freq | 0/1 | toggle |

---

## Index 22: Exciter (class: `Exciter`)
**Source:** [Exciter.cpp](src/Exciter.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Harmonic 1 | 0–127 | slider |
| 2 | Harmonic 2 | 0–127 | slider |
| 3 | Harmonic 3 | 0–127 | slider |
| 4 | Harmonic 4 | 0–127 | slider |
| 5 | Harmonic 5 | 0–127 | slider |
| 6 | Harmonic 6 | 0–127 | slider |
| 7 | Harmonic 7 | 0–127 | slider |
| 8 | Harmonic 8 | 0–127 | slider |
| 9 | Harmonic 9 | 0–127 | slider |
| 10 | Harmonic 10 | 0–127 | slider |
| 11 | LPF | freq (Hz) | slider |
| 12 | HPF | freq (Hz) | slider |

---

## Index 23: MBDist (class: `MBDist`)
**Source:** [MBDist.cpp](src/MBDist.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | LR Cross | 0–127 | slider |
| 3 | Drive | 0–127 | slider |
| 4 | Level | 0–127 | slider |
| 5 | Type Low | choice | waveshape type |
| 6 | Type Mid | choice | waveshape type |
| 7 | Type High | choice | waveshape type |
| 8 | Vol Low | 0–100 | slider |
| 9 | Vol Mid | 0–100 | slider |
| 10 | Vol High | 0–100 | slider |
| 11 | Negate | 0/1 | toggle |
| 12 | Cross 1 | freq (Hz) | slider |
| 13 | Cross 2 | freq (Hz) | slider |
| 14 | Stereo | 0/1 | toggle |

---

## Index 24: Arpie (class: `Arpie`)
**Source:** [Arpie.cpp](src/Arpie.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | Delay | 2–600 (BPM) | slider |
| 3 | LR Delay | 0–127 | slider |
| 4 | LR Cross | 0–127 | slider |
| 5 | Feedback | 0–127 | slider |
| 6 | Hi Damp | 0–127 | slider |
| 7 | Reverse | 0–127 | slider |
| 8 | Harmonics | 2–MAXHARMS | slider |
| 9 | Pattern | 0–6 | choice |
| 10 | Subdivision | choice | subdivision type |

---

## Index 25: Expander (class: `Expander`)
**Source:** [Expander.cpp](src/Expander.cpp)
**Note:** Uses `Expander_Change(np, value)` — params start at index 1.

| Param | Name | Range | Type |
|-------|------|-------|------|
| 1 | Threshold | dB (negative) | slider |
| 2 | Shape | 0–127 | slider |
| 3 | Attack | ms | slider |
| 4 | Decay | ms | slider |
| 5 | LPF | freq (Hz) | slider |
| 6 | HPF | freq (Hz) | slider |
| 7 | Level | dB | slider |

---

## Index 26: Shuffle (class: `Shuffle`)
**Source:** [Shuffle.cpp](src/Shuffle.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Gain Low | 0–127 | slider (center=64, getpar offsets by 64) |
| 2 | Gain Mid-Low | 0–127 | slider (center=64) |
| 3 | Gain Mid-High | 0–127 | slider (center=64) |
| 4 | Gain High | 0–127 | slider (center=64) |
| 5 | Cross 1 | freq (Hz) | slider |
| 6 | Cross 2 | freq (Hz) | slider |
| 7 | Cross 3 | freq (Hz) | slider |
| 8 | Cross 4 | freq (Hz) | slider |
| 9 | Q | 0–127 | slider (center=64, getpar offsets by 64) |
| 10 | E | 0/1 | toggle |

---

## Index 27: Synthfilter (class: `Synthfilter`)
**Source:** [Synthfilter.cpp](src/Synthfilter.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Distortion | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | 0–127 | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Width | 0–127 | slider |
| 7 | Feedback | −64 to 64 | slider |
| 8 | LP Stages | 0–MAX_SFILTER_STAGES | slider |
| 9 | HP Stages | 0–MAX_SFILTER_STAGES | slider |
| 10 | Subtract | 0/1 | toggle |
| 11 | Depth | 0–127 | slider |
| 12 | Envelope | 0–127 | slider |
| 13 | Attack | 5+ | slider |
| 14 | Release | 5+ | slider |
| 15 | Bandwidth | 0–127 | slider |

**LFO section:** P2–P5
**Special:** Envelope follower.

---

## Index 28: MBVvol (class: `MBVvol`)
**Source:** [MBVvol.cpp](src/MBVvol.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | LFO1 Freq | 0–127 | slider |
| 2 | LFO1 Type | choice | LFO waveform |
| 3 | LFO1 Stereo | 0–127 | slider |
| 4 | LFO2 Freq | 0–127 | slider |
| 5 | LFO2 Type | choice | LFO waveform |
| 6 | LFO2 Stereo | 0–127 | slider |
| 7 | Cross 1 | freq (Hz) | slider |
| 8 | Cross 2 | freq (Hz) | slider |
| 9 | Cross 3 | freq (Hz) | slider |
| 10 | Combi | choice | combination mode |

**Special:** Two separate LFO sections (LFO1: P1–P3, LFO2: P4–P6).

---

## Index 29: Convolotron (class: `Convolotron`)
**Source:** [Convolotron.cpp](src/Convolotron.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | Safe | 0/1 | toggle |
| 3 | Length | ms | slider |
| 4 | User | 0/1 | toggle (user file mode) |
| 5 | (unused) | — | — |
| 6 | Hi Damp | 0–127 | slider |
| 7 | Level | 0–127 | slider |
| 8 | File | file index | file selector |
| 9 | (unused) | — | — |
| 10 | Feedback | signed | slider |

**Special:** File selector (P8 selects reverb IR file, P4 toggles user file mode).

---

## Index 30: Looper (class: `Looper`)
**Source:** [Looper.cpp](src/Looper.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Play | 0/1 | button/toggle |
| 2 | Stop | 0/1 | button/toggle |
| 3 | Record | 0/1 | button/toggle |
| 4 | Clear | 0/1 | button |
| 5 | Reverse | 0/1 | toggle |
| 6 | Fade 1 | 0–127 | slider |
| 7 | Track 1 | 0/1 | toggle |
| 8 | Track 2 | 0/1 | toggle |
| 9 | Auto Play | 0/1 | toggle |
| 10 | Fade 2 | 0–127 | slider |
| 11 | Rec 1 | 0/1 | toggle |
| 12 | Rec 2 | 0/1 | toggle |
| 13 | Link | 0/1 | toggle |
| 14 | Tempo | BPM | slider |
| 15 | Bar | value | slider |
| 16 | Metro | 0/1 | toggle |
| 17 | Meter/Subdivision | choice | meter mode |

**Special:** Transport controls (Play/Stop/Record/Clear as buttons, not sliders).

---

## Index 31: RyanWah (class: `RyanWah`)
**Source:** [RyanWah.cpp](src/RyanWah.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Q | 0–127 | slider |
| 2 | LFO Freq | 0–127 | slider |
| 3 | LFO Randomness | (forced 0) | slider |
| 4 | LFO Type | choice | LFO waveform |
| 5 | LFO Stereo | 0–127 | slider |
| 6 | Width | 0–127 | slider |
| 7 | Amp Sensitivity | 0–127 | slider |
| 8 | Amp Sens Inverse | 0/1 | toggle |
| 9 | Amp Smooth | 0–127 | slider |
| 10 | LP Mix | 0–127 | slider |
| 11 | BP Mix | 0–127 | slider |
| 12 | HP Mix | 0–127 | slider |
| 13 | Stages | 1+ | slider |
| 14 | Range | freq (Hz) | slider |
| 15 | Min Freq | freq (Hz) | slider |
| 16 | Variable Q | 0–127 | slider |
| 17 | Mode | choice | wah mode |
| 18 | Preset | preset index | preset selector |

**LFO section:** P2–P5
**Special:** Filter mix controls (LP/BP/HP).

---

## Index 32: RBEcho (class: `RBEcho`)
**Source:** [RBEcho.cpp](src/RBEcho.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | Delay | BPM tempo | slider |
| 3 | LR Delay | 0–127 | slider |
| 4 | LR Cross | 0–127 | slider |
| 5 | Feedback | 0–127 | slider |
| 6 | Hi Damp | 0–127 | slider |
| 7 | Reverse | 0–127 | slider |
| 8 | Subdivision | choice | subdivision type |
| 9 | Extra Stereo | 0–127 | slider |

---

## Index 33: CoilCrafter (class: `CoilCrafter`)
**Source:** [CoilCrafter.cpp](src/CoilCrafter.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Pickup Origin (Ppo) | choice | pickup type preset (0=off, 1-8) |
| 2 | Pickup Destination (Ppd) | choice | pickup type preset (0=off, 1-8) |
| 3 | Freq 1 | freq (Hz) | slider |
| 4 | Q 1 | ×10 (int) | slider |
| 5 | Freq 2 | freq (Hz) | slider |
| 6 | Q 2 | ×10 (int) | slider |
| 7 | Tone | 0–127 | slider |
| 8 | Mode | 0/1 | toggle |

**Note:** P1/P2 select from preset pickup frequency/Q tables. P3–P6 allow manual override.

---

## Index 34: ShelfBoost (class: `ShelfBoost`)
**Source:** [ShelfBoost.cpp](src/ShelfBoost.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Q | 0–127 | slider |
| 2 | Freq | freq (Hz) | slider |
| 3 | Stereo | 0/1 | toggle |
| 4 | Level | 0–127 | slider |

---

## Index 35: Vocoder (class: `Vocoder`)
**Source:** [Vocoder.cpp](src/Vocoder.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Panning | 0–127 | slider |
| 2 | Muffle | 0–127 | slider |
| 3 | Q | 0–127 | slider |
| 4 | Input | 0–127 | slider |
| 5 | Level | 0–127 | slider |
| 6 | Ring | 0–127 | slider |

**Special:** Requires auxiliary input signal. VU meter level output available.

---

## Index 36: Sustainer (class: `Sustainer`)
**Source:** [Sustainer.cpp](src/Sustainer.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Sustain | 0–127 | slider |

---

## Index 37: Sequence (class: `Sequence`)
**Source:** [Sequence.cpp](src/Sequence.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Sequence Step 1 | 0–127 | slider |
| 1 | Sequence Step 2 | 0–127 | slider |
| 2 | Sequence Step 3 | 0–127 | slider |
| 3 | Sequence Step 4 | 0–127 | slider |
| 4 | Sequence Step 5 | 0–127 | slider |
| 5 | Sequence Step 6 | 0–127 | slider |
| 6 | Sequence Step 7 | 0–127 | slider |
| 7 | Sequence Step 8 | 0–127 | slider |
| 8 | Volume | 0–127 | slider |
| 9 | Tempo | BPM | slider |
| 10 | Q | 0–127 | slider (also controls panning) |
| 11 | Amplitude | 0/1 | toggle |
| 12 | Stereo Diff | 0–7 | slider (step offset) |
| 13 | Mode | choice | (0=UpDown, 1=UpDown2, 2=Stepper, 3=Shifter, 4=Tremor, 5=Arpegiator, 6=Chorus) |
| 14 | Range | choice | frequency range preset |

**Special:** 8-step sequencer (P0–P7).

---

## Index 38: Shifter (class: `Shifter`)
**Source:** [Shifter.cpp](src/Shifter.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Pan | 0–127 | slider |
| 2 | Gain | 0–127 | slider |
| 3 | Attack | ms | slider |
| 4 | Decay | ms | slider |
| 5 | Threshold | dB (negative) | slider |
| 6 | Interval | semitones | slider |
| 7 | Up/Down | 0/1 | toggle |
| 8 | Mode | choice | (0=triggered, 1=whammy, 2=triggered-reverse) |
| 9 | Whammy | 0–127 | slider |

---

## Index 39: StompBox (class: `StompBox`)
**Source:** [StompBox.cpp](src/StompBox.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | High | signed (−64 to 64) | slider |
| 2 | Mid | signed (−64 to 64) | slider |
| 3 | Low | signed (−64 to 64) | slider |
| 4 | Gain | 0–127 | slider |
| 5 | Mode | choice | (0=Odie, 1=Grunge, 2=Rat, 3=FatCat, 4=Dist+, 5=DeathMetal, 6=MetalZone, 7=ClassicFuzz) |

---

## Index 40: Reverbtron (class: `Reverbtron`)
**Source:** [Reverbtron.cpp](src/Reverbtron.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Fade | 0–127 | slider |
| 2 | Safe | 0/1 | toggle |
| 3 | Length | 0–2000 | slider |
| 4 | User | 0/1 | toggle (user file mode) |
| 5 | Initial Delay | ms | slider |
| 6 | Hi Damp | 0–127 | slider |
| 7 | Level | 0–127 | slider |
| 8 | File | file index | file selector |
| 9 | Stretch | 0–127 | slider (center=64) |
| 10 | Feedback | signed | slider |
| 11 | Panning | 0–127 | slider |
| 12 | Extra Stereo | 0/1 | toggle |
| 13 | Reverse | 0/1 | toggle |
| 14 | LPF | freq (Hz) | slider |
| 15 | Diffusion | 0–127 | slider |

**Special:** File selector (P8 for reverb IR file, P4 for user file mode).

---

## Index 41: Echotron (class: `Echotron`)
**Source:** [Echotron.cpp](src/Echotron.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Depth | 0–127 | slider |
| 2 | Width | 0–127 | slider |
| 3 | Length | 0–127 | slider |
| 4 | User | 0/1 | toggle (user file mode) |
| 5 | Tempo | BPM | slider |
| 6 | Hi Damp | 0–127 | slider |
| 7 | LR Cross | 0–127 | slider |
| 8 | File | file index | file selector |
| 9 | LFO Stereo | 0–127 | slider |
| 10 | Feedback | signed | slider |
| 11 | Panning | 0–127 | slider |
| 12 | Mod Delay | 0/1 | toggle |
| 13 | Mod Filters | 0/1 | toggle |
| 14 | LFO Type | choice | LFO waveform |
| 15 | Filters | 0/1 | toggle (filter delay line on/off) |

**Special:** File selector (P8 for delay pattern file, P4 for user mode). LFO section (P9, P14).

---

## Index 42: StereoHarm (class: `StereoHarm`)
**Source:** [StereoHarm.cpp](src/StereoHarm.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Gain L | 0–127 | slider |
| 2 | Interval L | 0–24 | slider (semitones, 12=unison) |
| 3 | Chrome L | signed | slider (fine tune) |
| 4 | Gain R | 0–127 | slider |
| 5 | Interval R | 0–24 | slider (semitones) |
| 6 | Chrome R | signed | slider (fine tune) |
| 7 | Select | 0/1 | toggle |
| 8 | Note | choice | root note |
| 9 | Type | choice | harmonizer type |
| 10 | MIDI | 0/1 | toggle |
| 11 | LR Cross | 0–127 | slider |

**Special:** Dual independent pitch shifters (L/R channels).

---

## Index 43: CompBand (class: `CompBand`)
**Source:** [CompBand.cpp](src/CompBand.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Low Ratio | 0–127 | slider |
| 2 | Mid-Low Ratio | 0–127 | slider |
| 3 | Mid-High Ratio | 0–127 | slider |
| 4 | High Ratio | 0–127 | slider |
| 5 | Low Threshold | dB (signed) | slider |
| 6 | Mid-Low Threshold | dB (signed) | slider |
| 7 | Mid-High Threshold | dB (signed) | slider |
| 8 | High Threshold | dB (signed) | slider |
| 9 | Cross 1 | freq (Hz) | slider |
| 10 | Cross 2 | freq (Hz) | slider |
| 11 | Cross 3 | freq (Hz) | slider |
| 12 | Level | 0–127 | slider |

---

## Index 44: Opticaltrem (class: `Opticaltrem`)
**Source:** [Opticaltrem.cpp](src/Opticaltrem.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Depth | 0–127 | slider |
| 1 | LFO Freq | 0–127 | slider |
| 2 | LFO Randomness | 0–127 | slider |
| 3 | LFO Type | choice | LFO waveform |
| 4 | LFO Stereo | 0–127 | slider |
| 5 | Pan | 0–127 | slider |
| 6 | Invert | 0/1 | toggle |

**LFO section:** P1–P4

---

## Index 45: Vibe (class: `Vibe`)
**Source:** [Vibe.cpp](src/Vibe.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Width | 0–127 | slider |
| 1 | LFO Freq | 0–127 | slider |
| 2 | LFO Randomness | 0–127 | slider |
| 3 | LFO Type | choice | LFO waveform |
| 4 | LFO Stereo | 0–127 | slider |
| 5 | Pan | 0–127 | slider |
| 6 | Volume | 0–127 | slider |
| 7 | Feedback | 0–127 | slider (center=64) |
| 8 | Depth | 0–127 | slider |
| 9 | LR Cross | 0–127 | slider (center=64) |
| 10 | Stereo | 0/1 | toggle |

**LFO section:** P1–P4

---

## Index 46: Infinity (class: `Infinity`)
**Source:** [Infinity.cpp](src/Infinity.cpp)

| Param | Name | Range | Type |
|-------|------|-------|------|
| 0 | Volume | 0–127 | slider |
| 1 | Band 1 Level | −64 to 64 | slider |
| 2 | Band 2 Level | −64 to 64 | slider |
| 3 | Band 3 Level | −64 to 64 | slider |
| 4 | Band 4 Level | −64 to 64 | slider |
| 5 | Band 5 Level | −64 to 64 | slider |
| 6 | Band 6 Level | −64 to 64 | slider |
| 7 | Band 7 Level | −64 to 64 | slider |
| 8 | Band 8 Level | −64 to 64 | slider |
| 9 | Q | signed | slider |
| 10 | Start Freq | 0–127 | slider (20–6020 Hz) |
| 11 | End Freq | 0–127 | slider (20–6020 Hz) |
| 12 | Rate | BPM | slider |
| 13 | Stereo Diff | signed | slider |
| 14 | Subdivision | signed | slider |
| 15 | AutoPan | 0–127 | slider |
| 16 | Reverse | 0/1 | toggle |
| 17 | Stages | 1+ | slider (stored as value-1 internally) |

**Special:** 8 individually controllable filter bands (P1–P8). Barber-pole phaser effect.

---

## Common Patterns

### LFO Section
Many effects share an LFO block with these params (typically at indices 2–5 unless noted):
- **Freq** — LFO rate (0–127)
- **Randomness** — LFO randomness amount (0–127)
- **LFO Type** — Waveform choice (Sine, Tri, Ramp Up, Ramp Down, Exp Up, Exp Down, S&H)
- **Stereo** — LFO stereo spread (0–127)

Effects with LFO: Chorus, Phaser, Alienwah, Pan, WahWah/DynamicFilter, APhaser, Synthfilter, Dual Flange (P10–P13), RyanWah, MBVvol (two LFOs), Opticaltrem (P1–P4), Vibe (P1–P4), Echotron (P9/P14).

### File Selector Pattern
Effects with file selectors: Convolotron (P8), Reverbtron (P8), Echotron (P8).
All use a `User` toggle param (P4) to switch between built-in files and user-provided files.

### Non-Standard `changepar` Methods
Three effects use renamed method signatures with params starting at index 1:
- **Compressor** → `Compressor_Change(np, value)`
- **Gate** → `Gate_Change(np, value)`
- **Expander** → `Expander_Change(np, value)`

### Shared Classes (Same Params, Different Instance)
- Index 3 (Overdrive) = Index 2 (Distorsion) — same `Distorsion` class
- Index 7 (Flanger) = Index 5 (Chorus) — same `Chorus` class
- Index 9 (EQ2) = Index 0 (EQ1) — same `EQ` class
- Index 12 (Cabinet) = `EQ` class with special preset-only configuration

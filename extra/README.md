# Extra Tools

Command-line utilities for converting and generating rakarrack data files.

## rakconvert

Converts **old-format** rakarrack bank files (pre-0.5.0, 82-preset layout with
24×22 parameter arrays) to the current bank format (62-preset layout with
70×20 parameter arrays). Also imports the associated `.ml` MIDI-learn file
if present alongside the bank.

The converter remaps LFO tempos, delay times, and filter frequencies from
the old 0–127 parameter encoding to the new direct-value encoding.

**Usage:**
```
rakconvert -c <bankfile>
```

**Arguments:**

| Flag | Long | Description |
|------|------|-------------|
| `-c` | `--convert` | Path to the old `.rkrb` bank file to convert |
| `-h` | `--help` | Display usage information and exit |

**Output:** Two new bank files written next to the input:
- `<name>01_050.rkrb` — presets 1–60
- `<name>02_050.rkrb` — presets 61–80

---

## rakgit2new

Converts **intermediate git-era** bank files (50×20 parameter arrays) to the
current bank format (70×20 parameter arrays). Use this for banks created with
development builds between the old and current formats.

**Usage:**
```
rakgit2new -c <bankfile>
```

**Arguments:**

| Flag | Long | Description |
|------|------|-------------|
| `-c` | `--convert` | Path to the git-era `.rkrb` bank file to convert |
| `-h` | `--help` | Display usage information and exit |

**Output:** A new bank file written as `<name>_new.rkrb`.

---

## rakverb

Converts a reverb impulse response **WAV file** into the rakarrack `.rvb`
text format used by the Reverbtron effect. The conversion works by detecting
zero-crossings in the audio, accumulating energy between them, and
sub-sampling the result to approximately 1500 data points.

**Usage:**
```
rakverb -i <input.wav> [-o <output.rvb>]
```

**Arguments:**

| Flag | Long | Description |
|------|------|-------------|
| `-i` | `--input` | Input WAV file (impulse response) |
| `-o` | `--output` | Output `.rvb` file (optional; defaults to `<input>.rvb`) |
| `-h` | `--help` | Display usage information and exit |

**Output:** A `.rvb` text file containing a header line, compression/quality
metrics, a data length, and time/amplitude pairs — one per line.

---

## rakverb2

An improved version of `rakverb` with additional control over the
conversion process. Adds parameters for controlling the number of
reflections (data points) in the output and a minimum time difference
between consecutive data points, allowing finer control over the
compression and quality of the resulting `.rvb` file.

**Usage:**
```
rakverb2 -i <input.wav> [-o <output.rvb>] [-l <length>] [-t <time_diff>]
```

**Arguments:**

| Flag | Long | Description |
|------|------|-------------|
| `-i` | `--input` | Input WAV file (impulse response) |
| `-o` | `--output` | Output `.rvb` file (optional; defaults to `<input>.rvb`) |
| `-l` | `--length` | Target number of reflections/data points (default: 1500) |
| `-t` | `--time` | Minimum time difference between data points in seconds (default: 0.0) |
| `-h` | `--help` | Display usage information and exit |

**Output:** Same `.rvb` format as `rakverb`, but with controllable length
and time resolution.

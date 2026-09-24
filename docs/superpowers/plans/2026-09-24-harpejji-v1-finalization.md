# Harpejji Mode V1 Finalization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Finalize LinnStrument OS 2.3.4-x7 Harpejji Mode with efficient exact-note LED feedback, temporary One Channel X policy, eight playable rows, and the approved Global Settings switch behavior.

**Architecture:** Keep the existing Harpejji pitch source and stock touch/MIDI engines. Add small Harpejji policy helpers for inverse note lookup, effective X state, and Low Row suppression; route existing LED, settings, touch-transfer, and MIDI-control entry points through those helpers only while Harpejji is active.

**Tech Stack:** Arduino/C++ for SAM3X8E (Arduino Due), existing LinnStrument `.ino` modules, C++11 host tests, Python source-wiring regression tests, project-local Arduino CLI build environment.

**Spec:** `docs/superpowers/specs/2026-09-24-harpejji-v1-finalization-design.md`

## Global Constraints

- Work only in the current `feature/harpejji-v1` project worktree and keep every generated artifact inside the repository.
- Build incrementally from the current `feature/harpejji-v1` worktree; do not restart from official firmware 2.3.4.
- Preserve all existing x6 features and the current x7 Harpejji implementation.
- Keep `OSVersion` and `OSVersionBuild` equal to `234-x7` for this test iteration.
- Do not change sensor acquisition, raw X/Y/Z definitions, calibration, phantom detection, or debounce.
- Do not write temporary One Channel X or Low Row override state to EEPROM/flash.
- Do not flash hardware, push GitHub, create a release, or tag a version without a separate explicit instruction.
- Treat automated source/build checks and physical LinnStrument verification as separate acceptance stages.

---

### Task 1: Preserve and characterize the current x7 baseline

**Files:**
- Inspect: all currently modified and untracked Harpejji source files
- Test: `tests/harpejji_pitch_test.cpp`
- Test: `tests/midi_clock_sleep_recovery_test.py`
- Output: `build/harpejji-v1-baseline/linnstrument-firmware.ino.bin`

**Interfaces:**
- Consumes: current uncommitted x7 Harpejji implementation on `feature/harpejji-v1`
- Produces: a reproducible baseline binary, checksum, status record, and local checkpoint commit before behavioral changes

- [ ] **Step 1: Record the exact baseline state**

Run:

```bash
git status --short
git diff --check
git diff --stat
git branch --show-current
rg -n 'OSVersion(Build)?\s*=' linnstrument-firmware.ino
```

Expected: branch `feature/harpejji-v1`, version `234-x7`, no whitespace errors, and only the known Harpejji worktree changes.

- [ ] **Step 2: Run the existing host regressions**

Run:

```bash
c++ -std=c++11 -Wall -Wextra -Werror -I. tests/harpejji_pitch_test.cpp -o build/harpejji-pitch-test
build/harpejji-pitch-test
python3 tests/midi_clock_sleep_recovery_test.py
```

Expected: every test exits zero.

- [ ] **Step 3: Connect the project-contained offline build environment**

The managed worktree does not duplicate the large offline Arduino directories. Point it at the copies already retained in the same repository root:

```bash
test -e .tools || ln -s ../../../.tools .tools
test -e .arduino-data || ln -s ../../../.arduino-data .arduino-data
test -x .tools/arduino-cli/arduino-cli
test -d .arduino-data/packages/arduino/hardware/sam
```

Expected: both links resolve within the LinnStrument project and the Arduino CLI plus SAM package are available. Do not download dependencies and do not create links outside the project.

- [ ] **Step 4: Build the unchanged baseline**

Run:

```bash
scripts/compile-firmware.sh harpejji-v1-baseline
shasum -a 256 build/harpejji-v1-baseline/linnstrument-firmware.ino.bin
stat -f '%z bytes' build/harpejji-v1-baseline/linnstrument-firmware.ino.bin
```

Expected: Arduino Due build succeeds and the baseline binary has a recorded size and SHA-256.

- [ ] **Step 5: Create a local safety checkpoint**

Run:

```bash
git add linnstrument-firmware.ino ls_harpejji.h ls_harpejji.ino ls_displayModes.ino ls_extstorage.ino ls_handleTouches.ino ls_settings.ino tests/harpejji_pitch_test.cpp
git commit -m "feat: add Harpejji mode x7 baseline"
```

Expected: the current x7 implementation can be recovered independently from subsequent finalization work. Do not push this commit.

---

### Task 2: Add the resource-efficient inverse Harpejji note mapping

**Files:**
- Modify: `ls_harpejji.h`
- Modify: `tests/harpejji_pitch_test.cpp`

**Interfaces:**
- Consumes: `harpejjiNoteNumber(...)`
- Produces: `int16_t harpejjiNoteColumn(int16_t basePitch, int16_t row, int16_t midiNote, int16_t transposeOctave, int16_t transposePitch, int16_t transposeLights, int16_t numCols, bool leftHanded)`

- [ ] **Step 1: Write failing inverse-mapping tests**

Add assertions covering normal orientation, every row, octave/pitch/light transpose, exact octave identity, and left-handed reversal:

```cpp
assert(harpejjiNoteColumn(30, 0, 30, 0, 0, 0, 26, false) == 1);
assert(harpejjiNoteColumn(30, 7, 44, 0, 0, 0, 26, false) == 1);
assert(harpejjiNoteColumn(30, 3, 48, 12, 0, 0, 26, false) == 1);
assert(harpejjiNoteColumn(30, 0, 30, 0, 0, 0, 26, true) == 25);
assert(harpejjiNoteColumn(30, 0, 42, 0, 0, 0, 26, false) == 13);
```

Also round-trip every playable row/column pair through `harpejjiNoteNumber()` and `harpejjiNoteColumn()`.

- [ ] **Step 2: Run the host test and verify the new API is missing**

Run:

```bash
c++ -std=c++11 -Wall -Wextra -Werror -I. tests/harpejji_pitch_test.cpp -o build/harpejji-pitch-test
```

Expected: compilation fails because `harpejjiNoteColumn` is not defined.

- [ ] **Step 3: Implement the inverse using integer arithmetic**

Add to `ls_harpejji.h`:

```cpp
inline int16_t harpejjiNoteColumn(int16_t basePitch,
                                  int16_t row,
                                  int16_t midiNote,
                                  int16_t transposeOctave,
                                  int16_t transposePitch,
                                  int16_t transposeLights,
                                  int16_t numCols,
                                  bool leftHanded) {
  int16_t rowBase = basePitch + (row * 2) + transposeOctave +
                    transposePitch - transposeLights;
  int16_t noteCol = midiNote - rowBase + 1;
  return leftHanded ? numCols - noteCol : noteCol;
}
```

- [ ] **Step 4: Run the round-trip tests**

Run:

```bash
c++ -std=c++11 -Wall -Wextra -Werror -I. tests/harpejji_pitch_test.cpp -o build/harpejji-pitch-test
build/harpejji-pitch-test
```

Expected: all forward and inverse pitch tests pass without warnings.

- [ ] **Step 5: Commit the pure mapping unit**

```bash
git add ls_harpejji.h tests/harpejji_pitch_test.cpp
git commit -m "test: cover inverse Harpejji pitch mapping"
```

---

### Task 3: Route Played Same LEDs through the Harpejji mapping

**Files:**
- Modify: `ls_harpejji.ino`
- Modify: `ls_midi.ino`
- Create: `tests/harpejji_source_wiring_test.py`

**Interfaces:**
- Consumes: `harpejjiNoteColumn(...)`, split boundaries, handedness, octave/pitch/light transpose
- Produces: `short getHarpejjiNoteNumColumn(byte split, byte midiNote, byte row)` and a Harpejji branch in `getNoteNumColumn(...)`

- [ ] **Step 1: Write a failing source-wiring regression**

Create a Python test that asserts:

```python
from pathlib import Path

root = Path(__file__).resolve().parents[1]
midi = (root / "ls_midi.ino").read_text()
harpejji = (root / "ls_harpejji.ino").read_text()

assert "getHarpejjiNoteNumColumn" in harpejji
assert "if (isHarpejjiLayoutActive())" in midi
assert "return getHarpejjiNoteNumColumn(split, notenum, row);" in midi
```

- [ ] **Step 2: Run it and verify failure**

Run:

```bash
python3 tests/harpejji_source_wiring_test.py
```

Expected: failure because the inverse wrapper and MIDI routing do not exist.

- [ ] **Step 3: Add the split-aware inverse wrapper**

Implement in `ls_harpejji.ino`:

```cpp
short getHarpejjiNoteNumColumn(byte split, byte midiNote, byte row) {
  short col = harpejjiNoteColumn(30, row, midiNote,
                                 Split[split].transposeOctave,
                                 Split[split].transposePitch,
                                 Split[split].transposeLights,
                                 NUMCOLS,
                                 isLeftHandedSplit(split));
  byte lowCol, highCol;
  getSplitBoundaries(split, lowCol, highCol);
  if (col < lowCol || col >= highCol) return -1;
  return col;
}
```

- [ ] **Step 4: Route existing highlight and reset operations through it**

At the start of `getNoteNumColumn(...)` in `ls_midi.ino`, add:

```cpp
if (isHarpejjiLayoutActive()) {
  return getHarpejjiNoteNumColumn(split, notenum, row);
}
```

This automatically corrects `highlightPossibleNoteCells`, `resetPossibleNoteCells`, incoming MIDI highlights, and Channel Per Row exact-cell resets without adding a full-surface scan.

- [ ] **Step 5: Run host, wiring, and compile checks**

Run:

```bash
build/harpejji-pitch-test
python3 tests/harpejji_source_wiring_test.py
scripts/compile-firmware.sh harpejji-led-map
```

Expected: all pass; build output reports success.

- [ ] **Step 6: Commit the exact-note LED mapping**

```bash
git add ls_harpejji.ino ls_midi.ino tests/harpejji_source_wiring_test.py
git commit -m "fix: map played-note LEDs in Harpejji mode"
```

---

### Task 4: Implement temporary One Channel X policy

**Files:**
- Modify: `linnstrument-firmware.ino`
- Modify: `ls_harpejji.ino`
- Modify: `ls_handleTouches.ino`
- Modify: `ls_settings.ino`
- Modify: `ls_switches.ino`
- Modify: `ls_displayModes.ino`
- Modify: `ls_midi.ino`
- Modify: `tests/harpejji_source_wiring_test.py`

**Interfaces:**
- Produces: `boolean harpejjiOneChannelXEnabled[2]`
- Produces: `void resetHarpejjiOneChannelX(byte split)`
- Produces: `boolean effectiveSendX(byte split)`
- Produces: `void setEffectiveSendX(byte split, boolean enabled)`
- Produces: `void toggleEffectiveSendX(byte split)`
- Consumes: current layout mode, `Split[split].midiMode`, and saved `Split[split].sendX`

- [ ] **Step 1: Extend the source-wiring test with runtime-policy expectations**

Assert that the new helpers exist and that touch-transfer, Pitch/X settings, the control-switch shortcut, display state, and NRPN read/write paths call them instead of directly using `Split[split].sendX` for live behavior.

- [ ] **Step 2: Run the wiring test and verify failure**

```bash
python3 tests/harpejji_source_wiring_test.py
```

Expected: failure on missing policy helpers.

- [ ] **Step 3: Add runtime-only state and policy helpers**

Implement the following behavior in `ls_harpejji.ino`:

```cpp
boolean harpejjiOneChannelXEnabled[2] = { false, false };

void resetHarpejjiOneChannelX(byte split) {
  harpejjiOneChannelXEnabled[split] = false;
}

boolean effectiveSendX(byte split) {
  if (Device.harpejjiLayoutEnabled && Split[split].midiMode == oneChannel) {
    return harpejjiOneChannelXEnabled[split];
  }
  return Split[split].sendX;
}

void setEffectiveSendX(byte split, boolean enabled) {
  if (Device.harpejjiLayoutEnabled && Split[split].midiMode == oneChannel) {
    harpejjiOneChannelXEnabled[split] = enabled;
  }
  else {
    Split[split].sendX = enabled;
  }
}

void toggleEffectiveSendX(byte split) {
  setEffectiveSendX(split, !effectiveSendX(split));
}
```

Keep the array outside `DeviceSettings`; it must never be serialized.

- [ ] **Step 4: Reset temporary X state at the required transitions**

When entering Harpejji, reset both splits to false for future One Channel use. When a split changes into One Channel through the settings UI, NRPN, or preset loading, reset that split. Exiting Harpejji requires no saved-value restoration because `Split.sendX` was never modified.

- [ ] **Step 5: Route touch and transfer behavior through effective X**

Replace live-performance checks of `Split[sensorSplit].sendX` with `effectiveSendX(sensorSplit)` in:

- cross-cell slide eligibility;
- Harpejji X contribution to Pitch Bend;
- new-note bend reset and release reset conditions;
- any Harpejji performance branch that distinguishes retrigger from transfer.

Do not replace storage-copy code or non-Harpejji preset initialization.

- [ ] **Step 6: Route all user controls through the effective policy**

- Per-Split Pitch/X setting: call `toggleEffectiveSendX(Global.currentPerSplit)`.
- Pitch/X control-switch shortcut: call the same helper.
- NRPN Pitch/X write: call `setEffectiveSendX(split, value)`.
- NRPN Pitch/X read and Per-Split LED: report `effectiveSendX(split)`.

Expected Harpejji + One Channel UI: Pitch/X initially appears off, can be switched on and off during the session, and does not overwrite the saved normal-layout preference.

- [ ] **Step 7: Verify X-off and X-on code paths**

Run:

```bash
python3 tests/harpejji_source_wiring_test.py
c++ -std=c++11 -Wall -Wextra -Werror -I. tests/harpejji_pitch_test.cpp -o build/harpejji-pitch-test
build/harpejji-pitch-test
scripts/compile-firmware.sh harpejji-x-policy
```

Expected: all pass. Static review must show that Harpejji + One Channel + effective X off cannot enter cross-cell transfer, while manual X on reuses the stock transfer and pitch-correction path.

- [ ] **Step 8: Commit the runtime X policy**

```bash
git add linnstrument-firmware.ino ls_harpejji.ino ls_handleTouches.ino ls_settings.ino ls_switches.ino ls_displayModes.ino ls_midi.ino tests/harpejji_source_wiring_test.py
git commit -m "feat: add temporary Harpejji One Channel X policy"
```

---

### Task 5: Suppress Low Row special behavior only during Harpejji performance

**Files:**
- Modify: `ls_harpejji.ino`
- Modify: `ls_handleTouches.ino`
- Modify: `ls_displayModes.ino`
- Modify: `tests/harpejji_source_wiring_test.py`

**Interfaces:**
- Produces: `boolean lowRowSpecialBehaviorActive(byte split)`
- Consumes: `isHarpejjiLayoutActive()` and saved `Split[split].lowRowMode`

- [ ] **Step 1: Add failing source-wiring assertions**

Require a helper whose effective rule is:

```cpp
boolean lowRowSpecialBehaviorActive(byte split) {
  return !isHarpejjiLayoutActive() && Split[split].lowRowMode != lowRowNormal;
}
```

Require all performance-only Low Row branches in touch handling and normal-display painting to use this helper.

- [ ] **Step 2: Run the wiring test and verify failure**

```bash
python3 tests/harpejji_source_wiring_test.py
```

Expected: failure until the helper and call sites exist.

- [ ] **Step 3: Implement the effective Low Row policy**

Add `lowRowSpecialBehaviorActive(byte split)` to `ls_harpejji.ino`. Do not modify `Split[split].lowRowMode` and do not add a persisted field.

- [ ] **Step 4: Audit and gate every performance-only Low Row branch**

Use:

```bash
rg -n "isLowRow\(|lowRowMode|lowRowRequiresSlideTracking|allowNewTouchOnLowRow" ls_handleTouches.ino ls_displayModes.ino
```

For note creation, release, slide tracking, expression, played LEDs, and normal performance painting, treat row 0 as a normal note row whenever the helper is false. Leave Per-Split Settings painting and editing untouched so the saved Low Row choice remains visible and editable.

- [ ] **Step 5: Clear stale Low Row LED layers on Harpejji entry**

Ensure the normal Harpejji repaint clears `LED_LAYER_LOWROW` for row 0 before drawing Main/Accent note lighting, so a prior fader/bend display cannot remain visible.

- [ ] **Step 6: Run regressions and build**

```bash
python3 tests/harpejji_source_wiring_test.py
build/harpejji-pitch-test
python3 tests/midi_clock_sleep_recovery_test.py
scripts/compile-firmware.sh harpejji-low-row
```

Expected: all pass; no serialized configuration structure changes.

- [ ] **Step 7: Commit the Low Row performance override**

```bash
git add ls_harpejji.ino ls_handleTouches.ino ls_displayModes.ino tests/harpejji_source_wiring_test.py
git commit -m "feat: use all eight rows in Harpejji mode"
```

---

### Task 6: Move and restyle the Global Settings entry

**Files:**
- Modify: `linnstrument-firmware.ino`
- Modify: `ls_displayModes.ino`
- Modify: `ls_settings.ino` only if coordinate-specific logic exists outside the shared constants
- Modify: `tests/harpejji_source_wiring_test.py`

**Interfaces:**
- Consumes: Tap Tempo beat event already mirrored by the 3x4 switch
- Produces: Harpejji switch at `(20, 0)`, off-state beat flash, on-state steady white

- [ ] **Step 1: Add failing coordinate and color assertions**

Require:

```cpp
#define SCALAR_LAYOUT_SETTINGS_COL 19
#define SCALAR_LAYOUT_SETTINGS_ROW 0
#define HARPEJJI_LAYOUT_SETTINGS_COL 20
#define HARPEJJI_LAYOUT_SETTINGS_ROW 0
```

Also require the active Harpejji indicator to use `COLOR_WHITE` and the off-state flash to be driven inside the same Tap Tempo `flash_on` block as the 3x4 indicator.

- [ ] **Step 2: Run the wiring test and verify failure**

```bash
python3 tests/harpejji_source_wiring_test.py
```

Expected: failure because the current Harpejji coordinate is `(19, 1)` and active color is orange.

- [ ] **Step 3: Change the shared constants and active color**

Set Harpejji to column 20, row 0 and change its enabled indicator to steady white. Keep the off-state beat-flash event and duration identical to the 3x4 indicator.

- [ ] **Step 4: Verify the destination does not collide with stock Global Settings logic**

Run:

```bash
rg -n "sensorCol == 20|setLed\(20, 0|clearLed\(20, 0|col == 20" *.ino
```

Expected: only the intended custom-layout references use this performance cell on the Global Settings surface.

- [ ] **Step 5: Build and commit**

```bash
python3 tests/harpejji_source_wiring_test.py
scripts/compile-firmware.sh harpejji-entry
git add linnstrument-firmware.ino ls_displayModes.ino ls_settings.ino tests/harpejji_source_wiring_test.py
git commit -m "feat: finalize Harpejji Global Settings indicator"
```

Expected: build succeeds; Harpejji and 3x4 remain mutually exclusive.

---

### Task 7: Full automated regression and final test binary

**Files:**
- Verify: all modified source and tests
- Output: `build/harpejji-v1-final/linnstrument-firmware.ino.bin`

**Interfaces:**
- Consumes: completed Tasks 1-6
- Produces: source/build-validated x7 test binary and audit report

- [ ] **Step 1: Run every host regression from a clean test executable**

```bash
c++ -std=c++11 -Wall -Wextra -Werror -I. tests/harpejji_pitch_test.cpp -o build/harpejji-pitch-test
build/harpejji-pitch-test
python3 tests/harpejji_source_wiring_test.py
python3 tests/midi_clock_sleep_recovery_test.py
```

Expected: all exit zero.

- [ ] **Step 2: Perform the final Arduino Due build**

```bash
scripts/compile-firmware.sh harpejji-v1-final
```

Expected: `BUILD SUCCESS` and `build/harpejji-v1-final/linnstrument-firmware.ino.bin` exists.

- [ ] **Step 3: Verify version, checksum, size, and inclusion**

```bash
rg -n 'OSVersion(Build)?\s*=.*234-x7' linnstrument-firmware.ino
rg -n "getHarpejjiNoteNumColumn|effectiveSendX|lowRowSpecialBehaviorActive" ls_harpejji.ino ls_handleTouches.ino ls_midi.ino
shasum -a 256 build/harpejji-v1-final/linnstrument-firmware.ino.bin
stat -f '%z bytes' build/harpejji-v1-final/linnstrument-firmware.ino.bin
```

Expected: both version strings are `234-x7`; the binary has one reported SHA-256 and size.

- [ ] **Step 4: Audit repository cleanliness and locality**

```bash
git diff --check
git status --short
find . -name '*.orig' -o -name '*.rej' -o -name '.DS_Store'
rg -n '/''Users/|Desktop|Linn-Patch' --glob '!build/**' --glob '!.git/**' .
```

Expected: no patch debris, no newly introduced personal paths, no files created outside the project, and only intentional source/test/build changes.

- [ ] **Step 5: Remove worktree-only build links after the final build**

Remove only the two symlinks created in Task 1; the target offline environment remains untouched in the project root:

```bash
test -L .tools && rm .tools
test -L .arduino-data && rm .arduino-data
```

Expected: `.tools` and `.arduino-data` no longer appear in `git status`; all build products remain under `build/`.

- [ ] **Step 6: Create the build-ready checkpoint commit**

```bash
git add linnstrument-firmware.ino ls_harpejji.h ls_harpejji.ino ls_displayModes.ino ls_handleTouches.ino ls_midi.ino ls_settings.ino ls_switches.ino tests/harpejji_pitch_test.cpp tests/harpejji_source_wiring_test.py docs/superpowers/specs/2026-09-24-harpejji-v1-finalization-design.md docs/superpowers/plans/2026-09-24-harpejji-v1-finalization.md
git commit -m "test: prepare Harpejji x7 for hardware validation"
```

Expected: local branch contains a reproducible test candidate. Do not push.

---

### Task 8: Real LinnStrument acceptance test

**Files:**
- Test artifact: `build/harpejji-v1-final/linnstrument-firmware.ino.bin`
- Record results in the final implementation report; do not label failures as source-verified successes

**Interfaces:**
- Consumes: source/build-validated x7 binary
- Produces: `READY FOR MANUAL TEST`, followed by `VERIFIED BY USER` only after the user reports successful physical testing

- [ ] **Step 1: Stop at the hardware gate and provide safe flashing instructions**

Report the binary path, byte size, SHA-256, build result, and recovery reminder. Do not flash automatically.

- [ ] **Step 2: Verify Global Settings UI**

On hardware confirm:

- 3x4 is at `(19, 0)` and Harpejji is immediately to its right;
- both off-state indicators flash on the same Tap Tempo beat;
- Harpejji becomes steady white when enabled;
- enabling one custom layout disables the other.

- [ ] **Step 3: Verify orientation and pitch geometry**

Place controls near the player and USB away. Confirm movement toward USB rises by one semitone per cell, adjacent virtual strings differ by two semitones, octave/transpose work, and all eight rows play notes.

- [ ] **Step 4: Verify exact-note Played Same LEDs**

Press repeated MIDI notes across rows. Confirm all cells with the exact same MIDI note illuminate, other octaves remain unchanged, releases clear correctly, Split boundaries are respected, and left-handed layout does not misplace LEDs.

- [ ] **Step 5: Verify One Channel default X-off behavior**

Enter Harpejji with One Channel active. Confirm Pitch/X displays off, movement across X cells retriggers discrete notes, each new pad has fresh velocity, and rapid movement produces no stuck notes.

- [ ] **Step 6: Verify manual X-on behavior**

Enable Pitch/X while remaining in Harpejji + One Channel. Confirm the same gesture becomes stock continuous slide/transfer, Pitch Correct/Hold and Bend Range respond normally, the focused finger controls the bend, and all notes sharing the channel move together.

- [ ] **Step 7: Verify Y behavior**

With X off and then on, confirm Y center is neutral, its endpoints are minus/plus one semitone, return-to-center is stable, and in One Channel every sounding note follows the shared Y bend.

- [ ] **Step 8: Verify MPE and Channel Per Row regressions**

Confirm entering these modes restores the saved stock Pitch/X preference, MPE touches bend independently, Channel Per Row follows its original channel semantics, and returning to One Channel starts effective X off again.

- [ ] **Step 9: Verify temporary Low Row suppression and restoration**

Before entering Harpejji, select a visible Low Row special mode. Confirm Harpejji temporarily uses row 0 as a normal note string with normal note lighting. Exit Harpejji and confirm the original Low Row mode and LEDs return unchanged.

- [ ] **Step 10: Run broad regression playing checks**

Check Normal and 3x4 layouts, Split on/off, Dynamic and Classic Strum, velocity, pressure, arpeggiator, sequencer, Tap Tempo, sleep/wake, MIDI clock, preset loading, and power-cycle persistence. Report any mismatch before release preparation.

---

## Completion report requirements

The implementation report must list modified files, explain the inverse LED mapping and its fixed eight-row cost, explain effective versus saved X state, confirm Low Row settings are preserved, give the exact build command/result/binary/checksum, separate automated results from hardware-only checks, and list remaining risks. It must not claim real-device success until the user has completed Task 8.

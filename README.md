# linnstrument-firmware-GZver

## Overview

This project is an unofficial community modification of Roger Linn Design's LinnStrument firmware 2.3.4. Release version: **2.3.4-x7**. It adds an optional 3x4 Scalar Layout, Dynamic Strum, and Harpejji Mode while preserving the original LinnStrument controls and MIDI architecture wherever possible.

This firmware is intended for LinnStrument users interested in alternative pitch grids, voicing-based performance, expressive playing, and MPE. It is not an official Roger Linn Design firmware release.

## Base Firmware

- Base: LinnStrument OS 2.3.4
- Current custom version: **2.3.4-x7**
- Target board: Arduino Due (`arduino:sam:arduino_due_x`)
- Tested hardware: LinnStrument 200
- LinnStrument 128 compatibility has not been fully verified.

The upstream project is maintained by [Roger Linn Design](https://github.com/rogerlinndesign/linnstrument-firmware).

## Main Features

- Optional global 3x4 Scalar Layout for LinnStrument 200.
- Scale-aware Scalar Swipe with per-touch MIDI/MPE state.
- Per-split Dynamic Strum with voicing-aware eight-row mapping.
- Harpejji Mode with a vertical playing orientation and Harpejji-style pitch geometry.
- Dynamic Sustain, gesture Snapshot, Retrigger, and Legato integration.
- Original LinnStrument performance, settings, sequencer, arpeggiator, and MIDI features retained when the custom modes are inactive.

See [the English user guide](USER_GUIDE.md) or [中文用户指南](USER_GUIDE_zh-CN.md), plus the [installation guide](INSTALLATION.md) / [中文安装指南](INSTALLATION_zh-CN.md).

## Documentation

- Project overview: [English](README.md) / [简体中文](README_zh-CN.md)
- User guide: [English](USER_GUIDE.md) / [简体中文](USER_GUIDE_zh-CN.md)
- Installation: [English](INSTALLATION.md) / [简体中文](INSTALLATION_zh-CN.md)
- Build instructions: [English](BUILDING.md) / [简体中文](BUILDING_zh-CN.md)
- Hardware checks: [English](HARDWARE_TESTING.md) / [简体中文](HARDWARE_TESTING_zh-CN.md)
- Release notes: [English](RELEASE_NOTES.md) / [简体中文](RELEASE_NOTES_zh-CN.md)
- Changelog: [English](CHANGELOG.md) / [简体中文](CHANGELOG_zh-CN.md)
- Scalar hardware checklist: [English](SCALAR_LAYOUT_TESTING.md) / [简体中文](SCALAR_LAYOUT_TESTING_zh-CN.md)
- Release baseline: [English](BASELINE.md) / [简体中文](BASELINE_zh-CN.md)

## 3x4 Scalar Layout

The layout uses a fixed pitch-grid relationship:

- Horizontal movement: **+3 / -3 semitones**.
- Vertical movement: **+4 / -4 semitones**.

The concept was inspired by [Mike Gao's Polyplayground app](https://apps.apple.com/cn/app/polyplayground/id511514938) and its approach to alternative pitch-grid relationships. This is an inspiration only; Mike Gao is not involved in this firmware project.

### Enable or disable

1. Enter **Global Settings** using the normal LinnStrument control button.
2. Touch the playable cell in **column 19, bottom row** on a LinnStrument 200.
3. The layout is OFF when this cell follows the Tap Tempo pulse in blue.
4. The layout is ON when the cell is white and steady.
5. Leave Global Settings normally. The setting is stored by the device.

If User Firmware Mode was entered accidentally, touching the custom 3x4 cell is also a recovery path: it exits User Firmware Mode and applies the requested Scalar Layout state.

When enabled, the custom layout uses a virtual register one octave below the normal layout. Disabling it restores the normal register without changing the saved ordinary Octave setting.

### Notes, lights, and swipe behavior

The Global Root, Scale/Mode, transpose, and split settings continue to provide the musical context. Scale-degree LEDs use the current scale; out-of-scale cells are unlit but remain playable. Root and accent behavior follows the existing LinnStrument note-light settings.

A touch sends the fixed pitch of its physical cell. Sliding across a pad boundary retriggers the nearest next or previous note in the active scale, one step per crossed pad. Active MIDI notes are shown in red only on cells with the exact same MIDI note number, including octave; a slid-to physical position is not independently painted red. MIDI and MPE channel behavior follows the existing touch engine.

## Dynamic Strum

Dynamic Strum is enabled per split from **Per-Split Settings -> Special -> Strum**. Repeated short presses cycle through:

```text
OFF -> Classic Strum -> Dynamic Strum -> OFF
```

The selected split becomes the Dynamic Strum output surface. Its Strum setting cell is red and its playable area is white. The opposite split becomes the Voicing Input area; it keeps the normal LinnStrument note lighting but does not directly send its pressed notes.

The roles are symmetric:

```text
Right Dynamic: Left = Voicing Input, Right = Strum Output
Left Dynamic:  Right = Voicing Input, Left = Strum Output
```

Both splits cannot run Dynamic Strum simultaneously. Enabling Dynamic on one side safely disables it on the other side. Dynamic Strum depends on Split mode; disabling Split safely ends Dynamic notes.

### Voicing and eight-row mapping

Dynamic Strum captures the currently held MIDI notes from the Voicing side, preserving their actual pitch order, inversions, bass notes, and open voicings. It maps them to eight rows from bottom to top, generating a strictly ascending sequence. For example:

```text
C3 E3 G3 -> C3 E3 G3 C4 E4 G4 C5 E5
E3 G3 C4 -> E3 G3 C4 E4 G4 C5 E5 G5
C2 G2 E4 -> C2 G2 E3 C4 G4 E5 C6 G6
```

No held voicing means silence. A Strum gesture uses a Snapshot of the voicing when that gesture begins; changing the voicing mid-gesture does not retune notes already sounding. The next gesture uses the new voicing.

Dynamic Sustain keeps notes alive while either the Voicing or Strum side still has an active touch. Retriggering an already sounding pitch sends the required Note Off/Note On sequence instead of stacking untracked notes. The original per-split Legato setting can retain the previous Dynamic notes until the first valid note of a new gesture.

## Installation / Firmware Update

Follow [INSTALLATION.md](INSTALLATION.md) or [安装指南（中文）](INSTALLATION_zh-CN.md). The release binary is named **linnstrument-firmware-GZver-2.3.4-x7.bin**.

The official updater workflow is recommended because direct Arduino upload can reset LinnStrument settings, Projects, and calibration data. Keep a copy of the official firmware so the device can be restored if needed.

## Controls / Usage

All ordinary LinnStrument controls remain in their original locations. Scalar Layout is a Global Settings option; Dynamic Strum is a per-split Strum option. Classic Strum retains its original behavior and lighting when selected.

## Harpejji Mode

Harpejji Mode is a global layout option for LinnStrument 200. In Global Settings, use the custom cell in **column 20, bottom row**, immediately to the right of the 3x4 Scalar Layout cell. When enabled, the cell is steady white; when disabled, it pulses blue in time with Tap Tempo. It is mutually exclusive with Scalar Layout.

For the intended vertical playing orientation, place the function-button edge toward the player. Pitch rises left-to-right and bottom-to-top: columns advance by semitone and rows by two semitones. The Y axis provides expressive bend; the original Pitch/X controls remain available. In One Channel mode, entering Harpejji Mode temporarily disables Pitch/X so vertical movement retriggers notes; Pitch/X can be manually re-enabled in Per-Split Settings, and the temporary rule is restored when leaving the mode.

The normal LinnStrument Main/Accent note-light configuration and Low Row behavior are retained. In Harpejji Mode, Switch 1 and Switch 2 actions that target a split are applied to both splits; global single actions such as Tap Tempo remain single actions. Outside Harpejji Mode, Switch behavior remains unchanged. See [USER_GUIDE.md](USER_GUIDE.md) for details.

## Compatibility

This release is based on LinnStrument firmware 2.3.4 and has been tested on a LinnStrument 200. LinnStrument 128 compatibility has not been fully verified. MPE, external MIDI Clock, and all third-party synth combinations have not been exhaustively tested.

## Known Limitations

No major known functional issues were found in the current hardware-tested candidate. This is an unofficial modification and has not undergone the same validation process as an official Roger Linn Design release.

## Build Instructions

The reproducible project build targets Arduino Due and uses Arduino SAM Boards 1.6.11. The repository includes the build script and the DueFlashStorage library. With the project's local toolchain available, run:

```sh
scripts/compile-firmware.sh release-2.3.4-x7
```

The script writes intermediates under `build/` and exports the compiled binary. Build caches and the offline toolchain are intentionally ignored by Git and are not required in the public repository.

## Credits

Special thanks to Roger Linn and the entire LinnStrument / Roger Linn Design team for creating LinnStrument and making its firmware openly available for modification and experimentation.

The 3x4 Scalar Layout concept in this firmware was inspired by Mike Gao's Polyplayground app and its approach to alternative pitch-grid relationships.

Harpejji Mode is an independent layout adaptation for LinnStrument. It does not imply affiliation with or endorsement by any third party.

The original LinnStrument source code and its copyright notices remain attributable to Roger Linn Design. Modifications in this fork are maintained by **GZ_Beatz**.

## License

The upstream LinnStrument firmware and this modification are distributed under the [Apache License 2.0](LICENSE.txt). Copyright headers and third-party notices from the upstream project are retained.

## Disclaimer

This firmware is an unofficial modification. It is not affiliated with or officially supported by Roger Linn Design. Use it at your own risk. Before flashing, make sure you know how to restore the official LinnStrument firmware.

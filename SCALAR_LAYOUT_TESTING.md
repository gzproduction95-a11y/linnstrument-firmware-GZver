# linnstrument-firmware-GZver 2.3.4-x7 Hardware Test Checklist

This checklist is for the unofficial LinnStrument 200 custom firmware release.
The current candidate has been tested on LinnStrument 200. LinnStrument 128 has
not been fully verified.

## Version and installation

1. Confirm the device reports `234-x7`.
2. Install the `.bin` with the official LinnStrument Updater.
3. Keep a copy of the official firmware before testing.

## 3x4 Scalar Layout

1. Enter Global Settings and touch column 19, bottom row.
2. Confirm OFF follows the Tap Tempo pulse in blue and ON is steady white.
3. Confirm horizontal movement is +/-3 semitones and vertical movement is +/-4 semitones.
4. Confirm enabling the layout uses the one-octave-lower virtual register and disabling it restores the normal register.
5. In C Major, swipe across one or more pad boundaries in every direction.
6. Confirm out-of-scale pads remain playable but unlit.
7. Confirm red overlays match only the exact sounding MIDI note, including octave.
8. Confirm unrelated columns do not flash during repeated note and swipe gestures.

## Dynamic Strum

1. Enable Split mode.
2. In either split, open Per-Split Settings -> Special -> Strum.
3. Confirm short presses cycle OFF -> Classic -> Dynamic -> OFF.
4. Confirm the Dynamic setting cell is red and the Dynamic performance split is white.
5. Confirm the opposite split keeps normal note lighting and acts as silent Voicing Input.
6. Test Right Dynamic and Left Dynamic separately.
7. Test inversions, open voicings, Snapshot behavior, Sustain, Retrigger, and Legato.
8. Disable Split or Dynamic and confirm all notes stop and normal LEDs return.

Record any missed trigger, unexpected MIDI event, stuck note, LED residue, or
gesture that feels unsafe.

## Harpejji Mode

1. Enter Global Settings and touch column 20, bottom row.
2. Confirm ON is steady white and OFF pulses blue with Tap Tempo.
3. Orient the instrument with the function-button edge toward the player.
4. Confirm pitch rises left-to-right and bottom-to-top.
5. In One Channel mode, confirm vertical movement retriggers with Pitch/X temporarily off.
6. Manually enable Pitch/X and confirm its normal horizontal behavior; exit Harpejji and confirm normal mode returns.
7. Test Low Row at the row nearest the player.
8. Verify Split-setting previews retain the Harpejji pitch layout.
9. Verify Switch 1/2 split-target actions apply to both splits only while Harpejji is active; global single actions execute once.

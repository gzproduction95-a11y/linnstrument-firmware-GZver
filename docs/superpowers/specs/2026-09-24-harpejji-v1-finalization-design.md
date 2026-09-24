# Harpejji Mode V1 Finalization Design

## Scope

Finalize the current LinnStrument OS 2.3.4-x7 Harpejji implementation before its first hardware-validation round. This is an incremental change to the existing `feature/harpejji-v1` worktree. It must not replace the current x6/x7 custom features or restart from upstream LinnStrument 2.3.4.

## Layout and physical orientation

- Place LinnStrument vertically with the control-button edge nearest the player and the USB edge farthest away.
- Keep the firmware sensor axes unchanged: original X remains X, original Y remains Y, and Z remains Z.
- Moving from the control-button edge toward the USB edge follows original X and raises the discrete pad pitch by one semitone per cell.
- Adjacent original rows differ by two semitones.
- All eight original rows are playable virtual strings while Harpejji is active.

## Global Settings entry

- Keep the 3x4 Scalar Layout switch at column 19, row 0.
- Move the Harpejji switch to column 20, row 0: one physical cell to the right of the 3x4 switch.
- While Harpejji is off, its switch flashes from the same Tap Tempo event and at the same rate as the 3x4 switch.
- While Harpejji is on, its switch is steady white.
- Normal, 3x4 Scalar, and Harpejji remain mutually exclusive.

## Note-light behavior

- Background Main Note and Accent Note lighting continues to use the stock LinnStrument settings and colors, evaluated against each cell's actual Harpejji MIDI note.
- In Played Same mode, pressing a note lights every playable cell in the same split whose complete MIDI note is identical, including octave.
- C3 matches C3 only; it does not match C2 or C4.
- Find matching cells by algebraically reversing the Harpejji pitch formula once per row. Do not scan all 200 cells and do not allocate a 128-note lookup array.
- The same inverse mapping is used when clearing played-note LEDs and when incoming MIDI highlights possible cells.

## One Channel X/Y policy

- Entering Harpejji while a split uses One Channel temporarily starts that split with effective Pitch/X disabled.
- The saved per-split `sendX` value is not overwritten.
- With effective Pitch/X disabled, movement across original-X cells follows stock Pitch/X-off behavior: the old pad releases and the new pad retriggers its mapped MIDI note.
- The user can manually enable Pitch/X while still in Harpejji + One Channel.
- When manually enabled, X uses the stock continuous slide, cross-cell transfer, pitch-correction, pitch-hold, bend-range, channel, and reset behavior.
- In One Channel, the focused touch controls the single shared Pitch Bend and all sounding notes on that channel bend together.
- Harpejji Y remains a bipolar plus/minus one-semitone contribution to that same shared Pitch Bend.
- When X is enabled, the emitted bend is `stock X bend + Harpejji Y bend`.
- Leaving Harpejji restores normal use of the saved `sendX` value. No temporary Harpejji choice is persisted over the user's normal-layout preference.
- Entering One Channel while Harpejji is already active resets effective X to off. Channel Per Note/MPE and Channel Per Row continue to use the saved stock X preference.

## Low Row policy

- Harpejji performance mode temporarily ignores all special Low Row modes.
- All eight original rows behave as normal Harpejji note strings.
- Saved Low Row configuration is not changed and remains editable in settings.
- Exiting Harpejji immediately restores the saved Low Row behavior.
- No column-based replacement Low Row is introduced.

## Compatibility and persistence

- Harpejji remains a persisted device layout selection using the existing configuration-v19 migration.
- Temporary One Channel X state and temporary Low Row suppression are runtime-only and are not added to EEPROM/flash.
- Normal Layout, 3x4 Scalar Layout, Dynamic Strum, Classic Strum, Split, MPE, Channel Per Row, sequencer, arpeggiator, Tap Tempo, and MIDI-clock behavior must remain unchanged when Harpejji is off.
- Keep the firmware identifier at `234-x7` for this test iteration.

## Performance constraints

- Harpejji-off hot paths should pay at most a simple false branch.
- The same-note lookup performs at most eight small integer calculations per highlight or reset operation.
- Do not add dynamic allocation, STL containers, full-surface scans, background LED animation, or sensor-engine changes.

## Validation boundary

Source tests and a successful Arduino Due build establish build readiness only. Physical orientation, slide/retrigger feel, Y center and endpoints, shared-channel bend behavior, LED appearance, and stuck-note safety require a real LinnStrument test before the feature can be called hardware-verified.

# User Guide

Firmware: linnstrument-firmware-GZver 2.3.4-x7

This guide describes the custom modes. Normal LinnStrument controls continue to follow the upstream 2.3.4 behavior unless noted.

## 3x4 Scalar Layout

Open Global Settings and touch the custom cell at column 19, bottom row (LinnStrument 200). A steady white cell means enabled; a blue pulse synchronized with Tap Tempo means disabled. The mode is global and mutually exclusive with Harpejji Mode.

Each horizontal pad changes pitch by three semitones; each vertical pad changes it by four semitones. Existing Global Root, Scale/Mode, transpose, and split context are retained. In-scale notes use the existing scale-degree lighting; out-of-scale notes are unlit but playable. Main and Accent lighting continues to use the normal LinnStrument settings.

Sliding across pad boundaries advances through scale notes and retriggers for every crossed step. Played-note feedback lights only pads whose actual MIDI note number matches the active note, including octave. The physical slide destination is not independently painted red.

## Dynamic Strum

Enable this in Per-Split Settings → Special → Strum. Short presses cycle OFF → Classic Strum → Dynamic Strum → OFF. Classic Strum keeps its original behavior.

The split set to Dynamic is the Strum Output; the other split becomes silent Voicing Input. Dynamic's setting cell is red and its playable output area is white. The Voicing side retains normal note lights.

| Setting | Voicing Input | Strum Output |
| --- | --- | --- |
| Left Dynamic | Right | Left |
| Right Dynamic | Left | Right |

Only one split can be Dynamic at once. Enabling it on the other side transfers the role. Dynamic requires Split mode; turning Split off safely ends active Dynamic notes.

Held voicing pitches are ordered by actual MIDI pitch, retaining inversions, octave duplicates, bass notes, and open voicings. The mapping is captured when each Strum gesture begins, then assigned bottom-to-top as eight ascending row pitches. No held voicing means silence. For example:

~~~text
C3 E3 G3 -> C3 E3 G3 C4 E4 G4 C5 E5
E3 G3 C4 -> E3 G3 C4 E4 G4 C5 E5 G5
C2 G2 E4 -> C2 G2 E3 C4 G4 E5 C6 G6
~~~

The gesture snapshot prevents a mid-gesture voicing change from changing already sounding pitches. Notes can sustain while a Voicing or Strum touch remains active. Revisiting an active pitch retriggers it without stacking untracked Note Ons. The original per-split Legato option affects transitions between gestures.

## Harpejji Mode

Open Global Settings and touch column 20, bottom row on LinnStrument 200, immediately to the right of Scalar Layout. White steady means enabled; blue Tap Tempo pulse means disabled. It is mutually exclusive with Scalar Layout.

Rotate the instrument so the function-button edge is toward you. With this orientation, pitch rises left-to-right and bottom-to-top. Each column adds one semitone; each row adds two semitones. The Y axis is expressive bend. Low Row behavior remains the original system's behavior, corresponding to the row nearest the player in this orientation.

Harpejji keeps the normal Main/Accent note-light settings. In One Channel mode, entering Harpejji temporarily turns Pitch/X off so vertical movement can retrigger notes. You may manually re-enable Pitch/X under Per-Split Settings → Pitch/X; its horizontal handling then follows the normal system. Leaving Harpejji restores normal mode behavior.

While Harpejji is active, Switch 1 and Switch 2 actions that operate on a split apply to both splits. Actions designed to occur once globally, such as Tap Tempo, remain single actions. The switch target is held consistently through the press/release event. Outside Harpejji, the original switch behavior remains.

## Firmware and support

This release is intended for LinnStrument 200 and was tested on that model. LinnStrument 128 compatibility is not fully verified. For installation, see INSTALLATION.md. For known test boundaries, see HARDWARE_TESTING.md.

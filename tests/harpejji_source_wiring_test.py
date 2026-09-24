from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MIDI_SOURCE = (ROOT / "ls_midi.ino").read_text()
HARPEJJI_SOURCE = (ROOT / "ls_harpejji.ino").read_text()
TOUCH_SOURCE = (ROOT / "ls_handleTouches.ino").read_text()
SETTINGS_SOURCE = (ROOT / "ls_settings.ino").read_text()
SWITCH_SOURCE = (ROOT / "ls_switches.ino").read_text()
DISPLAY_SOURCE = (ROOT / "ls_displayModes.ino").read_text()
LOWROW_SOURCE = (ROOT / "ls_lowRow.ino").read_text()
MIDI_CONTROL_SOURCE = (ROOT / "ls_midi.ino").read_text()

assert "getHarpejjiNoteNumColumn" in HARPEJJI_SOURCE
assert "if (isHarpejjiLayoutActive())" in MIDI_SOURCE
assert "return getHarpejjiNoteNumColumn(split, notenum, row);" in MIDI_SOURCE

assert "harpejjiOneChannelXEnabled[2]" in HARPEJJI_SOURCE
assert "boolean effectiveSendX(byte split)" in HARPEJJI_SOURCE
assert "void resetHarpejjiOneChannelX(byte split)" in HARPEJJI_SOURCE
for source in (TOUCH_SOURCE, SETTINGS_SOURCE, SWITCH_SOURCE, DISPLAY_SOURCE, MIDI_CONTROL_SOURCE):
    assert "effectiveSendX(" in source or "toggleEffectiveSendX(" in source
assert "resetHarpejjiOneChannelX" in SETTINGS_SOURCE
assert "resetHarpejjiOneChannelX" in MIDI_CONTROL_SOURCE
assert "setEffectiveSendX(split, value)" in MIDI_CONTROL_SOURCE
assert "value = effectiveSendX(split)" in MIDI_CONTROL_SOURCE
assert "if ((effectiveSendX(sensorSplit)" in TOUCH_SOURCE
assert "Split[sensorSplit].pitchResetOnRelease && isXExpressiveCell()" in TOUCH_SOURCE
assert "if (effectiveSendX(split) && !isLowRowBendActive(split))" in TOUCH_SOURCE

assert "boolean lowRowSpecialBehaviorActive(byte split)" in HARPEJJI_SOURCE
assert "!isHarpejjiLayoutActive() && Split[split].lowRowMode != lowRowNormal" in HARPEJJI_SOURCE
assert "lowRowSpecialBehaviorActive(sensorSplit)" in LOWROW_SOURCE
assert "lowRowSpecialBehaviorActive(split)" in DISPLAY_SOURCE

FIRMWARE_SOURCE = (ROOT / "linnstrument-firmware.ino").read_text()
assert "#define SCALAR_LAYOUT_SETTINGS_COL 19" in FIRMWARE_SOURCE
assert "#define SCALAR_LAYOUT_SETTINGS_ROW 0" in FIRMWARE_SOURCE

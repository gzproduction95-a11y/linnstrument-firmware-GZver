from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MIDI_SOURCE = (ROOT / "ls_midi.ino").read_text()
HARPEJJI_SOURCE = (ROOT / "ls_harpejji.ino").read_text()

assert "getHarpejjiNoteNumColumn" in HARPEJJI_SOURCE
assert "if (isHarpejjiLayoutActive())" in MIDI_SOURCE
assert "return getHarpejjiNoteNumColumn(split, notenum, row);" in MIDI_SOURCE

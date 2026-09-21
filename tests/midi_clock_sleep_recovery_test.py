"""Regression test for stale external MIDI clock state after sleep."""

from pathlib import Path


SOURCE = Path(__file__).parents[1] / "ls_midi.ino"


def test_firmware_has_midi_clock_timeout_recovery():
    source = SOURCE.read_text()
    assert "checkMidiClockTimeout" in source
    assert "MIDI_CLOCK_TIMEOUT" in source


if __name__ == "__main__":
    test_firmware_has_midi_clock_timeout_recovery()

# Hardware Testing and Compatibility

## Verified scope

- Device: LinnStrument 200.
- Firmware base: Roger Linn Design LinnStrument OS 2.3.4.
- Custom release identifier: 234-x7.
- The project owner reports that the current firmware has been tested on hardware and is basically stable. This document does not claim that every feature combination or long-duration scenario has exhaustive coverage.

## Not fully verified

- LinnStrument 128 compatibility.
- Every MPE/channel mode, external MIDI Clock configuration, and third-party synth combination.
- Every interaction between custom layouts and all optional original settings.

## Suggested acceptance checks

After updating, test normal performance first, then separately test Scalar Layout, Dynamic Strum with Left Dynamic and Right Dynamic, Harpejji orientation, One Channel Pitch/X behavior, Low Row behavior, Split on/off cleanup, and Switch 1/2. Verify notes release correctly and lighting returns to normal after leaving each mode.

If there is abnormal touch response, stuck MIDI notes, unexpected LED behavior, or a failed update, stop using the custom firmware for performance and restore the official firmware with the supported updater.

## Safety

Use a stable USB connection and power source during firmware update. Never interrupt an active update. Keep a known-good official firmware recovery copy.

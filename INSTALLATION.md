# Installation and Firmware Update

Release: linnstrument-firmware-GZver 2.3.4-x7

## Before you begin

1. Confirm that the firmware is intended for your LinnStrument model. LinnStrument 200 is the tested model; 128 has not been fully verified.
2. Back up any important user settings or Projects using the normal supported workflow.
3. Download the release asset named **linnstrument-firmware-GZver-2.3.4-x7.bin** from this repository's GitHub Release. Verify the release tag is **v2.3.4-x7**.
4. Keep a copy of the official LinnStrument 2.3.4 firmware and the official updater available as a recovery path.

## Update

1. Launch the official LinnStrument Updater.
2. Follow the updater's own instructions to connect the instrument and enter its update procedure.
3. Select the downloaded x7 BIN when prompted.
4. Do not disconnect power or USB while the update is in progress.
5. Wait for the updater to report completion, then restart or reconnect as instructed.
6. Verify the firmware identifier shown by the instrument is **234-x7** and test basic touch and MIDI operation before a performance.

Do not use an unverified bootloader or third-party flashing procedure. Direct Arduino uploads are primarily for development and may affect saved settings, Projects, or calibration. If an update fails, use the official recovery process and firmware before attempting another custom build.

## After updating

Test both custom layout switches, ordinary note input, Split operation, and any MIDI/MPE setup you depend on. See USER_GUIDE.md and HARDWARE_TESTING.md.

This is an unofficial firmware modification. Install only if you understand the recovery process and accept the risks.

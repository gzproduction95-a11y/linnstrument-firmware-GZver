# Build Instructions

Target: Arduino Due, FQBN **arduino:sam:arduino_due_x**. The verified build uses Arduino SAM Boards **1.6.11** and the repository's **scripts/compile-firmware.sh** script. The repository also includes the DueFlashStorage dependency.

## Reproducible project build

Use the project's provisioned offline Arduino CLI/toolchain. From the repository root:

~~~sh
scripts/compile-firmware.sh release-2.3.4-x7
~~~

The script stages the firmware source and headers, compiles for Arduino Due, and writes generated files under **build/**. The distributable firmware is the resulting **.bin** file. Build caches/toolchains are machine-local and ignored by Git; they are not included in this source release.

## Verify a build

- Confirm the compiler reports success.
- Confirm the output binary exists and is non-empty.
- Confirm the source version identifier is 234-x7.
- Run the repository tests and **git diff --check** before distributing a modified build.

The release binary should be built from the exact tagged source. Build warnings from upstream code may be present; they should be reviewed, but do not report a successful build unless the command exits successfully.

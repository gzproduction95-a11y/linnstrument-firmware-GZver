/*
 * Copyright 2026 GZ_Beatz.
 *
 * Modifications based on LinnStrument firmware by Roger Linn Design.
 * Licensed under the Apache License, Version 2.0.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LS_HARPEJJI_H
#define LS_HARPEJJI_H

#include <stdint.h>

// MIDI bend units use the LinnStrument's 48-semitone full scale.
enum HarpejjiLayoutMode : uint8_t {
  HARPEJJI_LAYOUT_NORMAL = 0,
  HARPEJJI_LAYOUT_SCALAR_3X4 = 1,
  HARPEJJI_LAYOUT_HARPEJJI = 2
};

inline HarpejjiLayoutMode harpejjiLayoutMode(bool scalarEnabled,
                                             bool harpejjiEnabled) {
  if (harpejjiEnabled) return HARPEJJI_LAYOUT_HARPEJJI;
  if (scalarEnabled) return HARPEJJI_LAYOUT_SCALAR_3X4;
  return HARPEJJI_LAYOUT_NORMAL;
}
static const int16_t HARPEJJI_BEND_UNITS_PER_SEMITONE = 171;

inline int16_t harpejjiNoteNumber(int16_t basePitch,
                                 int16_t row,
                                 int16_t colOffset,
                                 int16_t transposeOctave,
                                 int16_t transposePitch,
                                 int16_t transposeLights) {
  // The instrument is used with the control edge nearest the player. In
  // that physical orientation, increasing the firmware row index moves left,
  // so the visible string index is reversed before applying the +2 interval.
  return basePitch + ((7 - row) * 2) + colOffset + transposeOctave +
         transposePitch - transposeLights;
}

inline int16_t harpejjiNoteColumn(int16_t basePitch,
                                 int16_t row,
                                 int16_t midiNote,
                                 int16_t transposeOctave,
                                 int16_t transposePitch,
                                 int16_t transposeLights,
                                 int16_t numCols,
                                 bool leftHanded) {
  int16_t rowBase = basePitch + ((7 - row) * 2) + transposeOctave +
                    transposePitch - transposeLights;
  int16_t noteCol = midiNote - rowBase + 1;
  return leftHanded ? numCols - noteCol : noteCol;
}

// Calibrated Y reads 63 and 64 straddle center; both map to zero to avoid
// a one-unit bend bias or jitter at rest. The two calibrated endpoints map
// symmetrically to +/- one semitone.
inline int16_t harpejjiYBendUnits(uint8_t yValue) {
  if (yValue <= 63) {
    return -((63 - yValue) * HARPEJJI_BEND_UNITS_PER_SEMITONE / 63);
  }
  return (yValue - 64) * HARPEJJI_BEND_UNITS_PER_SEMITONE / 63;
}

inline int16_t harpejjiCombinedBend(int16_t xUnits, int16_t yUnits) {
  return xUnits + yUnits;
}

#endif

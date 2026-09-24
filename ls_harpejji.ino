/*
 * Copyright 2026 GZ_Beatz.
 *
 * Modifications based on LinnStrument firmware by Roger Linn Design.
 * Licensed under the Apache License, Version 2.0.
 * SPDX-License-Identifier: Apache-2.0
 */

boolean isHarpejjiLayoutActive() {
  return Device.harpejjiLayoutEnabled && displayMode == displayNormal;
}

short getHarpejjiLayoutNoteNumber(byte split, byte col, byte row) {
  short noteCol = col;
  if (isLeftHandedSplit(split)) {
    noteCol = NUMCOLS - col;
  }

  return harpejjiNoteNumber(30,
                            row,
                            noteCol - 1,
                            Split[split].transposeOctave,
                            Split[split].transposePitch,
                            Split[split].transposeLights);
}

void setCustomLayoutMode(HarpejjiLayoutMode mode) {
  Device.scalarLayoutEnabled = mode == HARPEJJI_LAYOUT_SCALAR_3X4;
  Device.harpejjiLayoutEnabled = mode == HARPEJJI_LAYOUT_HARPEJJI;
}

void normalizeCustomLayoutMode() {
  setCustomLayoutMode(harpejjiLayoutMode(Device.scalarLayoutEnabled,
                                          Device.harpejjiLayoutEnabled));
}

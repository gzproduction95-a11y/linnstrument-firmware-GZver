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

boolean isHarpejjiPerformanceLayoutActive() {
  return harpejjiPerformanceLayoutActive(Device.harpejjiLayoutEnabled,
                                         displayMode == displayNormal,
                                         displayMode == displaySplitPoint);
}

boolean lowRowSpecialBehaviorActive(byte split) {
  return !isHarpejjiPerformanceLayoutActive() &&
         Split[split].lowRowMode != lowRowNormal;
}

boolean harpejjiOneChannelXEnabled[2] = { false, false };

void resetHarpejjiOneChannelX(byte split) {
  harpejjiOneChannelXEnabled[split] = false;
}

boolean effectiveSendX(byte split) {
  if (Device.harpejjiLayoutEnabled && Split[split].midiMode == oneChannel) {
    return harpejjiOneChannelXEnabled[split];
  }
  return Split[split].sendX;
}

void setEffectiveSendX(byte split, boolean enabled) {
  if (Device.harpejjiLayoutEnabled && Split[split].midiMode == oneChannel) {
    harpejjiOneChannelXEnabled[split] = enabled;
  }
  else {
    Split[split].sendX = enabled;
  }
}

void toggleEffectiveSendX(byte split) {
  setEffectiveSendX(split, !effectiveSendX(split));
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

short getHarpejjiNoteNumColumn(byte split, byte midiNote, byte row) {
  short col = harpejjiNoteColumn(30,
                                 row,
                                 midiNote,
                                 Split[split].transposeOctave,
                                 Split[split].transposePitch,
                                 Split[split].transposeLights,
                                 NUMCOLS,
                                 isLeftHandedSplit(split));
  byte lowCol, highCol;
  getSplitBoundaries(split, lowCol, highCol);
  if (col < lowCol || col >= highCol) {
    return -1;
  }
  return col;
}

void setCustomLayoutMode(HarpejjiLayoutMode mode) {
  if (mode == HARPEJJI_LAYOUT_HARPEJJI) {
    resetHarpejjiOneChannelX(LEFT);
    resetHarpejjiOneChannelX(RIGHT);
  }
  Device.scalarLayoutEnabled = mode == HARPEJJI_LAYOUT_SCALAR_3X4;
  Device.harpejjiLayoutEnabled = mode == HARPEJJI_LAYOUT_HARPEJJI;
}

void normalizeCustomLayoutMode() {
  setCustomLayoutMode(harpejjiLayoutMode(Device.scalarLayoutEnabled,
                                          Device.harpejjiLayoutEnabled));
}

/*
 * Copyright 2026 GZ_Beatz.
 *
 * Modifications based on LinnStrument firmware by Roger Linn Design.
 * Licensed under the Apache License, Version 2.0.
 * SPDX-License-Identifier: Apache-2.0
 */

/************************ LinnStrument 3x4 Scalar Layout ************************
This optional performance layer keeps all stock code paths intact while the
device-level switch is off. Pure pitch and scale helpers live here so the touch
and LED integrations remain small and auditable.
********************************************************************************/

short scalarPositiveModulo(short value, short modulus) {
  short result = value % modulus;
  return result < 0 ? result + modulus : result;
}

// Calculate the fixed 3x4 Balzano pitch for a physical performance cell.
// Horizontal movement is three semitones per cell and vertical movement is
// four semitones per cell. Keep the stock transpose-lights convention so the
// existing transpose controls retain their meaning.
short getScalarLayoutNoteNumber(byte split, byte col, byte row) {
  short noteCol = col;
  if (isLeftHandedSplit(split)) {
    noteCol = NUMCOLS - col;
  }

  // The Scalar Layout has its own default register: one octave below the
  // stock F#2 baseline. This is a virtual offset, so toggling the layout
  // never changes or persists the user's normal octave setting.
  const short lowest = 18;
  return lowest + (row * 4) + ((noteCol - 1) * 3) - Split[split].transposeLights;
}

unsigned short getScalarLayoutScaleMask() {
  return Global.mainNotes[Global.activeNotes] & 0x0fff;
}

boolean isScalarLayoutScalePitch(short note, byte split) {
  short untransposedNote = note - Split[split].transposePitch;
  byte pitchClass = scalarPositiveModulo(untransposedNote, 12);
  return getScalarLayoutScaleMask() & (1 << pitchClass);
}

short getNextScalarLayoutScaleNote(byte split, short note, signed char direction) {
  if (direction == 0) return note;

  signed char step = direction > 0 ? 1 : -1;
  short candidate = note;
  while (candidate >= 0 && candidate <= 127) {
    candidate += step;
    if (candidate < 0 || candidate > 127) {
      return note;
    }
    if (isScalarLayoutScalePitch(candidate, split)) {
      return candidate;
    }
  }

  return note;
}

byte getScalarLayoutRootPitchClass() {
  unsigned short scaleMask = getScalarLayoutScaleMask();
  unsigned short rootMask = Global.accentNotes[Global.activeNotes] & scaleMask;
  unsigned short preferredMask = rootMask ? rootMask : scaleMask;

  for (byte pitchClass = 0; pitchClass < 12; ++pitchClass) {
    if (preferredMask & (1 << pitchClass)) {
      return pitchClass;
    }
  }

  return 0;
}

byte getScalarLayoutDegreeColor(short displayedNote) {
  static const byte degreeColors[7] = {
    COLOR_WHITE,
    COLOR_CYAN,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_ORANGE
  };

  unsigned short scaleMask = getScalarLayoutScaleMask();
  byte pitchClass = scalarPositiveModulo(displayedNote, 12);
  if (!(scaleMask & (1 << pitchClass))) {
    return COLOR_OFF;
  }

  byte root = getScalarLayoutRootPitchClass();
  byte degree = 0;
  for (byte offset = 0; offset < 12; ++offset) {
    byte currentPitchClass = (root + offset) % 12;
    if (scaleMask & (1 << currentPitchClass)) {
      if (currentPitchClass == pitchClass) {
        return degreeColors[degree % 7];
      }
      degree++;
    }
  }

  return COLOR_OFF;
}

boolean isScalarLayoutPerformanceCell(byte split, byte row) {
  if (Split[split].ccFaders || isStrummingSplit(split)) return false;
  if (row == 0 && Split[split].lowRowMode != lowRowNormal) return false;
  return true;
}

void refreshScalarLayoutPlayedLeds() {
  if (!Device.scalarLayoutEnabled || displayMode != displayNormal || userFirmwareActive) return;

  boolean activeMidiNotes[128];
  memset(activeMidiNotes, 0, sizeof(activeMidiNotes));

  // First collect active notes from the maintained touch bitmasks. The touch
  // and note checks remain deliberately unchanged for pending-release cells.
  for (byte row = 0; row < NUMROWS; ++row) {
    int32_t touchedColumns = colsInRowsTouched[row];
    while (touchedColumns) {
      byte col = __builtin_ctz((uint32_t)touchedColumns);
      touchedColumns &= touchedColumns - 1;
      if (col == 0 || col >= NUMCOLS) continue;
      TouchInfo& touch = cell(col, row);
      if (touch.touched == touchedCell && touch.hasNote()) {
        activeMidiNotes[(byte)touch.note] = true;
      }
    }
  }

  // A direct refresh can be initiated by note-on, note-off, or a Scalar Swipe.
  // Update a hidden buffer in that case, then commit one complete frame. This
  // prevents the LED scanner from showing the old overlay after it has been
  // cleared but before the replacement red cells are drawn.
  boolean ownsLedBuffer = bufferedLeds == 0;
  if (ownsLedBuffer) startBufferedLeds();

  // Active-pitch overlay: red means this exact MIDI note is sounding. It does
  // not match notes in other octaves and does not follow the physical finger
  // position after a Scalar Swipe. Each cell is assigned its final state in a
  // single pass rather than clearing the entire layer before repainting it.
  for (byte row = 0; row < NUMROWS; ++row) {
    for (byte col = 1; col < NUMCOLS; ++col) {
      byte split = getSplitOf(col);
      boolean shouldShowPlayed = false;
      if (isScalarLayoutPerformanceCell(split, row)) {
        short note = getScalarLayoutNoteNumber(split, col, row) +
                     Split[split].transposePitch + Split[split].transposeOctave;
        shouldShowPlayed = note >= 0 && note <= 127 && activeMidiNotes[note];
      }

      if (shouldShowPlayed) {
        setLed(col, row, COLOR_RED, cellOn, LED_LAYER_PLAYED);
      }
      else {
        clearLed(col, row, LED_LAYER_PLAYED);
      }
    }
  }

  if (ownsLedBuffer) finishBufferedLeds();
}

boolean isScalarLayoutTouchTransferAllowed() {
  if (!Device.scalarLayoutEnabled || displayMode != displayNormal) return false;
  if (userFirmwareActive || controlModeActive) return false;
  if (Split[Global.currentPerSplit].sequencer) return false;
  if (Split[sensorSplit].ccFaders || isStrummingSplit(sensorSplit)) return false;
  if (isLowRow() && Split[sensorSplit].lowRowMode != lowRowNormal) return false;
  return true;
}

boolean isScalarLayoutTransferNear(byte sourceCol, byte sourceRow) {
  TouchInfo& source = cell(sourceCol, sourceRow);
  if (source.pendingReleaseCount) return true;

  short colDelta = sensorCol - sourceCol;
  short rowDelta = sensorRow - sourceRow;
  boolean horizontalNear = true;
  boolean verticalNear = true;

  if (colDelta != 0) {
    horizontalNear = abs(sensorCell->calibratedX() - source.currentCalibratedX) < TRANSFER_SLIDE_PROXIMITY;
  }

  if (rowDelta > 0) {
    verticalNear = sensorCell->calibratedY() < 40 && source.currentCalibratedY > 87;
  }
  else if (rowDelta < 0) {
    verticalNear = sensorCell->calibratedY() > 87 && source.currentCalibratedY < 40;
  }

  return horizontalNear && verticalNear;
}

boolean findScalarLayoutTransferSource(byte* sourceCol, byte* sourceRow) {
  boolean found = false;
  boolean foundPendingRelease = false;
  unsigned short bestScore = 0xffff;

  for (byte candidateRow = 0; candidateRow < NUMROWS; ++candidateRow) {
    int32_t touchedColumns = colsInRowsTouched[candidateRow] & ~(int32_t)1;
    while (touchedColumns) {
      byte candidateCol = __builtin_ctz((uint32_t)touchedColumns);
      touchedColumns &= touchedColumns - 1;
      if (candidateRow == sensorRow && candidateCol == sensorCol) continue;
      if (getSplitOf(candidateCol) != sensorSplit) continue;

      byte padDistance = max(abs((short)sensorCol - candidateCol), abs((short)sensorRow - candidateRow));
      if (padDistance > 4) continue;

      TouchInfo& candidate = cell(candidateCol, candidateRow);
      if (candidate.touched == untouchedCell || !candidate.hasNote()) continue;
      if (!isScalarLayoutTransferNear(candidateCol, candidateRow)) continue;

      boolean pendingRelease = candidate.pendingReleaseCount > 0;
      unsigned short pressureDifference = abs((short)sensorCell->currentRawZ - (short)candidate.currentRawZ);
      unsigned short score = padDistance * 1024 + pressureDifference;
      if (!found ||
          (pendingRelease && !foundPendingRelease) ||
          (pendingRelease == foundPendingRelease && score < bestScore)) {
        *sourceCol = candidateCol;
        *sourceRow = candidateRow;
        found = true;
        foundPendingRelease = pendingRelease;
        bestScore = score;
      }
    }
  }

  return found;
}

boolean hasOtherScalarLayoutNote(byte split, byte excludedCol, byte excludedRow, signed char note, signed char channel) {
  for (byte row = 0; row < NUMROWS; ++row) {
    int32_t touchedColumns = colsInRowsTouched[row];
    while (touchedColumns) {
      byte col = 31 - __builtin_clz(touchedColumns);
      if (!(col == excludedCol && row == excludedRow) &&
          getSplitOf(col) == split &&
          cell(col, row).touched == touchedCell &&
          cell(col, row).note == note &&
          cell(col, row).channel == channel) {
        return true;
      }
      touchedColumns &= ~(1 << col);
    }
  }
  return false;
}

signed char getScalarLayoutTransferDirection(byte sourceCol, byte sourceRow) {
  short colDelta = sensorCol - sourceCol;
  short rowDelta = sensorRow - sourceRow;

  if (colDelta == 0) return rowDelta > 0 ? 1 : -1;
  if (rowDelta == 0) return colDelta > 0 ? 1 : -1;

  // Same-sign diagonals have an unambiguous direction. For conflicting
  // diagonals, use the axis with the larger normalized physical movement.
  signed char horizontalDirection = colDelta > 0 ? 1 : -1;
  signed char verticalDirection = rowDelta > 0 ? 1 : -1;
  if (horizontalDirection == verticalDirection) return horizontalDirection;

  short movementX = sensorCell->currentCalibratedX - cell(sourceCol, sourceRow).currentCalibratedX;
  short movementY = (rowDelta * 128) + sensorCell->currentCalibratedY - cell(sourceCol, sourceRow).currentCalibratedY;
  return abs(movementX) >= abs(movementY) ? horizontalDirection : verticalDirection;
}

void transferScalarLayoutTouch(byte sourceCol, byte sourceRow) {
  TouchInfo& source = cell(sourceCol, sourceRow);
  signed char oldNote = source.note;
  signed char channel = source.channel;
  signed char direction = getScalarLayoutTransferDirection(sourceCol, sourceRow);
  byte steps = max(abs((short)sensorCol - sourceCol), abs((short)sensorRow - sourceRow));

  // Preserve touch ownership and expressive state, then establish fresh local
  // X/Y origins for the newly entered pad.
  sensorCell->lastTouch = source.lastTouch;
  sensorCell->didMove = source.didMove;
  sensorCell->initialX = sensorCell->currentCalibratedX;
  sensorCell->initialColumn = sensorCol;
  if (Split[sensorSplit].pitchCorrectQuantize) {
    sensorCell->quantizationOffsetX = sensorCell->currentCalibratedX -
      FXD_TO_INT(Device.calRows[sensorCol][0].fxdReferenceX);
  }
  else {
    sensorCell->quantizationOffsetX = 0;
  }
  sensorCell->lastMovedX = 0;
  sensorCell->lastValueX = INVALID_DATA;
  sensorCell->fxdRateX = source.fxdRateX;
  sensorCell->fxdRateCountX = source.fxdRateCountX;
  sensorCell->slideTransfer = true;
  sensorCell->rogueSweepX = false;
  sensorCell->initialY = sensorCell->currentCalibratedY;
  sensorCell->note = oldNote;
  sensorCell->channel = channel;
  sensorCell->octaveOffset = source.octaveOffset;
  sensorCell->fxdPrevPressure = source.fxdPrevPressure;
  sensorCell->fxdPrevTimbre = source.fxdPrevTimbre;
  sensorCell->velocity = calcPreferredVelocity(sensorCell->velocityZ);
  sensorCell->vcount = source.vcount;

  noteTouchMapping[sensorSplit].changeCell(oldNote, channel, sensorCol, sensorRow);

  source.lastTouch = 0;
  source.didMove = false;
  source.initialX = INVALID_DATA;
  source.initialColumn = -1;
  source.quantizationOffsetX = 0;
  source.lastMovedX = 0;
  source.lastValueX = INVALID_DATA;
  source.fxdRateX = 0;
  source.fxdRateCountX = 0;
  source.slideTransfer = true;
  source.rogueSweepX = false;
  source.initialY = -1;
  source.pendingReleaseCount = 0;
  source.note = -1;
  source.channel = -1;
  source.octaveOffset = 0;
  source.fxdPrevPressure = 0;
  source.fxdPrevTimbre = FXD_CONST_255;
  source.velocity = 0;
  cellTouched(sourceCol, sourceRow, transferCell);

  for (byte step = 0; step < steps; ++step) {
    oldNote = sensorCell->note;
    short newNote = getNextScalarLayoutScaleNote(sensorSplit, oldNote, direction);
    if (newNote == oldNote) break;

    if (isArpeggiatorEnabled(sensorSplit)) {
      handleArpeggiatorNoteOff(sensorSplit, oldNote, channel);
    }
    else if (!hasOtherScalarLayoutNote(sensorSplit, sensorCol, sensorRow, oldNote, channel)) {
      midiSendNoteOffWithVelocity(sensorSplit, oldNote, sensorCell->velocity, channel);
    }
    noteTouchMapping[sensorSplit].noteOff(oldNote, channel);

    sensorCell->note = newNote;
    noteTouchMapping[sensorSplit].noteOn(newNote, channel, sensorCol, sensorRow);

    FocusCell& focused = focus(sensorSplit, channel);
    focused.col = sensorCol;
    focused.row = sensorRow;

    if (!isArpeggiatorEnabled(sensorSplit)) {
      if (Split[sensorSplit].sendX && !isLowRowBendActive(sensorSplit)) {
        resetLastMidiPitchBend(channel);
        preSendPitchBend(sensorSplit, 0, channel);
      }
      if (Split[sensorSplit].sendZ) {
        preResetLastLoudness(sensorSplit, sensorCell->note, channel);
      }
      if (Split[sensorSplit].sendY) {
        preResetLastTimbre(sensorSplit, sensorCell->note, channel);
      }
      sendNewNote();
    }
  }

  refreshScalarLayoutPlayedLeds();
}

boolean handleScalarLayoutNewTouchTransfer() {
  if (!isScalarLayoutTouchTransferAllowed()) return false;

  byte sourceCol = 0;
  byte sourceRow = 0;
  if (!findScalarLayoutTransferSource(&sourceCol, &sourceRow)) return false;

  TouchInfo& source = cell(sourceCol, sourceRow);
  if (source.pendingReleaseCount || sensorCell->currentRawZ > source.currentRawZ) {
    transferScalarLayoutTouch(sourceCol, sourceRow);
    handleXYZupdate();
  }
  else {
    cellTouched(transferCell);
  }

  return true;
}

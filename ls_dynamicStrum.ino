/*
 * Copyright 2026 GZ_Beatz.
 *
 * Modifications based on LinnStrument firmware by Roger Linn Design.
 * Licensed under the Apache License, Version 2.0.
 * SPDX-License-Identifier: Apache-2.0
 */

// x6 Dynamic Strum: role resolution, held voicing, and dynamic row pitch generation.
#ifndef STRUM_OFF
#define STRUM_OFF 0
#define STRUM_CLASSIC 1
#define STRUM_DYNAMIC 2
#endif

#define DYNAMIC_NOTE_INVALID (-1)
struct DynamicSoundingNote { boolean sounding; signed char channel; byte split; };
short dynamicLiveMap[MAXROWS];
short dynamicStrumSnapshot[MAXROWS];
boolean dynamicLiveMapValid = false;
boolean dynamicStrumSnapshotValid = false;
byte dynamicVoicingTouchCount = 0;
byte dynamicStrumTouchCount = 0;
boolean dynamicNotesRetainedByLegato = false;
DynamicSoundingNote dynamicSoundingNotes[128];

void clearDynamicPitchMaps() {
  for (byte row=0; row<MAXROWS; ++row) { dynamicLiveMap[row]=DYNAMIC_NOTE_INVALID; dynamicStrumSnapshot[row]=DYNAMIC_NOTE_INVALID; }
  dynamicLiveMapValid=false; dynamicStrumSnapshotValid=false;
}
void clearDynamicSoundingState() {
  for (byte n=0; n<128; ++n) { dynamicSoundingNotes[n].sounding=false; dynamicSoundingNotes[n].channel=-1; dynamicSoundingNotes[n].split=0; }
}
boolean dynamicRuntimeHasSoundingNotes() {
  for (byte n=0; n<128; ++n) if (dynamicSoundingNotes[n].sounding) return true;
  return false;
}
void releaseAllSoundingDynamicNotes() {
  for (byte n=0; n<128; ++n) if (dynamicSoundingNotes[n].sounding) {
    midiSendNoteOff(dynamicSoundingNotes[n].split,n,dynamicSoundingNotes[n].channel);
    releaseChannel(dynamicSoundingNotes[n].split,dynamicSoundingNotes[n].channel);
  }
  clearDynamicSoundingState();
  dynamicNotesRetainedByLegato = false;
}
byte countDynamicTouches(byte split) {
  byte count=0;
  for (byte col=1; col<NUMCOLS; ++col) if (getSplitOf(col)==split)
    for (byte row=0; row<NUMROWS; ++row) if (cell(col,row).touched==touchedCell) ++count;
  return count;
}
void rebuildDynamicLiveMap();
void refreshDynamicTouchState() {
  byte strum=getDynamicStrumSplit(); byte voicing=getDynamicVoicingSplit();
  byte oldStrum=dynamicStrumTouchCount;
  dynamicVoicingTouchCount=(voicing==255)?0:countDynamicTouches(voicing);
  dynamicStrumTouchCount=(strum==255)?0:countDynamicTouches(strum);
  if (oldStrum==0 && dynamicStrumTouchCount>0) {
    for (byte row=0; row<MAXROWS; ++row) dynamicStrumSnapshot[row]=dynamicLiveMap[row];
    dynamicStrumSnapshotValid=dynamicLiveMapValid;
  }
  else if (oldStrum>0 && dynamicStrumTouchCount==0) {
    dynamicStrumSnapshotValid=false;
    for (byte row=0; row<MAXROWS; ++row) dynamicStrumSnapshot[row]=DYNAMIC_NOTE_INVALID;
  }
  if (dynamicVoicingTouchCount==0 && dynamicStrumTouchCount==0) {
    byte dynamicSplit = getConfiguredDynamicStrumSplit();
    if (dynamicRuntimeHasSoundingNotes() && dynamicSplit != 255 && isSwitchLegatoPressed(dynamicSplit)) {
      dynamicNotesRetainedByLegato = true;
    }
    else if (!dynamicNotesRetainedByLegato) {
      releaseAllSoundingDynamicNotes();
    }
    dynamicStrumSnapshotValid=false;
  }
}
void resetDynamicRuntime() { releaseAllSoundingDynamicNotes(); clearDynamicPitchMaps(); dynamicVoicingTouchCount=0; dynamicStrumTouchCount=0; dynamicNotesRetainedByLegato=false; }
void clearDynamicTouchCells(byte dynamicSplit);

// Clear all runtime state owned by the currently configured Dynamic Strum
// before another feature overwrites its mode or settings. The caller remains
// responsible for changing Split[].strum and refreshing the display.
void resetConfiguredDynamicStrumRuntime(byte dynamicSplit) {
  if (dynamicSplit == 255) return;
  resetDynamicRuntime();
  clearDynamicTouchCells(dynamicSplit);
}

void clearDynamicTouchCells(byte dynamicSplit) {
  if (dynamicSplit == 255) return;
  byte voicingSplit = otherSplit(dynamicSplit);
  for (byte col=1; col<NUMCOLS; ++col) {
    byte cellSplit = getSplitOf(col);
    if (cellSplit != dynamicSplit && cellSplit != voicingSplit) continue;
    for (byte row=0; row<NUMROWS; ++row) {
      cell(col,row).touched = untouchedCell;
      cell(col,row).note = -1;
      cell(col,row).channel = -1;
    }
  }
}

byte getConfiguredDynamicStrumSplit() {
  if (Split[LEFT].strum == STRUM_DYNAMIC) return LEFT;
  if (Split[RIGHT].strum == STRUM_DYNAMIC) return RIGHT;
  return 255;
}

void handleDynamicLegatoStateChanged(byte split, boolean enabled) {
  if (enabled || split != getConfiguredDynamicStrumSplit()) return;
  if (dynamicVoicingTouchCount == 0 && dynamicStrumTouchCount == 0 && dynamicNotesRetainedByLegato) {
    releaseAllSoundingDynamicNotes();
  }
  else {
    dynamicNotesRetainedByLegato = false;
  }
}


byte getDynamicStrumSplit() {
  if (!Global.splitActive) return 255;
  if (Split[LEFT].strum == STRUM_DYNAMIC) return LEFT;
  if (Split[RIGHT].strum == STRUM_DYNAMIC) return RIGHT;
  return 255;
}

byte getDynamicVoicingSplit() {
  byte s = getDynamicStrumSplit();
  return s == 255 ? 255 : otherSplit(s);
}

boolean isDynamicStrumSplit(byte split) {
  return Global.splitActive && Split[split].strum == STRUM_DYNAMIC;
}

boolean isDynamicVoicingSplit(byte split) {
  byte s = getDynamicVoicingSplit();
  return s != 255 && s == split;
}

void rebuildDynamicLiveMap() {
  byte voicing=getDynamicVoicingSplit();
  if (voicing==255) { clearDynamicPitchMaps(); return; }
  buildDynamicStrumNotes(voicing,dynamicLiveMap);
  dynamicLiveMapValid=(dynamicLiveMap[0]!=DYNAMIC_NOTE_INVALID);
}

// Build strictly ascending notes while preserving the sorted voicing pitch-class order.
void buildDynamicStrumNotes(byte voicingSplit, short* output) {
  short held[MAXCOLS * MAXROWS];
  byte count = 0;
  for (byte col = 1; col < NUMCOLS; ++col) {
    if (getSplitOf(col) != voicingSplit) continue;
    for (byte row = 0; row < NUMROWS; ++row) {
      TouchInfo& t = cell(col, row);
      if (t.touched == touchedCell && t.note >= 0 && t.note <= 127 && count < MAXCOLS * MAXROWS)
        held[count++] = t.note;
    }
  }
  for (byte i = 1; i < count; ++i) {
    short v = held[i]; byte j = i;
    while (j > 0 && held[j - 1] > v) { held[j] = held[j - 1]; --j; }
    held[j] = v;
  }
  for (byte i = 0; i < MAXROWS; ++i) output[i] = -1;
  if (!count) return;
  output[0] = held[0];
  for (byte i = 1; i < MAXROWS; ++i) {
    byte pc = held[i % count] % 12;
    short prev = output[i - 1];
    short delta = (pc - (prev % 12) + 12) % 12;
    if (!delta) delta = 12;
    short candidate = prev + delta;
    if (candidate > 127) break;
    output[i] = candidate;
  }
}

void dynamicStrumCaptureVoicing(byte split) {
  sensorCell->note = cellTransposedNote(split);
  sensorCell->channel = -1;
  rebuildDynamicLiveMap();
  refreshDynamicTouchState();
}

void dynamicStrumTrigger(byte split, boolean retrigger) {
  refreshDynamicTouchState();
  if (!dynamicStrumSnapshotValid || sensorRow >= MAXROWS) return;
  short pitch = dynamicStrumSnapshot[sensorRow];
  if (pitch < 0 || pitch > 127) return;
  byte midiSplit=getDynamicVoicingSplit();
  if (midiSplit==255) return;
  if (dynamicNotesRetainedByLegato) {
    releaseAllSoundingDynamicNotes();
  }
  if (dynamicSoundingNotes[pitch].sounding) {
    midiSendNoteOff(dynamicSoundingNotes[pitch].split, pitch, dynamicSoundingNotes[pitch].channel);
    releaseChannel(dynamicSoundingNotes[pitch].split, dynamicSoundingNotes[pitch].channel);
    dynamicSoundingNotes[pitch].sounding=false;
  }
  byte channel=takeChannel(midiSplit,sensorRow);
  byte velocity=sensorCell->velocity ? sensorCell->velocity : 127;
  midiSendNoteOn(midiSplit,pitch,velocity,channel);
  dynamicSoundingNotes[pitch].sounding=true;
  dynamicSoundingNotes[pitch].channel=channel;
  dynamicSoundingNotes[pitch].split=midiSplit;
  sensorCell->note=pitch;
  sensorCell->channel=channel;
  sensorCell->velocity=velocity;
}

void dynamicStrumRelease(byte split) {
  sensorCell->note = -1;
  sensorCell->channel = -1;
  rebuildDynamicLiveMap();
  refreshDynamicTouchState();
}

void setDynamicStrumMode(byte split, byte mode) {
  if (mode > STRUM_DYNAMIC) mode = STRUM_OFF;
  // Only Dynamic mode changes may clear Dynamic-owned touch state.
  byte oldDynamicSplit = getConfiguredDynamicStrumSplit();
  if (oldDynamicSplit != 255 &&
      (oldDynamicSplit != split || mode != STRUM_DYNAMIC)) {
    resetConfiguredDynamicStrumRuntime(oldDynamicSplit);
  }
  Split[split].strum = mode;
  if (mode == STRUM_DYNAMIC) {
    byte other = otherSplit(split);
    if (Split[other].strum == STRUM_DYNAMIC) Split[other].strum = STRUM_OFF;
    Split[split].arpeggiator = false;
    Split[split].ccFaders = false;
    setSplitSequencerEnabled(split, false);
  }
  updateDisplay();
}

byte dynamicShortPressMode(byte split) {
  byte mode = Split[split].strum;
  if (mode == STRUM_OFF) return STRUM_CLASSIC;
  if (mode == STRUM_CLASSIC) return STRUM_DYNAMIC;
  return STRUM_OFF;
}

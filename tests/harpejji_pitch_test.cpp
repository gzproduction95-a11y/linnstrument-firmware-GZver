#include "../ls_harpejji.h"

#include <assert.h>

int main() {
  assert(harpejjiPerformanceLayoutActive(true, true, false));
  assert(harpejjiPerformanceLayoutActive(true, false, true));
  assert(!harpejjiPerformanceLayoutActive(true, false, false));
  assert(!harpejjiPerformanceLayoutActive(false, true, true));

  assert(harpejjiSwitchAffectsBothSplits(true, true, false));
  assert(!harpejjiSwitchAffectsBothSplits(false, true, false));
  assert(!harpejjiSwitchAffectsBothSplits(true, false, false));
  assert(!harpejjiSwitchAffectsBothSplits(true, true, true));

  assert(harpejjiLayoutMode(false, false) == 0);
  assert(harpejjiLayoutMode(true, false) == 1);
  assert(harpejjiLayoutMode(true, true) == 2);
  assert(harpejjiNoteNumber(30, 0, 0, 0, 0, 0) == 44);
  assert(harpejjiNoteNumber(30, 0, 7, 0, 0, 0) == 51);
  assert(harpejjiNoteNumber(30, 1, 0, 0, 0, 0) == 42);
  assert(harpejjiNoteNumber(30, 0, 1, 0, 0, 0) == 45);
  assert(harpejjiNoteNumber(30, 7, 24, 0, 0, 0) == 54);
  assert(harpejjiNoteNumber(30, 7, 0, 0, 0, 0) == 30);
  assert(harpejjiNoteNumber(30, 0, 0, -12, 2, 0) == 34);
  assert(harpejjiYBendUnits(0) == -171);
  assert(harpejjiYBendUnits(63) == 0);
  assert(harpejjiYBendUnits(64) == 0);
  assert(harpejjiYBendUnits(127) == 171);
  assert(harpejjiCombinedBend(1197, 171) == 1368);
  assert(harpejjiCombinedBend(1197, -171) == 1026);

  assert(harpejjiNoteColumn(30, 0, 44, 0, 0, 0, 26, false) == 1);
  assert(harpejjiNoteColumn(30, 7, 30, 0, 0, 0, 26, false) == 1);
  assert(harpejjiNoteColumn(30, 3, 50, 12, 0, 0, 26, false) == 1);
  assert(harpejjiNoteColumn(30, 0, 44, 0, 0, 0, 26, true) == 25);
  assert(harpejjiNoteColumn(30, 0, 56, 0, 0, 0, 26, false) == 13);
  assert(harpejjiNoteColumn(30, 2, 45, 0, 2, -1, 26, false) == 3);

  for (int16_t row = 0; row < 8; ++row) {
    for (int16_t col = 1; col < 26; ++col) {
      int16_t note = harpejjiNoteNumber(30, row, col - 1, 0, 0, 0);
      assert(harpejjiNoteColumn(30, row, note, 0, 0, 0, 26, false) == col);
      int16_t leftNote = harpejjiNoteNumber(30, row, 26 - col - 1, 0, 0, 0);
      assert(harpejjiNoteColumn(30, row, leftNote, 0, 0, 0, 26, true) == col);
    }
  }
}

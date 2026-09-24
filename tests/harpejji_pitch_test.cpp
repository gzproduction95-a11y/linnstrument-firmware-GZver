#include "../ls_harpejji.h"

#include <assert.h>

int main() {
  assert(harpejjiLayoutMode(false, false) == 0);
  assert(harpejjiLayoutMode(true, false) == 1);
  assert(harpejjiLayoutMode(true, true) == 2);
  assert(harpejjiNoteNumber(30, 0, 0, 0, 0, 0) == 30);
  assert(harpejjiNoteNumber(30, 0, 7, 0, 0, 0) == 37);
  assert(harpejjiNoteNumber(30, 1, 0, 0, 0, 0) == 32);
  assert(harpejjiNoteNumber(30, 0, 1, 0, 0, 0) == 31);
  assert(harpejjiNoteNumber(30, 7, 24, 0, 0, 0) == 68);
  assert(harpejjiNoteNumber(30, 7, 0, 0, 0, 0) == 44);
  assert(harpejjiNoteNumber(30, 0, 0, -12, 2, 0) == 20);
  assert(harpejjiYBendUnits(0) == -171);
  assert(harpejjiYBendUnits(63) == 0);
  assert(harpejjiYBendUnits(64) == 0);
  assert(harpejjiYBendUnits(127) == 171);
  assert(harpejjiCombinedBend(1197, 171) == 1368);
  assert(harpejjiCombinedBend(1197, -171) == 1026);
}

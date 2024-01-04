#define EXTISM_IMPLEMENTATION
#include "util.h"

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  char c[2];
  __extism_load(extism_input_offset(), c, 0);
  assert(extism_load_input(0, c, 0));
  assert(!extism_load_input(0, c, 1));
  extism_load_sz_unsafe(extism_input_offset(), c, 1);
  assert(!extism_load_input_sz(0, c, 0));
  assert(extism_load_input_sz(0, c, 1));
  assert(!extism_load_input_sz(0, c, 2));
  return 0;
}

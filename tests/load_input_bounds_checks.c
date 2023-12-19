#define EXTISM_IMPLEMENTATION
#include "util.h"

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  char c[2];
  extism_load_input_unsafe(0, c, 0);
  assert(extism_load_input(0, c, 0));
  assert(!extism_load_input(0, c, 1));
  extism_load_input_sz_unsafe(0, c, 1);
  assert(!extism_load_input_sz(0, c, 0));
  assert(extism_load_input_sz(0, c, 1));
  assert(!extism_load_input_sz(0, c, 2));
  return 0;
}

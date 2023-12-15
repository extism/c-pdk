#include "util.h"

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  char c[2];
  extism_load_input_unsafe(c, 0);
  assert(extism_load_input(c, 0));
  assert(!extism_load_input(c, 1));
  extism_load_input_sz_unsafe(c, 1);
  assert(!extism_load_input_sz(c, 0));
  assert(extism_load_input_sz(c, 1));
  assert(!extism_load_input_sz(c, 2));
  return 0;
}

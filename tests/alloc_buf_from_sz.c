#define EXTISM_IMPLEMENTATION
#include "util.h"

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  const char *msg = "Hello, world!";
  ExtismHandle buf = extism_alloc_buf_from_sz(msg);
  assert(buf > 0);
  assert(extism_length(buf) == extism_strlen(msg));
  return 0;
}

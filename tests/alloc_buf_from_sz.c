#include "util.h"

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  const char *msg = "Hello, world!";
  ExtismPointer ptr = extism_alloc_buf_from_sz(msg);
  assert(ptr > 0);
  assert(extism_length(ptr) == extism_strlen(msg));
  return 0;
}

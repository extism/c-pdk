#define EXTISM_IMPLEMENTATION
#include "util.h"

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  const char *msg = "Hello, world!";
  ExtismPointer ptr = extism_alloc(extism_strlen(msg));
  assert(ptr > 0);
  extism_store(ptr, (const uint8_t *)msg, extism_strlen(msg));
  assert(extism_length(ptr) == extism_strlen(msg));
  extism_output_set(ptr, extism_strlen(msg));
  return 0;
}

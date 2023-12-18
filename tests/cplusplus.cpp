#include "util.h"
#include <string.h>

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  const char *msg = "Hello, world!";
  ExtismPointer ptr = extism_alloc(strlen(msg));
  assert(ptr > 0);
  extism_store(ptr, (const uint8_t *)msg, strlen(msg));
  assert(extism_length(ptr) == strlen(msg));
  extism_output_set(ptr, strlen(msg));
  return 0;
}

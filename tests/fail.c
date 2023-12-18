#define EXTISM_IMPLEMENTATION
#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  const char *msg = "Some error message";
  ExtismPointer ptr = extism_alloc(extism_strlen(msg));
  extism_store(ptr, (const uint8_t *)msg, extism_strlen(msg));
  extism_error_set(ptr);
  return 1;
}

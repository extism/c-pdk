#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  const char *msg = "Some error message";
  ExtismPointer ptr = extism_alloc(strlen(msg));
  extism_store(ptr, (const uint8_t *)msg, strlen(msg));
  extism_error_set(ptr);
  return 1;
}

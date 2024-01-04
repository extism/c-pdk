#define EXTISM_IMPLEMENTATION
#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  const char *msg = "Some error message";
  ExtismHandle h = extism_alloc(extism_strlen(msg));
  extism_store_to_handle(h, 0, (const uint8_t *)msg, extism_strlen(msg));
  extism_error_set(h);
  return 1;
}

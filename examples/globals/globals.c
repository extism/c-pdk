#define EXTISM_IMPLEMENTATION
#include "../../extism-pdk.h"

#include <stdio.h>

uint64_t count = 0;

int32_t EXTISM_EXPORTED_FUNCTION(globals) {
  char out[128];
  int n = snprintf(out, 128, "{\"count\": %llu}", count);

  ExtismHandle buf = extism_alloc(n);
  extism_store_to_handle(buf, 0, out, n);
  extism_output_set_from_handle(buf, 0, n);

  count += 1;

  return 0;
}

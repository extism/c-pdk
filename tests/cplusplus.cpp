#include "util.h"
#include <string.h>

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  const char *msg = "Hello, world!";
  ExtismHandle buf = extism_alloc(strlen(msg));
  assert(buf > 0);
  extism_store_to_handle(buf, 0, msg, strlen(msg));
  assert(extism_length(buf) == strlen(msg));
  extism_output_handle(buf);
  return 0;
}

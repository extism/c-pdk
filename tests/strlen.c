#define EXTISM_IMPLEMENTATION
#include "util.h"
#include <string.h>

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  const char *hello = "hello";
  assert(strlen(hello) == extism_strlen(hello));
  assert(strlen("") == extism_strlen(""));
  return 0;
}

#define EXTISM_USE_LIBC
#define EXTISM_IMPLEMENTATION
#include "util.h"
#include <stdlib.h>

EXTISM_EXPORT_AS("run_test") int32_t run_test(void) {
  void *input = extism_load_input_dup(NULL);
  if (input != NULL) {
    free(input);
  }
  size_t outSize;
  if ((input = extism_load_input_dup(&outSize))) {
    assert(outSize == 0);
    free(input);
  }
  char *sz = extism_load_input_sz_dup(NULL);
  assert(sz != NULL);
  assert(*sz == '\0');
  free(sz);
  sz = extism_load_input_sz_dup(&outSize);
  assert(sz != NULL);
  assert(*sz == '\0');
  assert(outSize == 1);
  free(sz);
  return 0;
}

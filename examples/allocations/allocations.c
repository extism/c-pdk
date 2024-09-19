#define EXTISM_IMPLEMENTATION
#include "../../extism-pdk.h"

uint64_t count = 0;

int32_t EXTISM_EXPORTED_FUNCTION(allocations) {

  for (int i = 0; i < 10000; i++) {
    ExtismHandle h = extism_alloc(i + 10);
    if (i % 3 == 0) {
      extism_free(h);
    }
  }

  return 0;
}

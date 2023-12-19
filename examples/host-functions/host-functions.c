#define EXTISM_ENABLE_LOW_LEVEL_API
#define EXTISM_IMPLEMENTATION
#include "../../extism-pdk.h"

#include <stdio.h>

EXTISM_IMPORT("extism:host/user", "hello_world")
extern ExtismHandle hello_world(ExtismHandle);

int32_t EXTISM_EXPORTED_FUNCTION(count_vowels) {
  uint64_t length = extism_input_length();

  if (length == 0) {
    return 0;
  }

  int64_t count = 0;
  char ch = 0;
  for (int64_t i = 0; i < length; i++) {
    ch = extism_input_load_u8(i);
    if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' ||
        ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') {
      count += 1;
    }
  }

  char out[128];
  int n = snprintf(out, 128, "{\"count\": %lld}", count);
  ExtismHandle buf = extism_alloc(n);
  extism_store_to_handle(buf, 0, out, n);
  buf = hello_world(buf);
  extism_output_set(buf, extism_length(buf));
  return 0;
}

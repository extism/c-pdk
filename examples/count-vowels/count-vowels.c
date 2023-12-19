#define EXTISM_ENABLE_LOW_LEVEL_API
#define EXTISM_IMPLEMENTATION
#include "../../extism-pdk.h"

#include <stdio.h>
#include <string.h>

int32_t EXTISM_EXPORTED_FUNCTION(count_vowels) {
  uint64_t count = 0;
  uint8_t ch = 0;
  uint64_t length = extism_input_length();

  for (uint64_t i = 0; i < length; i++) {
    ch = extism_input_load_u8(i);
    count += (ch == 'A') + (ch == 'a') + (ch == 'E') + (ch == 'e') +
             (ch == 'I') + (ch == 'i') + (ch == 'O') + (ch == 'o') +
             (ch == 'U') + (ch == 'u');
  }

  char out[128];
  int n = snprintf(out, 128, "{\"count\": %llu}", count);

  ExtismHandle buf = extism_alloc(n);
  extism_store(buf, out, n);
  extism_output_set(buf, n);

  return 0;
}

#pragma once

#include "../extism-pdk.h"

#define assert(x)                                                              \
  if (!(x)) {                                                                  \
    __builtin_unreachable();                                                   \
  }

static unsigned long strlen(const char *s) {
  unsigned long count = 0;

  while (s && *s) {
    s += 1;
    count += 1;
  }

  return count;
}

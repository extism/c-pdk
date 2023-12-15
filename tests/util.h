#pragma once

#include "../extism-pdk.h"

#define assert(x)                                                              \
  if (!(x)) {                                                                  \
    __builtin_unreachable();                                                   \
  }

#pragma once

#include <stdint.h>

typedef uint64_t ExtismPointer;

#define EXTISM_ENV_MODULE "extism:host/env"
#define EXTISM_USER_MODULE "extism:host/user"

#define IMPORT(a, b) __attribute__((import_module(a), import_name(b)))
#define IMPORT_ENV(b)                                                          \
  __attribute__((import_module(EXTISM_ENV_MODULE), import_name(b)))
#define IMPORT_USER(b)                                                         \
  __attribute__((import_module(EXTISM_USER_MODULE), import_name(b)))

IMPORT_ENV("input_length")
extern uint64_t extism_input_length();
IMPORT_ENV("length")
extern uint64_t extism_length(ExtismPointer);
IMPORT_ENV("alloc")
extern ExtismPointer extism_alloc(uint64_t);
IMPORT_ENV("free") extern void extism_free(ExtismPointer);

IMPORT_ENV("input_load_u8")
extern uint8_t extism_input_load_u8(ExtismPointer);

IMPORT_ENV("input_load_u64")
extern uint64_t extism_input_load_u64(ExtismPointer);

IMPORT_ENV("output_set")
extern void extism_output_set(ExtismPointer, uint64_t);

IMPORT_ENV("error_set")
extern void extism_error_set(ExtismPointer);

IMPORT_ENV("config_get")
extern ExtismPointer extism_config_get(ExtismPointer);

IMPORT_ENV("var_get")
extern ExtismPointer extism_var_get(ExtismPointer);

IMPORT_ENV("var_set")
extern void extism_var_set(ExtismPointer, ExtismPointer);

IMPORT_ENV("store_u8")
extern void extism_store_u8(ExtismPointer, uint8_t);

IMPORT_ENV("load_u8")
extern uint8_t extism_load_u8(ExtismPointer);

IMPORT_ENV("store_u64")
extern void extism_store_u64(ExtismPointer, uint64_t);

IMPORT_ENV("load_u64")
extern uint64_t extism_load_u64(ExtismPointer);

IMPORT_ENV("http_request")
extern ExtismPointer extism_http_request(ExtismPointer, ExtismPointer);

IMPORT_ENV("http_status_code")
extern int32_t extism_http_status_code();

IMPORT_ENV("log_info")
extern void extism_log_info(ExtismPointer);
IMPORT_ENV("log_debug")
extern void extism_log_debug(ExtismPointer);
IMPORT_ENV("log_warn")
extern void extism_log_warn(ExtismPointer);
IMPORT_ENV("log_error")
extern void extism_log_error(ExtismPointer);

// Load data from Extism memory
static void extism_load(ExtismPointer offs, uint8_t *buffer, uint64_t length) {
  uint64_t n;
  uint64_t left = 0;

  for (uint64_t i = 0; i < length; i += 1) {
    left = length - i;
    if (left < 8) {
      buffer[i] = extism_load_u8(offs + i);
      continue;
    }

    n = extism_load_u64(offs + i);
    *((uint64_t *)buffer + (i / 8)) = n;
    i += 7;
  }
}

// Load data from input buffer
static void extism_load_input(uint8_t *buffer, uint64_t length) {
  uint64_t n;
  uint64_t left = 0;

  for (uint64_t i = 0; i < length; i += 1) {
    left = length - i;
    if (left < 8) {
      buffer[i] = extism_input_load_u8(i);
      continue;
    }

    n = extism_input_load_u64(i);
    *((uint64_t *)buffer + (i / 8)) = n;
    i += 7;
  }
}

// Copy data into Extism memory
static void extism_store(ExtismPointer offs, const uint8_t *buffer,
                         uint64_t length) {
  uint64_t n;
  uint64_t left = 0;
  for (uint64_t i = 0; i < length; i++) {
    left = length - i;
    if (left < 8) {
      extism_store_u8(offs + i, buffer[i]);
      continue;
    }

    n = *((uint64_t *)buffer + (i / 8));
    extism_store_u64(offs + i, n);
    i += 7;
  }
}

// Allocate a string and copy the provided value into Extism memory
static ExtismPointer extism_alloc_string(const char *s, uint64_t length) {
  ExtismPointer ptr = extism_alloc(length);
  extism_store(ptr, (const uint8_t *)s, length);
  return ptr;
}

typedef enum {
  ExtismLogInfo,
  ExtismLogDebug,
  ExtismLogWarn,
  ExtismLogError,
} ExtismLog;

// Write to Extism log
static void extism_log(const char *s, uint64_t len, ExtismLog level) {
  ExtismPointer ptr = extism_alloc(len);
  extism_store(ptr, (const uint8_t *)s, len);
  switch (level) {
  case ExtismLogInfo:
    extism_log_info(ptr);
    break;
  case ExtismLogDebug:
    extism_log_debug(ptr);
    break;
  case ExtismLogWarn:
    extism_log_warn(ptr);
    break;
  case ExtismLogError:
    extism_log_error(ptr);
    break;
  }
  extism_free(ptr);
}

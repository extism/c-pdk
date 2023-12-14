#pragma once

#include <stdint.h>

typedef uint64_t ExtismPointer;

#define EXTISM_ENV_MODULE "extism:host/env"
#define EXTISM_USER_MODULE "extism:host/user"

#define EXTISM_EXPORT_AS(name) __attribute__((export_name(name)))
#define EXTISM_EXPORTED_FUNCTION(name)                                         \
  EXTISM_EXPORT_AS(#name)                                                      \
  name(void)

#define IMPORT(a, b) __attribute__((import_module(a), import_name(b)))
#define IMPORT_ENV(b)                                                          \
  __attribute__((import_module(EXTISM_ENV_MODULE), import_name(b)))
#define IMPORT_USER(b)                                                         \
  __attribute__((import_module(EXTISM_USER_MODULE), import_name(b)))

IMPORT_ENV("input_length")
extern uint64_t extism_input_length(void);
IMPORT_ENV("length")
extern uint64_t extism_length(const ExtismPointer);
IMPORT_ENV("alloc")
extern ExtismPointer extism_alloc(const uint64_t);
IMPORT_ENV("free")
extern void extism_free(ExtismPointer);
IMPORT_ENV("input_load_u8")
extern uint8_t extism_input_load_u8(const ExtismPointer);

IMPORT_ENV("input_load_u64")
extern uint64_t extism_input_load_u64(const ExtismPointer);

IMPORT_ENV("output_set")
extern void extism_output_set(const ExtismPointer, const uint64_t);

IMPORT_ENV("error_set")
extern void extism_error_set(const ExtismPointer);

IMPORT_ENV("config_get")
extern ExtismPointer extism_config_get(const ExtismPointer);

IMPORT_ENV("var_get")
extern ExtismPointer extism_var_get(const ExtismPointer);

IMPORT_ENV("var_set")
extern void extism_var_set(ExtismPointer, const ExtismPointer);

IMPORT_ENV("store_u8")
extern void extism_store_u8(ExtismPointer, const uint8_t);

IMPORT_ENV("load_u8")
extern uint8_t extism_load_u8(const ExtismPointer);

IMPORT_ENV("store_u64")
extern void extism_store_u64(ExtismPointer, const uint64_t);

IMPORT_ENV("load_u64")
extern uint64_t extism_load_u64(const ExtismPointer);

IMPORT_ENV("http_request")
extern ExtismPointer extism_http_request(const ExtismPointer,
                                         const ExtismPointer);

IMPORT_ENV("http_status_code")
extern int32_t extism_http_status_code(void);

IMPORT_ENV("log_info")
extern void extism_log_info(const ExtismPointer);
IMPORT_ENV("log_debug")
extern void extism_log_debug(const ExtismPointer);
IMPORT_ENV("log_warn")
extern void extism_log_warn(const ExtismPointer);
IMPORT_ENV("log_error")
extern void extism_log_error(const ExtismPointer);

// Load data from Extism memory
static void extism_load(const ExtismPointer offs, void *buffer,
                        const size_t length) {
  const size_t chunk_count = length >> 3;
  uint64_t *i64_buffer = buffer;
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    i64_buffer[chunk_idx] = extism_load_u64(offs + (chunk_idx << 3));
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (length & 7);
  for (uint8_t *u8_buffer = buffer; remainder_offset < remainder_end;
       remainder_offset++) {
    u8_buffer[remainder_offset] = extism_load_u8(offs + remainder_offset);
  }
}

// Load data from input buffer
static void extism_load_input(void *buffer, const size_t length) {
  const size_t chunk_count = length >> 3;
  uint64_t *i64_buffer = buffer;
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    i64_buffer[chunk_idx] = extism_input_load_u64(chunk_idx << 3);
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (length & 7);
  for (uint8_t *u8_buffer = buffer; remainder_offset < remainder_end;
       remainder_offset++) {
    u8_buffer[remainder_offset] = extism_input_load_u8(remainder_offset);
  }
}

// Copy data into Extism memory
static void extism_store(ExtismPointer offs, const void *buffer,
                         const size_t length) {
  const size_t chunk_count = length >> 3;
  const uint64_t *i64_buffer = buffer;
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    extism_store_u64(offs + (chunk_idx << 3), i64_buffer[chunk_idx]);
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (length & 7);
  for (const uint8_t *u8_buffer = buffer; remainder_offset < remainder_end;
       remainder_offset++) {
    extism_store_u8(offs + remainder_offset, u8_buffer[remainder_offset]);
  }
}

// Allocate a buffer in Extism memory and copy into it
static ExtismPointer extism_alloc_buf(const void *src, const size_t length) {
  ExtismPointer ptr = extism_alloc(length);
  extism_store(ptr, src, length);
  return ptr;
}

__attribute__((
    deprecated("Use extism_alloc_buf instead."))) static inline ExtismPointer
extism_alloc_string(const char *s, const size_t length) {
  return extism_alloc_buf(s, length);
}

typedef enum {
  ExtismLogInfo,
  ExtismLogDebug,
  ExtismLogWarn,
  ExtismLogError,
} ExtismLog;

// Write to Extism log
static void extism_log(const char *s, const size_t len, const ExtismLog level) {
  ExtismPointer ptr = extism_alloc(len);
  extism_store(ptr, s, len);
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

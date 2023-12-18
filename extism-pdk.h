#ifndef extism_pdk_h
#define extism_pdk_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t ExtismPointer;

#define EXTISM_ENV_MODULE "extism:host/env"
#define EXTISM_USER_MODULE "extism:host/user"

#define EXTISM_EXPORT_AS(name) __attribute__((export_name(name)))
#define EXTISM_EXPORTED_FUNCTION(name)                                         \
  EXTISM_EXPORT_AS(#name)                                                      \
  name(void)

#define EXTISM_IMPORT(a, b) __attribute__((import_module(a), import_name(b)))
#define EXTISM_IMPORT_ENV(b)                                                   \
  __attribute__((import_module(EXTISM_ENV_MODULE), import_name(b)))
#define EXTISM_IMPORT_USER(b)                                                  \
  __attribute__((import_module(EXTISM_USER_MODULE), import_name(b)))

EXTISM_IMPORT_ENV("input_length")
extern uint64_t extism_input_length(void);
EXTISM_IMPORT_ENV("length")
extern uint64_t extism_length(const ExtismPointer);
EXTISM_IMPORT_ENV("alloc")
extern ExtismPointer extism_alloc(const uint64_t);
EXTISM_IMPORT_ENV("free")
extern void extism_free(ExtismPointer);
EXTISM_IMPORT_ENV("input_load_u8")
extern uint8_t extism_input_load_u8(const ExtismPointer);

EXTISM_IMPORT_ENV("input_load_u64")
extern uint64_t extism_input_load_u64(const ExtismPointer);

EXTISM_IMPORT_ENV("output_set")
extern void extism_output_set(const ExtismPointer, const uint64_t);

EXTISM_IMPORT_ENV("error_set")
extern void extism_error_set(const ExtismPointer);

EXTISM_IMPORT_ENV("config_get")
extern ExtismPointer extism_config_get(const ExtismPointer);

EXTISM_IMPORT_ENV("var_get")
extern ExtismPointer extism_var_get(const ExtismPointer);

EXTISM_IMPORT_ENV("var_set")
extern void extism_var_set(ExtismPointer, const ExtismPointer);

EXTISM_IMPORT_ENV("store_u8")
extern void extism_store_u8(ExtismPointer, const uint8_t);

EXTISM_IMPORT_ENV("load_u8")
extern uint8_t extism_load_u8(const ExtismPointer);

EXTISM_IMPORT_ENV("store_u64")
extern void extism_store_u64(ExtismPointer, const uint64_t);

EXTISM_IMPORT_ENV("load_u64")
extern uint64_t extism_load_u64(const ExtismPointer);

EXTISM_IMPORT_ENV("http_request")
extern ExtismPointer extism_http_request(const ExtismPointer,
                                         const ExtismPointer);

EXTISM_IMPORT_ENV("http_status_code")
extern int32_t extism_http_status_code(void);

EXTISM_IMPORT_ENV("log_info")
extern void extism_log_info(const ExtismPointer);
EXTISM_IMPORT_ENV("log_debug")
extern void extism_log_debug(const ExtismPointer);
EXTISM_IMPORT_ENV("log_warn")
extern void extism_log_warn(const ExtismPointer);
EXTISM_IMPORT_ENV("log_error")
extern void extism_log_error(const ExtismPointer);

// Load data from Extism memory, does not verify load is in bounds
void extism_load(const ExtismPointer offs, void *dest, const size_t n);

// Load data from input buffer, verifies load is inbounds
bool extism_load_input(void *dest, const size_t n);

// Load n-1 bytes from input buffer and zero terminate
// Verifies load is inbounds
bool extism_load_input_sz(char *dest, const size_t n);

// Copy data into Extism memory
void extism_store(ExtismPointer offs, const void *buffer, const size_t length);

// Allocate a buffer in Extism memory and copy into it
ExtismPointer extism_alloc_buf(const void *src, const size_t n);

__attribute__((
    deprecated("Use extism_alloc_buf instead."))) static inline ExtismPointer
extism_alloc_string(const char *s, const size_t n) {
  return extism_alloc_buf(s, n);
}

#ifdef EXTISM_USE_LIBC
// get the input length (n) and malloc(n), load n bytes from Extism memory
// into it. If outSize is provided, set it to n
void *extism_load_input_dup(size_t *outSize);

// get the input length, add 1 to it to get n. malloc(n), load n - 1 bytes
// from Extism memory into it. Zero terminate. If outSize is provided, set it
// to n
void *extism_load_input_sz_dup(size_t *outSize);

#endif // EXTISM_USE_LIBC

// Allocate a buffer in Extism memory and copy string data into it
// copied string is NOT null terminated
ExtismPointer extism_alloc_buf_from_sz(const char *sz);

typedef enum {
  ExtismLogInfo,
  ExtismLogDebug,
  ExtismLogWarn,
  ExtismLogError,
} ExtismLog;

// Write to Extism log
void extism_log(const char *s, const size_t len, const ExtismLog level);

// Write zero-terminated string to Extism log
void extism_log_sz(const char *s, const ExtismLog level);

#ifdef __cplusplus
}
#endif
#endif // extism_pdk_h

// avoid greying out the implementation section
#if defined(Q_CREATOR_RUN) || defined(__INTELLISENSE__) ||                     \
    defined(_CDT_PARSER__)
#define EXTISM_IMPLEMENTATION
#endif

#ifdef EXTISM_IMPLEMENTATION
#ifndef extism_pdk_c
#define extism_pdk_c

// Load data from Extism memory, does not verify load is in bounds
void extism_load(const ExtismPointer offs, void *dest, const size_t n) {
  const size_t chunk_count = n >> 3;
  uint64_t *i64_buffer = dest;
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    i64_buffer[chunk_idx] = extism_load_u64(offs + (chunk_idx << 3));
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (n & 7);
  for (uint8_t *u8_buffer = dest; remainder_offset < remainder_end;
       remainder_offset++) {
    u8_buffer[remainder_offset] = extism_load_u8(offs + remainder_offset);
  }
}

// Load data from input buffer, does not verify load is inbounds
static void extism_load_input_unsafe(void *dest, const size_t n) {
  const size_t chunk_count = n >> 3;
  uint64_t *i64_buffer = dest;
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    i64_buffer[chunk_idx] = extism_input_load_u64(chunk_idx << 3);
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (n & 7);
  for (uint8_t *u8_buffer = dest; remainder_offset < remainder_end;
       remainder_offset++) {
    u8_buffer[remainder_offset] = extism_input_load_u8(remainder_offset);
  }
}

// Load data from input buffer, verifies load is inbounds
bool extism_load_input(void *dest, const size_t n) {
  const uint64_t input_len = extism_input_length();
  if (n > input_len) {
    return false;
  }
  extism_load_input_unsafe(dest, n);
  return true;
}

// Load n-1 bytes from input buffer and zero terminate
// Does not verify load is inbounds
static void extism_load_input_sz_unsafe(char *dest, const size_t n) {
  extism_load_input_unsafe(dest, n - 1);
  dest[n - 1] = '\0';
}

// Load n-1 bytes from input buffer and zero terminate
// Verifies load is inbounds
bool extism_load_input_sz(char *dest, const size_t n) {
  const uint64_t input_len = extism_input_length();
  if ((n - 1) > input_len) {
    return false;
  }
  extism_load_input_sz_unsafe(dest, n);
  return true;
}

// Copy data into Extism memory
void extism_store(ExtismPointer offs, const void *buffer, const size_t length) {
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
ExtismPointer extism_alloc_buf(const void *src, const size_t n) {
  ExtismPointer ptr = extism_alloc(n);
  extism_store(ptr, src, n);
  return ptr;
}

#ifdef EXTISM_USE_LIBC
#include <stdlib.h>
#include <string.h>

#define extism_strlen strlen

// get the input length (n) and malloc(n), load n bytes from Extism memory
// into it. If outSize is provided, set it to n
void *extism_load_input_dup(size_t *outSize) {
  const uint64_t n = extism_input_length();
  if (n > SIZE_MAX) {
    return NULL;
  }
  void *buf = malloc(n);
  if (!buf) {
    return NULL;
  }
  extism_load_input_unsafe(buf, n);
  if (outSize) {
    *outSize = n;
  }
  return buf;
}

// get the input length, add 1 to it to get n. malloc(n), load n - 1 bytes
// from Extism memory into it. Zero terminate. If outSize is provided, set it
// to n
void *extism_load_input_sz_dup(size_t *outSize) {
  uint64_t n = extism_input_length();
  if (n > (SIZE_MAX - 1)) {
    return NULL;
  }
  n++;
  char *buf = malloc(n);
  if (!buf) {
    return NULL;
  }
  extism_load_input_sz_unsafe(buf, n);
  if (outSize) {
    *outSize = n;
  }
  return buf;
}

#else
static size_t extism_strlen(const char *sz) {
  size_t len;
  for (len = 0; sz[len] != '\0'; len++) {
  }
  return len;
}
#endif

// Allocate a buffer in Extism memory and copy string data into it
// copied string is NOT null terminated
ExtismPointer extism_alloc_buf_from_sz(const char *sz) {
  return extism_alloc_buf(sz, extism_strlen(sz));
}

// Write to Extism log
void extism_log(const char *s, const size_t len, const ExtismLog level) {
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

// Write zero-terminated string to Extism log
void extism_log_sz(const char *s, const ExtismLog level) {
  const size_t len = extism_strlen(s);
  extism_log(s, len, level);
}

#endif // extism_pdk_c
#endif // EXTISM_IMPLEMENTATION

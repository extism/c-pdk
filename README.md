# Extism C PDK

This project contains a tool that can be used to create [Extism Plug-ins](https://extism.org/docs/concepts/plug-in) in C.

## Installation

The Extism C PDK is a single header library. Just copy [extism-pdk.h](https://github.com/extism/c-pdk/blob/main/extism-pdk.h) into your project or add this repo as a Git submodule:

```shell
git submodule add https://github.com/extism/c-pdk extism-pdk
```

## Getting Started

The goal of writing an [Extism plug-in](https://extism.org/docs/concepts/plug-in) is to compile your C code to a Wasm module with exported functions that the host application can invoke.
The first thing you should understand is creating an export.

### Exports

Let's write a simple program that exports a `greet` function which will take a name as a string and return a greeting string. Paste this into a file `plugin.c`:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdint.h>

#define Greet_Max_Input 1024
static const char Greeting[] = "Hello, ";

int32_t EXTISM_EXPORTED_FUNCTION(greet) {
  uint64_t inputLen = extism_input_length();
  if (inputLen > Greet_Max_Input) {
    inputLen = Greet_Max_Input;
  }

  // Load input
  static uint8_t inputData[Greet_Max_Input];
  extism_load_input(0, inputData, inputLen);

  // Allocate memory to store greeting and name
  const uint64_t greetingLen = sizeof(Greeting) - 1;
  const uint64_t outputLen = greetingLen + inputLen;
  ExtismHandle handle = extism_alloc(outputLen);
  extism_store_to_handle(handle, 0, Greeting, greetingLen);
  extism_store_to_handle(handle, greetingLen, inputData, inputLen);

  // Set output
  extism_output_set_from_handle(handle, 0, outputLen);
  return 0;
}
```

The `EXTISM_EXPORTED_FUNCTION` macro simplifies declaring an Extism function that will be exported to the host.

The `load`, `store`, and `alloc` functions are used to load from, store to and allocate Extism memory. Extism eases passing data to and from the host and the plug-in by managing memory isolated from both the host and the plug-in/Wasm module. For more details, see the [Memory](https://extism.org/docs/concepts/memory) concept page.

Since we don't need any system access for this, we can compile this directly with clang:

```shell
clang -o plugin.wasm --target=wasm32-unknown-unknown -nostdlib -Wl,--no-entry plugin.c
```

The above command may fail if ran with system clang. It's highly recommended to use clang from the [wasi-sdk](https://github.com/WebAssembly/wasi-sdk) instead. The `wasi-sdk` also includes a libc implementation targeting WASI, necessary for plugins that need the C standard library.

Let's break down the command a little:

- `--target=wasm32-unknown-unknown` configures the correct Webassembly target
- `-nostdlib` tells the compiler not to link the standard library
- `-Wl,--no-entry` is a linker flag to tell the linker there is no `_start` function

We can now test `plugin.wasm` using the [Extism CLI](https://github.com/extism/cli)'s `call`
command:

```bash
extism call plugin.wasm greet --input="Benjamin"
# => Hello, Benjamin
```

### More Exports: Error Handling

We catch any exceptions thrown and return them as errors to the host. Suppose we want to re-write our greeting module to never greet Benjamins:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define Greet_Max_Input 1024
static const char Greeting[] = "Hello, ";

static bool is_benjamin(const char *name) {
  return strcasecmp(name, "benjamin") == 0;
}

int32_t EXTISM_EXPORTED_FUNCTION(greet) {
  uint64_t inputLen = extism_input_length();
  const uint64_t greetMaxString = Greet_Max_Input - 1;
  if (inputLen > greetMaxString) {
    inputLen = greetMaxString;
  }

  // Load input
  static uint8_t inputData[Greet_Max_Input];
  extism_load_input(0, inputData, inputLen);
  inputData[inputLen] = '\0';

  // Check if the input matches "benjamin", if it does
  // return an error
  if (is_benjamin((const char *)inputData)) {
    ExtismHandle err = extism_alloc_buf_from_sz("ERROR");
    extism_error_set(err);
    return -1;
  }

  // Allocate memory to store greeting and name
  const uint64_t greetingLen = sizeof(Greeting) - 1;
  const uint64_t outputLen = greetingLen + inputLen;
  ExtismHandle handle = extism_alloc(outputLen);
  extism_store_to_handle(handle, 0, Greeting, greetingLen);
  extism_store_to_handle(handle, greetingLen, inputData, inputLen);

  // Set output
  extism_output_set_from_handle(handle, 0, outputLen);
  return 0;
}
```

This time we will compile our example using [wasi-sdk](https://github.com/WebAssembly/wasi-sdk), since we used the `<string.h>` header file. And because we are targeting 
`wasm32-wasi`, we will need to add the `-mexec-model=reactor` flag to be able to export specific functions instead of a single `_start` function:

```bash
$WASI_SDK_PATH/bin/clang -o plugin.wasm plugin.c -mexec-model=reactor
extism call plugin.wasm greet --input="Benjamin" --wasi
# => Error: ERROR
echo $? # print last status code
# => 1
extism call plugin.wasm greet --input="Zach" --wasi
# => Hello, Zach
echo $?
# => 0
```

### Configs

Configs are key-value pairs that can be passed in by the host when creating a
plug-in. These can be useful to statically configure the plug-in with some data that exists across every function call. Here is a trivial example using `extism_config_get`:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdint.h>
#include <stdlib.h>

#define Greet_Max_Input 1024
static const char Greeting[] = "Hello, ";

int32_t EXTISM_EXPORTED_FUNCTION(greet) {
  ExtismHandle key = extism_alloc_buf_from_sz("user");
  ExtismHandle value = extism_config_get(key);

  if (value == 0) {
    ExtismHandle err = extism_alloc_buf_from_sz("Invalid key");
    extism_error_set(err);
    return -1;
  }

  const uint64_t valueLen = extism_length(value);

  // Load config value
  uint8_t *valueData = malloc(valueLen);
  if (valueData == NULL) {
    ExtismHandle err = extism_alloc_buf_from_sz("OOM");
    extism_error_set(err);
    return -1;
  }
  extism_load_from_handle(value, 0, valueData, valueLen);

  // Allocate memory to store greeting and name
  const uint64_t greetingLen = sizeof(Greeting) - 1;
  const uint64_t outputLen = greetingLen + valueLen;
  ExtismHandle handle = extism_alloc(outputLen);
  extism_store_to_handle(handle, 0, Greeting, greetingLen);
  extism_store_to_handle(handle, greetingLen, valueData, valueLen);
  free(valueData);

  // Set output
  extism_output_set_from_handle(handle, 0, outputLen);
  return 0;
}
```

To test it, the [Extism CLI](https://github.com/extism/cli) has a `--config` option that lets you pass in `key=value` pairs:


```bash
extism call plugin.wasm greet --config user=Benjamin
# => Hello, Benjamin
```

### Variables

Variables are another key-value mechanism but it's a mutable data store that
will persist across function calls. These variables will persist as long as the
host has loaded and not freed the plug-in. 
You can use `extism_var_get`, and `extism_var_set` to manipulate vars:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdint.h>

int32_t EXTISM_EXPORTED_FUNCTION(count) {
  ExtismHandle key = extism_alloc_buf_from_sz("count");
  ExtismHandle value = extism_var_get(key);

  uint64_t count = 0;
  if (value != 0) {
    extism_load_from_handle(value, 0, &count, sizeof(uint64_t));
  }
  count += 1;

  // Allocate a new value if it isn't saved yet
  if (value == 0) {
    value = extism_alloc(sizeof(uint64_t));
  }

  // Update the memory block
  extism_store_to_handle(value, 0, &count, sizeof(uint64_t));

  // Set the variable
  extism_var_set(key, value);

  return 0;
}
```

### Logging

The `extism_log*` functions can be used to emit logs:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdint.h>

int32_t EXTISM_EXPORTED_FUNCTION(log_stuff) {
  ExtismHandle msg = extism_alloc_buf_from_sz("Hello!");
  extism_log_info(msg);
  extism_log_debug(msg);
  extism_log_warn(msg);
  extism_log_error(msg);
  extism_log_sz("Hello!", ExtismLogInfo);
  return 0;
}
```

Running it, you need to pass a log-level flag:

```
extism call plugin.wasm log_stuff --log-level=info
# => 2023/10/17 14:25:00 Hello!
# => 2023/10/17 14:25:00 Hello!
# => 2023/10/17 14:25:00 Hello!
# => 2023/10/17 14:25:00 Hello!
# => 2023/10/17 14:25:00 Hello!
# => 2023/10/17 14:25:00 Hello!
```

### HTTP

HTTP calls can be made using `extism_http_request`: 

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdint.h>
#include <string.h>

int32_t EXTISM_EXPORTED_FUNCTION(call_http) {
  const char *reqStr = "{\
    \"method\": \"GET\",\
    \"url\": \"https://jsonplaceholder.typicode.com/todos/1\"\
  }";

  ExtismHandle req = extism_alloc_buf_from_sz(reqStr);
  ExtismHandle res = extism_http_request(req, 0);

  if (extism_http_status_code() != 200) {
    return -1;
  }

  extism_output_set_from_handle(res, 0, extism_length(res));
  return 0;
}
```

To test it you will need to pass `--allow-host jsonplaceholder.typicode.com` to the `extism` CLI, otherwise the HTTP request will
be rejected.

## Imports (Host Functions)

Like any other code module, Wasm not only let's you export functions to the outside world, you can
import them too. Host Functions allow a plug-in to import functions defined in the host. For example,
if you host application is written in Python, it can pass a Python function down to your C plug-in
where you can invoke it.

This topic can get fairly complicated and we have not yet fully abstracted the Wasm knowledge you need
to do this correctly. So we recommend reading out [concept doc on Host Functions](https://extism.org/docs/concepts/host-functions) before you get started.

### A Simple Example

Host functions have a similar interface as exports. You just need to declare them as `extern` on the top of your header file. You only declare the interface as it is the host's responsibility to provide the implementation:

```c
extern ExtismHandle a_python_func(ExtismHandle);
```

A namespace may be set for an import using the `IMPORT` macro in `extism-pdk.h`:

```c
IMPORT("my_module", "a_python_func") extern ExtismHandle a_python_func(ExtismHandle);
```

> **Note**: The types we accept here are the same as the exports as the interface also uses the [convert crate](https://docs.rs/extism-convert/latest/extism_convert/).

To call this function, we pass an Extism handle and receive one back:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
#include <stdint.h>

int32_t EXTISM_EXPORTED_FUNCTION(hello_from_python) {
  ExtismHandle arg = extism_alloc_buf_from_sz("Hello!");
  ExtismHandle res = a_python_func(arg);
  extism_free(arg);
  extism_output_set_from_handle(res, 0, extism_length(res));
  return 0;
}
```

### Testing it out

We can't really test this from the Extism CLI as something must provide the implementation. So let's
write out the Python side here. Check out the [docs for Host SDKs](https://extism.org/docs/concepts/host-sdk) to implement a host function in a language of your choice.

```python
from extism import host_fn, Plugin

@host_fn()
def a_python_func(input: str) -> str:
    # just printing this out to prove we're in Python land
    print("Hello from Python!")

    # let's just add "!" to the input string
    # but you could imagine here we could add some
    # applicaiton code like query or manipulate the database
    # or our application APIs
    return input + "!"
```

Now when we load the plug-in we pass the host function:
 
```python
manifest = {"wasm": [{"path": "/path/to/plugin.wasm"}]}
plugin = Plugin(manifest, functions=[a_python_func], wasi=True)
result = plugin.call('hello_from_python', b'').decode('utf-8')
print(result)
```

```bash
python3 app.py
# => Hello from Python!
# => An argument to send to Python!
```

## Building

One source file must contain the implementation:

```c
#define EXTISM_IMPLEMENTATION
#include "extism-pdk.h"
```

All other source files using the pdk must include the header without `#define EXTISM_IMPLEMENTATION`

The C PDK does not require building with `libc`, but additional functions can be enabled when `libc` is available. `#define EXTISM_USE_LIBC` in each file before including the pdk (everywhere it is included) or, when compiling, pass it as a flag to clang: `-D EXTISM_USE_LIBC`

The low-level API that operates on `ExtismPointer` is no longer included by default, `#define EXTISM_ENABLE_LOW_LEVEL_API` in each file before including the pdk (everywhere it is included) or, when compiling, pass it as a flag to clang: `-D EXTISM_ENABLE_LOW_LEVEL_API` . Updating to use the `ExtismHandle`-based API is highly recommended.

The C PDK may be used from C++, however, the implementation must be built with a C compiler. See `cplusplus` in `tests/Makefile` for an example.

## Exports (details)

The `EXTISM_EXPORTED_FUNCTION` macro is not essential to create a plugin function and export it to the host. You may instead write a function and then export it when linking. For example, the first example may have the following signature instead:

```c
int32_t greet(void)
```

Then, it can be built and linked with:

```bash
$WASI_SDK_PATH/bin/clang -o plugin.wasm --target=wasm32-unknown-unknown -nostdlib -Wl,--no-entry -Wl,--export=greet plugin.c
```

Note the `-Wl,--export=greet`

Exports names do not necessarily have to match the function name either. Going back to the first example again. Try:

```c
EXTISM_EXPORT_AS("greet") int32_t internal_name_for_greet(void)
```

and build with:

```bash
$WASI_SDK_PATH/bin/clang -o plugin.wasm --target=wasm32-unknown-unknown -nostdlib -Wl,--no-entry plugin.c
```

## Reach Out!

Have a question or just want to drop in and say hi? [Hop on the Discord](https://extism.org/discord)!

WASI_SDK_PATH?=../wasi-sdk

test: test-alloc test-hello test-load_input_bounds_checks test-strlen test-use_libc test-alloc_buf_from_sz test-cplusplus
run-fail: test-fail
build: alloc hello fail load_input_bounds_checks strlen use_libc alloc_buf_from_sz cplusplus

cplusplus:
	$(WASI_SDK_PATH)/bin/clang++ --target=wasm32-wasi -c $@.cpp
	$(WASI_SDK_PATH)/bin/clang --target=wasm32-wasi -c implementation.c
	$(WASI_SDK_PATH)/bin/clang++ --target=wasm32-wasi -o $@.wasm $@.o implementation.o -mexec-model=reactor

strlen use_libc:
	$(WASI_SDK_PATH)/bin/clang --target=wasm32-wasi -o $@.wasm $@.c -mexec-model=reactor

%: %.c
	$(WASI_SDK_PATH)/bin/clang --target=wasm32-unknown-unknown -o $*.wasm $*.c -Wl,--no-entry -nostdlib

test-%: %
	extism call $*.wasm run_test

clean:
	rm -f *.wasm

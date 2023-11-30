WASI_SDK_PATH?=../wasi-sdk

test: test-alloc test-hello
run-fail: test-fail
build: alloc hello fail

%: %.c
	$(WASI_SDK_PATH)/bin/clang --target=wasm32-unknown-unknown -o $*.wasm $*.c -Wl,--no-entry -Wl,--export=run_test -nostdlib

test-%: %
	extism call $*.wasm run_test

clean:
	rm -f *.wasm

WASI_SDK_PATH?=../../wasi-sdk

.PHONY: allocations
allocations:
	$(WASI_SDK_PATH)/bin/clang -target wasm32-unknown-unknown -nostdlib -Wl,--no-entry -Wl,--export=allocations -O2 -g -o allocations.wasm allocations.c

run: allocations
	extism call ./allocations.wasm allocations --loop 100 --wasi

WASI_SDK_PATH?=../../wasi-sdk

.PHONY: infinite-loop
infinite-loop:
	$(WASI_SDK_PATH)/bin/clang -O2 -g -o infinite-loop.wasm infinite-loop.c -nostartfiles -Wl,--no-entry -Wl,--export=infinite_loop

run:
	extism call --manifest ./manifest.json infinite_loop --wasi

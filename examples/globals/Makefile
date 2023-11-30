WASI_SDK_PATH?=../../wasi-sdk

.PHONY: globals
globals:
	$(WASI_SDK_PATH)/bin/clang -O2 -g -o globals.wasm globals.c -mexec-model=reactor

run: globals
	extism call ./globals.wasm globals --loop 100 --wasi

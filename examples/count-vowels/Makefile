WASI_SDK_PATH?=../../wasi-sdk

.PHONY: count-vowels
count-vowels:
	$(WASI_SDK_PATH)/bin/clang -O2 -g -o count-vowels.wasm count-vowels.c -mexec-model=reactor

run: count-vowels
	extism call ./count-vowels.wasm count_vowels --wasi --input "this is a test"
WASICC?=wasicc

.PHONY: count-vowels
count-vowels:
	$(WASICC) -O2 -g -o count-vowels.wasm count-vowels.c -nostartfiles -Wl,--no-entry -Wl,--export=count_vowels

run:
	extism call ./count-vowels.wasm count_vowels --wasi --input "this is a test"
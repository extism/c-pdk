.PHONY: count-vowels
count-vowels:
	wasicc -g -o count-vowels.wasm count-vowels.c -Wl,--export=count_vowels

run:
	extism call ./count-vowels.wasm count_vowels --wasi --input "this is a test"
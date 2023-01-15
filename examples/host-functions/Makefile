WASICC?=wasicc

.PHONY: host-functions
host-functions:
	$(WASICC) -g -o host-functions.wasm host-functions.c -Wl,--export=count_vowels -Wl,--import-undefined

# run:
# 	extism call ./count-vowels.wasm count_vowels --wasi --input "this is a test"
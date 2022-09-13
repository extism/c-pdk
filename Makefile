.PHONY: example
example:
	emcc -o example.wasm example/count_vowels.c --no-entry -Wl,--export-all -sERROR_ON_UNDEFINED_SYMBOLS=0


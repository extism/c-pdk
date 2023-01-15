WASICC?=wasicc

build:
	$(WASICC) -o infinite-loop.wasm infinite-loop.c -Wl,--export=infinite_loop

run:
	extism call --manifest ./manifest.json infinite_loop --wasi

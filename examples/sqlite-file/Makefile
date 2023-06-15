WASI_SDK_PATH?=../../wasi-sdk

OBJS=sqlite3.o sqlite-file.o
CC=$(WASI_SDK_PATH)/bin/clang
CFLAGS=-O2

sqlite-file: $(OBJS)
	$(WASI_SDK_PATH)/bin/clang -O2 -g -o sqlite-file.wasm $(OBJS) -nostartfiles -Wl,--no-entry -Wl,--export=sqlite_file

run:
	mkdir -p tmp
	extism call ./sqlite-file.wasm sqlite_file --wasi --allow-path tmp:. --input "" --log-level info

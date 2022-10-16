# Your WASI SDK path
WASI_SDK=C:\Users\GuilleX7\Downloads\wasi-sdk-16.0-mingw.tar\wasi-sdk-16.0

# You probably don't need to change anything here
CC=$(WASI_SDK)/bin/clang
WASI_SYSROOT=$(WASI_SDK)/share/wasi-sysroot

# You definitely don't need to change anything here
CFLAGS=-O3 -Wall --target=wasm32-wasi
LDFLAGS=--sysroot=$(WASI_SYSROOT) -nostartfiles -fvisibility=hidden \
        -Wl,--no-entry -Wl,--export-dynamic -Wl,--export=malloc     \
        -Wl,--export=free -Wl,--import-undefined

DEPS=src/about.h src/as_parse.h src/cs_instructions.h src/cs_memory.h   \
     src/cs_op.h src/cs_registers.h src/cs2010.h src/hash_table.h       \
     src/parse.h src/trace_log.h src/utils.h src/wasm.h

OBJ=build/as_parse.o build/cs_instructions.o build/cs_op.o build/cs2010.o           \
    build/hash_table.o build/parse.o build/trace_log.o build/utils.o build/wasm.o

OUTPUT_LIB=build/asm2010.wasm

build/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUT_LIB): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

all: $(OUTPUT_LIB)

clean:
	rm build/*.o

.PHONY: clean

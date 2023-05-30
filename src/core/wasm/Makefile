# Note: This Makefile is kept for historical reasons only.
#       You should switch to the Meson build system

ifdef WASI_SDK
	CC=$(WASI_SDK)/bin/clang
	WASI_SYSROOT=$(WASI_SDK)/share/wasi-sysroot
endif

CFLAGS=-O3 -Wall --target=wasm32-wasi
LDFLAGS=--sysroot=$(WASI_SYSROOT) -nostartfiles -fvisibility=hidden \
        -Wl,--no-entry -Wl,--export-dynamic -Wl,--import-undefined

OUTPUT_LIB=bin/WASM2010_core.wasm

# WASM wrapper
build/wasm_glue.o: src/wasm_glue.c
	$(CC) -c -o $@ $(CFLAGS) $<

# ASM2010
build/trace_log.o: subprojects/ASM2010/src/trace_log.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/parse.o: subprojects/ASM2010/src/parse.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/hash_table.o: subprojects/ASM2010/src/hash_table.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs.o: subprojects/ASM2010/src/m2010/cs.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs_opcodes.o: subprojects/ASM2010/src/m2010/cs_opcodes.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs_io.o: subprojects/ASM2010/src/m2010/cs_io.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs_instructions.o: subprojects/ASM2010/src/m2010/cs_instructions.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs2010_platform.o: subprojects/ASM2010/src/m2010/cs2010/cs2010_platform.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs2010_opcodes.o: subprojects/ASM2010/src/m2010/cs2010/cs2010_opcodes.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs3_platform.o: subprojects/ASM2010/src/m2010/cs3/cs3_platform.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/m2010_cs3_opcodes.o: subprojects/ASM2010/src/m2010/cs3/cs3_opcodes.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/as_parse.o: subprojects/ASM2010/src/as_parse/as_parse.c
	$(CC) -c -o $@ $(CFLAGS) $<

build/as_disassemble.o: subprojects/ASM2010/src/as_parse/as_disassemble.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(OUTPUT_LIB): build/wasm_glue.o build/trace_log.o build/parse.o build/hash_table.o build/m2010_cs.o build/m2010_cs_opcodes.o \
			build/m2010_cs_io.o build/m2010_cs_instructions.o build/m2010_cs2010_platform.o build/m2010_cs2010_opcodes.o \
			build/m2010_cs3_platform.o build/m2010_cs3_opcodes.o build/as_parse.o build/as_disassemble.o
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

all: $(OUTPUT_LIB)

clean:
	rm build/*.o

.PHONY: clean

#include "../subprojects/ASM2010/include/asm2010.h"
#include <stdlib.h>

extern cs_io_read_fn  wasm_custom_io_read;
extern cs_io_write_fn wasm_custom_io_write;

ASM2010_API void wasm_set_custom_io_functions(struct cs_machine *cs) {
    cs_set_io_functions(cs, wasm_custom_io_read, wasm_custom_io_write);
}

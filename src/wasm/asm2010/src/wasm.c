#include "wasm.h"

void wasm_set_custom_io_functions(cs2010 *cs) {
    cs_set_io_functions(cs, wasm_custom_io_read, wasm_custom_io_write);
}

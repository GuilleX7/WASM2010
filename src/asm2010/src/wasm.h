#ifndef WASM_H
#define WASM_H

#include "cs2010.h"
#include "utils.h"
#include <stddef.h>

extern size_t wasm_custom_io_read(size_t address);
extern bool wasm_custom_io_write(size_t address, unsigned char content);

__attribute__((visibility("default"))) void
wasm_set_custom_io_functions(cs2010 *cs);

#endif

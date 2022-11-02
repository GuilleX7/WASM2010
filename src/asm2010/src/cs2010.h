/** @file cs2010.h */

#ifndef CS2010_H
#define CS2010_H

#include <stddef.h>

#include "utils.h"

#include "cs_memory.h"
#include "cs_registers.h"

#define CS_IO_READ_NOT_CONTROLLED -1
#define CS_IO_WRITE_NOT_CONTROLLED false
#define CS_IO_WRITE_CONTROLLED true

#define CS_CLEAR_RAM (1u << 0)
#define CS_CLEAR_ROM (1u << 1)

#define CS_SUCCESS 0
#define CS_FAILED 1
#define CS_NOT_ENOUGH_ROM 2
#define CS_INVALID_INSTRUCTIONS 3

typedef size_t (*io_read_fn_t)(size_t address);
typedef bool (*io_write_fn_t)(size_t address, unsigned char content);

struct cs2010 {
  cs_memory memory;
  cs_registers registers;
  unsigned char microop;
  bool stopped;
  io_read_fn_t io_read_fn;
  io_write_fn_t io_write_fn;
};
typedef struct cs2010 cs2010;

/**
 * @brief Creates a new CS2010 emulation instance
 * @return Pointer to a new CS2010 emulation instance
 */
__attribute__((visibility("default"))) cs2010 *cs_create();

/**
 * @brief Initialize a given CS2010 emulation instance
 * @param cs Pointer to the CS2010 emulation instance
 * @return true if success, false otherwise
 */
__attribute__((visibility("default"))) bool cs_init(cs2010 *cs);

/**
 * @brief Sets the emulation instance's I/O handlers
 * @param io_read_fn Function to read from I/O
 * @param io_write_fn Function to write to I/O
 */
__attribute__((visibility("default"))) void
cs_set_io_functions(cs2010 *cs, io_read_fn_t io_read_fn,
                    io_write_fn_t io_write_fn);

/**
 * @brief Performs a hard reset
 *      This includes clearing all the registers and
 *      memories
 * @param cs Pointer to the emulation instance
 * @param clear_rom Whether the ROM should be wiped out
 */
__attribute__((visibility("default"))) void cs_hard_reset(cs2010 *cs, bool clear_rom);

/**
 * @brief Performs a soft reset
 *      Doesn't clear memories, just resets PC and SP
 * @param cs Pointer to the emulation instance
 */
__attribute__((visibility("default"))) void cs_soft_reset(cs2010 *cs);

/**
 * @brief Clears the selected memories
 * @param cs Pointer to the emulation instance
 * @param flags Valid bitmasks are CS_CLEAR_RAM, CS_CLEAR_ROM
 */
void cs_clear_memory(cs2010 *cs, unsigned char flags);

/**
 * @brief Resets all registers and signals
 * @param cs Pointer to the emulation instance
 */
void cs_reset_registers(cs2010 *cs);

/**
 * @brief Loads and checks CS2010 machine code
 * @param cs Pointer to the emulation instance
 * @param sentences Pointer to the machine code
 * @param sentences_length Length of the machine code
 * @return CS_SUCCESS if success,
 *          CS_NOT_ENOUGH_ROM if machine code is too large,
 *          CS_INVALID_INSTRUCTIONS if machine code isn't valid CS2010
 *          machine code or
 *          CS_FAILED otherwise
 */
__attribute__((visibility("default"))) int
cs_load_program(cs2010 *cs, unsigned short *sentences,
                  size_t sentences_length);

/**
 * @brief Performs an increment of the microop counter
 *       and fetchs the matching flags
 * @param cs Pointer to the emulation instance
 */
void cs_microfetch(cs2010 *cs);

/**
 * @brief Performs a full instruction fetch, resetting
 *      the microop counter
 * @param cs Pointer to the emulation instance
 */
void cs_fetch(cs2010 *cs);

/**
 * @brief Performs a microstep, executing the current
 *      microoperation, and fetching the next instruction
 *      if needed
 * @param cs Pointer to the emulation instance
 */
__attribute__((visibility("default"))) void cs_microstep(cs2010 *cs);

/**
 * @brief Performs a fullstep, which means:
 *      - If the machine state is in the middle of an
 *        operation, it will finish the remaining
 *        microoperations
 *      - Otherwise, this is the same as calling cs_step
 * @param cs Pointer to the emulation instance
 */
__attribute__((visibility("default"))) void cs_fullstep(cs2010 *cs);

/**
 * @brief Performs a blockstep, which means executing
 *      instructions until a BRxx/JMP/CALL is found
 *      or the machine halts
 * @param cs Pointer to the emulation instance
 * @param max_instructions Maximum number of instructions to execute
 *      until the execution halts because no stopping condition
 *      is met
 * @return true if a stopping condition was met,
 *         false if the execution halted because of exhaustion
 */
__attribute__((visibility("default"))) bool
cs_blockstep(cs2010 *cs, size_t max_instructions);

/**
 * @brief Frees a given emulation instance
 * @param cs Pointer to the emulation instance
 */
void cs_free(cs2010 *cs);

#endif /* CS2010_H */

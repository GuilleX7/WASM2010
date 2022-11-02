/** @file as_parse.h */

#ifndef AS_PARSE_H
#define AS_PARSE_H

#include <stddef.h>

#include "cs_instructions.h"

#include "hash_table.h"
#include "trace_log.h"
#include "utils.h"

#define AS_MAX_TRACE_LENGTH 256
#define AS_MAX_LOG_LENGTH (AS_MAX_TRACE_LENGTH * 5)
#define AS_MAX_EQU_LENGTH 128

/* The longest line, so 16 bytes is enough
        1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
        O  O  O  O     0  x  F  F  ,     0  X  F  F /0 */
#define AS_MAX_DISASSEMBLY_LENGTH 16

#define AS_ARGUMENT_TYPE_INM 0
#define AS_ARGUMENT_TYPE_EQU 1
#define AS_ARGUMENT_TYPE_INVALID 2
#define AS_ARGUMENT_DEFINE_OF_TYPE(type)                                       \
  { {0}, type }

#define AS_PARSE_OK 0
#define AS_PARSE_WARNING 1
#define AS_PARSE_ERROR 2

struct as_parse_argument {
  union {
    size_t inm;
    char *equ_key;
  } value;
  int type;
};
typedef struct as_parse_argument as_parse_argument;

struct as_parse_sentence {
  as_parse_argument arg_a;
  as_parse_argument arg_b;
  cs_instruction const *instruction;
  size_t parsing_line_index;
};
typedef struct as_parse_sentence as_parse_sentence;

struct as_parse_assembled_code {
  size_t parsing_line_index;
  unsigned short machine_code;
};
typedef struct as_parse_assembled_code as_parse_assembled_code;

/**
        @brief Structure containing all the information needed to assemble
        a CS2010 program after parsing the assembly code
*/
struct as_parse_info {
  /** @brief Hash table containing all equ replacements */
  hash_table equs_ht;
  /** @brief Trace log for the assembly proccess */
  trace_log log;
  /** @brief Index of current parsing line */
  size_t parsing_line_index;
  /** @brief Maximum amount of sentences */
  size_t max_sentences;
  /** @brief Array containing all valid parsed sentences */
  as_parse_sentence *sentences;
  /** @brief Index of current sentence */
  size_t sentence_index;
  /** @brief Array containing assembled machine code */
  as_parse_assembled_code *assembled_code;
  /** @brief Index of current machine code */
  size_t assembled_code_length;
};
typedef struct as_parse_info as_parse_info;

/**
 * @brief Creates a new parsing struct
 * @return Pointer to a new parsing struct
 */
__attribute__((visibility("default"))) as_parse_info *as_parse_create();

/**
 * @brief Initializes a given parsing struct
 * @param parsing_info Pointer to as_parse_info struct to be initialized
 * @return 0 if success, non-zero value otherwise
 */
__attribute__((visibility("default"))) int
as_parse_init(as_parse_info *parsing_info, size_t max_sentences);

/**
 * @brief Parses CS2010 assembly code
 * @param parsing_info Pointer to the parse struct that keeps track of
 *				parsing proccess
 * @param line Pointer to the string containing the assembly source
 * @param stop_on_error Whether it should stop parsing if any
 *          error comes up
 * @return AS_PARSE_OK if source was parsed successfully,
 *			AS_PARSE_WARNING if any warning raised or
 *			AS_PARSE_ERROR if parsing was aborted
 */
__attribute__((visibility("default"))) int
as_parse_source(as_parse_info *parsing_info, char const *source,
                bool stop_on_error);

/**
 * @brief Parses one line of CS2010 assembly code
 *		This function must be called for every line of the assembly
 *		in order before calling parse_assemble
 * @param parsing_info Pointer to the parse struct that keeps track of
 *				parsing proccess
 * @param line Pointer to the string containing the assembly source
 * @return AS_PARSE_OK if line was parsed successfully,
 *			AS_PARSE_WARNING if a warning raised or
 *			AS_PARSE_ERROR if parsing should be aborted
 */
__attribute__((visibility("default"))) int
as_parse_line(as_parse_info *parsing_info, char const *line);

/**
 * @brief Assembles the parsing struct (it contains the previously
                parsed assembly lines) into binary machine code
 * @param parsing_info Pointer to as_parse_info struct that keeps tract of
 *		parsing proccess
* @param stop_on_error Whether it should stop assembling if any
 *          error comes up
 * @return AS_PARSE_OK if machine code was assembled successfully,
 *			AS_PARSE_ERROR otherwise
*/
__attribute__((visibility("default"))) int
as_parse_assemble(as_parse_info *parsing_info, bool stop_on_error);

/**
 * @brief Reads the log of a given parsing struct
 * @param parsing_info Pointer to as_parse_info struct that keeps tract of
 *		parsing proccess
 * @return Pointer to the log string
*/
__attribute__((visibility("default"))) char const *
as_parse_get_log(as_parse_info *parsing_info);

/**
 * @brief Disassembles a given raw, binary sentence of CS2010 machine
 *		code into human-readable CS2010 assembler. The returned string
 *		must be freed by the caller.
 * @param raw_sentence Binary CS2010 machine code sentence
 * @return Pointer to string containing the dissasembly if success,
 *		a null pointer otherwise (invalid binary code)
 */
__attribute__((visibility("default"))) char const *
as_disassemble_sentence(unsigned short raw_sentence);

/**
 * @brief Frees as_parse_info struct and associated memory
 * @param parsing_info Pointer to as_parse_info structure
 */
__attribute__((visibility("default"))) void
as_parse_free(as_parse_info *parsing_info);

#endif /* AS_PARSE_H */

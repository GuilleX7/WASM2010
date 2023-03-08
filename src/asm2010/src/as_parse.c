/** @file as_parse.c */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cs_memory.h"
#include "cs_registers.h"

#include "parse.h"
#include "utils.h"

#include "as_parse.h"

#define EQU_KEYWORD ".EQU "
#define EQU_KEYWORD_LENGTH strlen(EQU_KEYWORD)
#define EQU_LINE_MARK ':'
#define EQU_DEFINE_MARK '='
#define OPCODE_KEYWORD ".OPCODE "
#define OPCODE_KEYWORD_LENGTH strlen(OPCODE_KEYWORD)
#define ARG_SEP_MARK ','
#define COMMENT_MARK ';'
#define INDIRECT_REG_OP_MARK '('
#define INDIRECT_REG_END_MARK ')'
#define REG_MARK 'R'

/**
 * @brief Searchs for a line end or a comment mark.
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_END if line ends,
 *          PARSE_LINE_KEEP otherwise
 */
int search_line_end(char const **line_tracker) {
  if (skip_spaces(line_tracker) == PARSE_LINE_KEEP &&
      **line_tracker != COMMENT_MARK) {
    return PARSE_LINE_KEEP;
  }

  return PARSE_LINE_END;
}

/**
 * @brief Gets an immediate value with retrieve_value, and
 *		  packs it in a as_parse_argument
 * @param line_tracker Pointer to pointer to the line string
 * @param status Pointer to int holding
 *				the retrieval information
 * @param max_value Max value passed to retrieve_value
 * @return as_parse_argument containing an immediate value,
 *		or invalid value otherwise
 */
static as_parse_argument retrieve_inm(char const **line_tracker, int *status,
                                      size_t max_value) {
  as_parse_argument argument =
      AS_ARGUMENT_DEFINE_OF_TYPE(AS_ARGUMENT_TYPE_INVALID);
  argument.value.inm =
      retrieve_value(line_tracker, status, max_value, search_line_end);
  if (*status == RETRIEVE_VALUE_OK) {
    argument.type = AS_ARGUMENT_TYPE_INM;
  }

  return argument;
}

/**
 * @brief Gets either a valid equ identifier or an immediate value
 *		within given bounds
 * @param parsing_info Pointer to as_parse_info struct
 * @param line_tracker Pointer to pointer to line string
 * @param status Pointer to status holding retrieval status
 * @param max_length Maximum length for the equ identifier
 * @param max_value Maximum value for the immediate value
 * @return as_parse_argument containing an equ identifier,
 *		a valid immediate value or invalid value otherwise
 */
static as_parse_argument retrieve_inm_or_equ(as_parse_info *parsing_info,
                                             char const **line_tracker,
                                             int *status, size_t max_length,
                                             size_t max_value) {
  as_parse_argument argument =
      AS_ARGUMENT_DEFINE_OF_TYPE(AS_ARGUMENT_TYPE_INVALID);
  char *identifier =
      retrieve_alnum_identifier(line_tracker, max_length, search_line_end);

  if (identifier) {
    argument.type = AS_ARGUMENT_TYPE_EQU;
    argument.value.equ_key = identifier;
  } else {
    argument.value.inm =
        retrieve_value(line_tracker, status, max_value, search_line_end);
    if (*status == RETRIEVE_VALUE_OK) {
      argument.type = AS_ARGUMENT_TYPE_INM;
    }
  }

  return argument;
}

/**
 * @brief Search for user-defined equ
 * @param parsing_info Pointer to as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if no user-defined equ was found,
 *          PARSE_LINE_END if either and user-defined equ as found or not,
 *          but line ended,
 *          PARSE_LINE_WARNING if user-defined equ was found but ignored due
 *          to malforming or
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int search_defined_equ(as_parse_info *parsing_info,
                              char const **line_tracker) {
  char *identifier;
  int status = 0;
  size_t value = 0;

  if (strncmp(*line_tracker, EQU_KEYWORD, EQU_KEYWORD_LENGTH))
    return PARSE_LINE_KEEP;
  (*line_tracker) += EQU_KEYWORD_LENGTH;

  identifier = retrieve_alnum_identifier(line_tracker, AS_MAX_EQU_LENGTH,
                                         search_line_end);
  if (!identifier) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid identifier for equ at line %" PRI_SIZET
                     "\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  if (hash_table_exists(&parsing_info->equs_ht, identifier)) {
    trace_log_printf(&parsing_info->log,
                     "[Warning] identifier '%s' declared at line %" PRI_SIZET
                     " already exists, redeclaration "
                     "will be ignored\n",
                     identifier, parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_WARNING;
  }

  if (search_character(line_tracker, EQU_DEFINE_MARK, search_line_end) !=
      PARSE_LINE_KEEP) {
    trace_log_printf(&parsing_info->log,
                     "[Error] token '%c' not found for identifier '%s' or "
                     "value not specified at line %" PRI_SIZET "\n",
                     EQU_DEFINE_MARK, identifier,
                     parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_ERROR;
  }

  value = retrieve_value(line_tracker, &status, CS_INS_INM_MAX_VALUE,
                         search_line_end);
  switch (status) {
  case RETRIEVE_VALUE_INVALID:
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid value specified for equ identifier '%s' "
                     "at line %" PRI_SIZET "\n",
                     identifier, parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_ERROR;
  case RETRIEVE_VALUE_OUTOFBOUNDS:
    trace_log_printf(
        &parsing_info->log,
        "[Error] value specified for equ identifier '%s' at line %" PRI_SIZET
        " is out of bounds\n",
        identifier, parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_ERROR;
  default:
    break;
  }

  if (search_line_end(line_tracker) == PARSE_LINE_KEEP) {
    trace_log_printf(
        &parsing_info->log,
        ("[Error] expected end of line after equ definition at line %" PRI_SIZET
         "\n"),
        parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_ERROR;
  }

  hash_table_put(&parsing_info->equs_ht, identifier, &value);
  free(identifier);
  return PARSE_LINE_END;
}

/**
 * @brief Searchs for a .opcode pseudoinstruction in the given line, and
 *      transforms it to the corresponding machine code, adding it to the
 *      parse_info struct
 * @param parsing_info Pointer to the parse_info struct
 * @param line_tracker Pointer to pointer to the line
 * @return PARSE_LINE_KEEP if no opcode was found,
           PARSE_LINE_END if a correct opcode was found,
 *         PARSE_LINE_ERROR if parsing should be aborted
*/
static unsigned char search_opcode(as_parse_info *parsing_info,
                                   char const **line_tracker) {
  int status = 0;
  as_parse_argument msbyte = {0};
  as_parse_argument lsbyte = {0};
  unsigned short raw_sentence = 0;
  as_parse_sentence *sentence = {0};

  if (strncmp(*line_tracker, OPCODE_KEYWORD, OPCODE_KEYWORD_LENGTH))
    return PARSE_LINE_KEEP;
  (*line_tracker) += OPCODE_KEYWORD_LENGTH;

  msbyte = retrieve_inm(line_tracker, &status, CS_INS_INM_MAX_VALUE);
  switch (status) {
  case RETRIEVE_VALUE_INVALID:
    trace_log_printf(
        &parsing_info->log,
        "[Error] invalid value specified as MSB for opcode at line %" PRI_SIZET
        "\n",
        parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  case RETRIEVE_VALUE_OUTOFBOUNDS:
    trace_log_printf(&parsing_info->log,
                     "[Error] value specified as MSB for opcode is out of "
                     "bounds at line %" PRI_SIZET "\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  default:
    break;
  }

  if (search_character(line_tracker, ARG_SEP_MARK, search_line_end) ==
      PARSE_LINE_END) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid format for opcode at line %" PRI_SIZET
                     "\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  lsbyte = retrieve_inm_or_equ(parsing_info, line_tracker, &status,
                               AS_MAX_EQU_LENGTH, CS_INS_INM_MAX_VALUE);
  switch (status) {
  case RETRIEVE_VALUE_INVALID:
    trace_log_printf(
        &parsing_info->log,
        "[Error] invalid value specified as LSB for opcode at line %" PRI_SIZET
        "\n",
        parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  case RETRIEVE_VALUE_OUTOFBOUNDS:
    trace_log_printf(&parsing_info->log,
                     "[Error] value specified as LSB for opcode is out of "
                     "bounds at line %" PRI_SIZET "\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  default:
    break;
  }

  if (search_line_end(line_tracker) == PARSE_LINE_KEEP) {
    trace_log_printf(
        &parsing_info->log,
        "[Error] expected end of line after opcode at line %" PRI_SIZET "\n",
        parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  if (parsing_info->sentence_index + 1 >= parsing_info->max_sentences) {
    trace_log_printf(&parsing_info->log,
                     "[Error] maximum sentence lines reached (%" PRI_SIZET
                     ")\n",
                     parsing_info->max_sentences);
    return PARSE_LINE_ERROR;
  }

  raw_sentence = CS_GET_RAW_SENTENCE(lsbyte.value.inm, msbyte.value.inm);
  sentence = &parsing_info->sentences[parsing_info->sentence_index];
  sentence->instruction = cs_ins_get_from_sentence(raw_sentence);
  if (!sentence->instruction) {
    trace_log_printf(&parsing_info->log,
                     "[Error] unknown instruction with opcode '%#x' (MSB %#x) "
                     "at line %" PRI_SIZET "\n",
                     CS_GET_OPCODE(raw_sentence), msbyte.value.inm,
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }
  sentence->arg_a = msbyte;
  sentence->arg_a.value.inm = CS_GET_ARG_A(raw_sentence);
  sentence->arg_b = lsbyte;
  sentence->parsing_line_index = parsing_info->parsing_line_index;
  parsing_info->sentence_index++;
  return PARSE_LINE_END;
}

/**
 * @brief Search for line equ
 * @param parsing_info Pointer to as_parse_info struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if no line equ was found,
 *          PARSE_LINE_WARNING if line equ was found but ignored due
 *          to malforming or
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int search_line_equ(as_parse_info *parsing_info,
                           char const **line_tracker) {
  char *comment_mark_ptr = strchr(*line_tracker, COMMENT_MARK);
  char *equ_mark_ptr = strchr(*line_tracker, EQU_LINE_MARK);
  char *identifier = {0};

  if (!equ_mark_ptr ||
      (comment_mark_ptr && comment_mark_ptr - equ_mark_ptr < 0))
    return PARSE_LINE_KEEP;

  identifier = retrieve_alnum_identifier(line_tracker, AS_MAX_EQU_LENGTH,
                                         search_line_end);
  if (!identifier) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid equ identifier at line %" PRI_SIZET
                     " (perhaps not alphanumerical?)\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  if (search_character(line_tracker, EQU_LINE_MARK, search_line_end) ==
      PARSE_LINE_END) {
    trace_log_printf(&parsing_info->log,
                     "[Error] token '%c' not found at equ definition or no "
                     "instruction follows "
                     "the token at line %" PRI_SIZET "\n",
                     EQU_LINE_MARK, parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_ERROR;
  }

  if (hash_table_exists(&parsing_info->equs_ht, identifier)) {
    trace_log_printf(&parsing_info->log,
                     "[Warning] identifier '%s' declared at line %" PRI_SIZET
                     " already exists, redeclaration "
                     "will be ignored\n",
                     identifier, parsing_info->parsing_line_index);
    free(identifier);
    return PARSE_LINE_WARNING;
  }

  /* Everything's seems alright, but we can't guarantee that we are pointing to
     a valid sentence. Don't worry, instruction parsing stage will throw an
     error. */
  hash_table_put(&parsing_info->equs_ht, identifier,
                 &parsing_info->sentence_index);
  free(identifier);
  return PARSE_LINE_KEEP;
}

/**
 * @brief Search for a register argument
 * @param line_tracker Pointer to the pointer to the line string
 * @param allow_indirect_syntax Whether the indirect syntax is allowed or not
 * @return A valid parse_argument with ARGUMENT_TYPE_INM type if found,
 *          a parse argument with ARGUMENT_TYPE_INVALID type otherwise
 */
static as_parse_argument search_register(char const **line_tracker,
                                         bool allow_indirect_syntax) {
  as_parse_argument register_argument =
      AS_ARGUMENT_DEFINE_OF_TYPE(AS_ARGUMENT_TYPE_INVALID);
  size_t register_number = 0;
  int status = {0};
  bool indirect = false;

  if (skip_spaces(line_tracker) == PARSE_LINE_END) {
    return register_argument;
  }

  if (**line_tracker == INDIRECT_REG_OP_MARK) {
    if (allow_indirect_syntax) {
      indirect = true;
      (*line_tracker)++;
    } else {
      return register_argument;
    }
  }

  if (skip_spaces(line_tracker) == PARSE_LINE_END) {
    return register_argument;
  }

  if (**line_tracker == REG_MARK) {
    (*line_tracker)++;
    register_number =
        retrieve_value_decimal(line_tracker, &status, CS_REGISTERS_RMAX);
    if (status == RETRIEVE_VALUE_OK && register_number >= CS_REGISTERS_RMIN) {
      register_argument.type = AS_ARGUMENT_TYPE_INM;
      register_argument.value.inm = register_number;
    }
  }

  if ((indirect && **line_tracker != INDIRECT_REG_END_MARK) ||
      (!indirect && **line_tracker == INDIRECT_REG_END_MARK)) {
    register_argument.type = AS_ARGUMENT_TYPE_INVALID;
  } else if (**line_tracker == INDIRECT_REG_END_MARK) {
    (*line_tracker)++;
  }

  return register_argument;
}

/**
 * @brief Frees a given parse argument and its associated memory
 * @param arg_ptr Pointer to the parse argument to be freed
 */
static void free_argument(as_parse_argument *arg_ptr) {
  if (arg_ptr->type == AS_ARGUMENT_TYPE_EQU && arg_ptr->value.equ_key) {
    free(arg_ptr->value.equ_key);
  }
}

/**
 * @brief Parses A-format instruction
 * @param parsing_info Pointer to the as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if instruction was successfully parsed,
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int parse_instruction_fa(as_parse_info *parsing_info,
                                char const **line_tracker) {
  as_parse_sentence *sentence =
      &parsing_info->sentences[parsing_info->sentence_index];
  as_parse_argument *reg_dst = &sentence->arg_a;
  as_parse_argument *reg_src = &sentence->arg_b;

  if (sentence->instruction->index == CS_INS_I_ST) {
    reg_dst = &sentence->arg_b;
    reg_src = &sentence->arg_a;
  }

  *reg_dst = search_register(
      line_tracker,
      (sentence->instruction->index == CS_INS_I_ST) ? true : false);
  if (reg_dst->type == AS_ARGUMENT_TYPE_INVALID) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid register as parameter 1 of instruction "
                     "'%s' at line %" PRI_SIZET "\n",
                     sentence->instruction->name,
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  if (search_character(line_tracker, ARG_SEP_MARK, search_line_end) !=
      PARSE_LINE_KEEP) {
    trace_log_printf(
        &parsing_info->log,
        "[Error] invalid format for instruction '%s' at line %" PRI_SIZET "\n",
        sentence->instruction->name, parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  *reg_src = search_register(
      line_tracker,
      (sentence->instruction->index == CS_INS_I_LD) ? true : false);
  if (reg_src->type == AS_ARGUMENT_TYPE_INVALID) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid register as parameter 2 of instruction "
                     "'%s' at line %" PRI_SIZET "\n",
                     sentence->instruction->name,
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  return PARSE_LINE_KEEP;
}

/**
 * @brief Parses B-format instruction
 * @param parsing_info Pointer to the as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if instruction was successfully parsed,
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int parse_instruction_fb(as_parse_info *parsing_info,
                                char const **line_tracker) {
  as_parse_sentence *sentence =
      &parsing_info->sentences[parsing_info->sentence_index];
  as_parse_argument *reg_dst = &sentence->arg_a;
  as_parse_argument *inm_address = &sentence->arg_b;

  if (sentence->instruction->index == CS_INS_I_STS) {
    int status = {0};
    *inm_address = retrieve_inm_or_equ(parsing_info, line_tracker, &status,
                                       AS_MAX_EQU_LENGTH, CS_INS_INM_MAX_VALUE);
    if (inm_address->type == AS_ARGUMENT_TYPE_INVALID) {
      trace_log_printf(&parsing_info->log,
                       "[Error] invalid value/address as parameter 1 of "
                       "instruction '%s' at line %" PRI_SIZET "\n",
                       sentence->instruction->name,
                       parsing_info->parsing_line_index);
      return PARSE_LINE_ERROR;
    }
  } else {
    *reg_dst = search_register(line_tracker, false);
    if (reg_dst->type == AS_ARGUMENT_TYPE_INVALID) {
      trace_log_printf(&parsing_info->log,
                       "[Error] invalid register as parameter 1 of instruction "
                       "'%s' at line %" PRI_SIZET "\n",
                       sentence->instruction->name,
                       parsing_info->parsing_line_index);
      return PARSE_LINE_ERROR;
    }
  }

  if (search_character(line_tracker, ARG_SEP_MARK, search_line_end) !=
      PARSE_LINE_KEEP) {
    trace_log_printf(
        &parsing_info->log,
        "[Error] invalid format for instruction '%s' at line %" PRI_SIZET "\n",
        sentence->instruction->name, parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  if (sentence->instruction->index == CS_INS_I_STS) {
    *reg_dst = search_register(line_tracker, false);
    if (reg_dst->type == AS_ARGUMENT_TYPE_INVALID) {
      trace_log_printf(&parsing_info->log,
                       "[Error] invalid register as parameter 2 of instruction "
                       "'%s' at line %" PRI_SIZET "\n",
                       sentence->instruction->name,
                       parsing_info->parsing_line_index);
      return PARSE_LINE_ERROR;
    }
  } else {
    int status = {0};
    *inm_address = retrieve_inm_or_equ(parsing_info, line_tracker, &status,
                                       AS_MAX_EQU_LENGTH, CS_INS_INM_MAX_VALUE);
    if (inm_address->type == AS_ARGUMENT_TYPE_INVALID) {
      trace_log_printf(&parsing_info->log,
                       "[Error] invalid value/address as parameter 2 of "
                       "instruction '%s' at line %" PRI_SIZET "\n",
                       sentence->instruction->name,
                       parsing_info->parsing_line_index);
      return PARSE_LINE_ERROR;
    }
  }

  return PARSE_LINE_KEEP;
}

/**
 * @brief Parses C-format instruction
 * @param parsing_info Pointer to the as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if instruction was successfully parsed,
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int parse_instruction_fc(as_parse_info *parsing_info,
                                char const **line_tracker) {
  as_parse_sentence *sentence =
      &parsing_info->sentences[parsing_info->sentence_index];
  as_parse_argument *jump_condition = &sentence->arg_a;
  as_parse_argument *inm_address = &sentence->arg_b;
  int status = {0};

  jump_condition->type = AS_ARGUMENT_TYPE_INM;
  jump_condition->value.inm = cs_ins_get_jmp_condition(sentence->instruction);

  *inm_address = retrieve_inm_or_equ(parsing_info, line_tracker, &status,
                                     AS_MAX_EQU_LENGTH, CS_INS_INM_MAX_VALUE);
  if (inm_address->type == AS_ARGUMENT_TYPE_INVALID) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid address as parameter 1 of instruction "
                     "'%s' at line %" PRI_SIZET "\n",
                     sentence->instruction->name,
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  return PARSE_LINE_KEEP;
}

/**
 * @brief Parses D-format instruction
 * @param parsing_info Pointer to the as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if instruction was successfully parsed,
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int parse_instruction_fd(as_parse_info *parsing_info,
                                char const **line_tracker) {
  as_parse_sentence *sentence =
      &parsing_info->sentences[parsing_info->sentence_index];
  as_parse_argument *reg_dst = &sentence->arg_a;
  sentence->arg_b.type = AS_ARGUMENT_TYPE_INM;
  sentence->arg_b.value.inm = 0;

  *reg_dst = search_register(line_tracker, false);
  if (reg_dst->type == AS_ARGUMENT_TYPE_INVALID) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid register as parameter 1 of instruction "
                     "'%s' at line %" PRI_SIZET "\n",
                     sentence->instruction->name,
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  return PARSE_LINE_KEEP;
}

/**
 * @brief Parses E-format instruction
 * @param parsing_info Pointer to the as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if instruction was successfully parsed,
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int parse_instruction_fe(as_parse_info *parsing_info) {
  as_parse_sentence *sentence =
      &parsing_info->sentences[parsing_info->sentence_index];
  sentence->arg_a.type = AS_ARGUMENT_TYPE_INM;
  sentence->arg_a.value.inm = 0;
  sentence->arg_b.type = AS_ARGUMENT_TYPE_INM;
  sentence->arg_b.value.inm = 0;

  return PARSE_LINE_KEEP;
}

/**
 * @brief Search and parses any instruction
 * @param parsing_info Pointer to the as_parse_info_struct
 * @param line_tracker Pointer to pointer to the line string
 * @return PARSE_LINE_KEEP if instruction was successfully parsed,
 *          PARSE_LINE_ERROR if parsing should be aborted
 */
static int search_instruction(as_parse_info *parsing_info,
                              char const **line_tracker) {
  char *instruction_name = retrieve_alnum_identifier(
      line_tracker, CS_INS_NAME_MAX_LENGTH, search_line_end);
  as_parse_sentence *sentence = {0};
  unsigned char result = {0};

  if (!instruction_name) {
    trace_log_printf(&parsing_info->log,
                     "[Error] invalid instruction at line %" PRI_SIZET "\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  sentence = &parsing_info->sentences[parsing_info->sentence_index];
  sentence->instruction = cs_ins_search_by_name(instruction_name);
  if (!sentence->instruction) {
    trace_log_printf(&parsing_info->log,
                     "[Error] unknown instruction '%s' at line %" PRI_SIZET
                     "\n",
                     instruction_name, parsing_info->parsing_line_index);
    free(instruction_name);
    return PARSE_LINE_ERROR;
  }
  free(instruction_name);
  sentence->parsing_line_index = parsing_info->parsing_line_index;

  switch (sentence->instruction->format) {
  case CS_INS_FORMAT_A:
    result = parse_instruction_fa(parsing_info, line_tracker);
    break;
  case CS_INS_FORMAT_B:
    result = parse_instruction_fb(parsing_info, line_tracker);
    break;
  case CS_INS_FORMAT_C:
    result = parse_instruction_fc(parsing_info, line_tracker);
    break;
  case CS_INS_FORMAT_D:
    result = parse_instruction_fd(parsing_info, line_tracker);
    break;
  case CS_INS_FORMAT_E:
    result = parse_instruction_fe(parsing_info);
    break;
  default:
    trace_log_printf(
        &parsing_info->log,
        "[Error] unknown instruction type for '%s' at line %" PRI_SIZET "\n",
        sentence->instruction->name, parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  switch (result) {
  case PARSE_LINE_ERROR:
    return PARSE_LINE_ERROR;
  default:
    break;
  }

  if (search_line_end(line_tracker) == PARSE_LINE_KEEP) {
    trace_log_printf(&parsing_info->log,
                     "[Error] expected end of the line after instruction "
                     "arguments at line %" PRI_SIZET "\n",
                     parsing_info->parsing_line_index);
    return PARSE_LINE_ERROR;
  }

  if (parsing_info->sentence_index >= parsing_info->max_sentences) {
    trace_log_printf(&parsing_info->log,
                     "[Error] maximum sentence lines reached (%" PRI_SIZET
                     ")\n",
                     parsing_info->max_sentences);
    return PARSE_LINE_ERROR;
  }
  parsing_info->sentence_index++;
  return PARSE_LINE_END;
}

as_parse_info *as_parse_create() { return malloc(sizeof(as_parse_info)); }

int as_parse_init(as_parse_info *parsing_info, size_t max_sentences) {
  if (!hash_table_init(&parsing_info->equs_ht)) {
    return -1;
  }

  if (!trace_log_init(&parsing_info->log, AS_MAX_TRACE_LENGTH,
                      AS_MAX_LOG_LENGTH)) {
    hash_table_free(&parsing_info->equs_ht);
    return -2;
  }

  if (!cs_ins_search_start()) {
    hash_table_free(&parsing_info->equs_ht);
    trace_log_free(&parsing_info->log);
    return -3;
  }

  parsing_info->sentences =
      malloc(sizeof *parsing_info->sentences * max_sentences);
  if (!parsing_info->sentences) {
    hash_table_free(&parsing_info->equs_ht);
    trace_log_free(&parsing_info->log);
    cs_ins_search_stop();
    return -4;
  }

  parsing_info->assembled_code =
      malloc(sizeof *parsing_info->assembled_code * max_sentences);
  if (!parsing_info->assembled_code) {
    hash_table_free(&parsing_info->equs_ht);
    trace_log_free(&parsing_info->log);
    cs_ins_search_stop();
    free(parsing_info->sentences);
    return -5;
  }

  parsing_info->parsing_line_index = 0;
  parsing_info->sentence_index = 0;
  parsing_info->assembled_code_length = 0;
  parsing_info->max_sentences = max_sentences;
  return 0;
}

void as_parse_free(as_parse_info *parsing_info) {
  size_t i = 0;
  hash_table_free(&parsing_info->equs_ht);
  for (; i < parsing_info->sentence_index; i++) {
    free_argument(&parsing_info->sentences[i].arg_a);
    free_argument(&parsing_info->sentences[i].arg_b);
  }
  trace_log_free(&parsing_info->log);
  free(parsing_info->sentences);
  free(parsing_info->assembled_code);
}

int as_parse_source(as_parse_info *parsing_info, char const *source,
                    bool stop_on_error) {
  char line[1024];
  size_t offset = 0;
  int status = AS_PARSE_OK;
  int next_status = AS_PARSE_OK;

  while (read_upper_line(line, 1024, source, &offset)) {
    next_status = as_parse_line(parsing_info, line);
    if (next_status > status) {
      status = next_status;
    }

    if (stop_on_error && status == AS_PARSE_ERROR) {
      break;
    }
  }

  return status;
}

int as_parse_line(as_parse_info *parsing_info, char const *line) {
  char const **line_tracker = &line;
  parsing_info->parsing_line_index++;

  /* Skip commentaries */
  if (search_line_end(line_tracker)) {
    return AS_PARSE_OK;
  }

  /* Search user-defined equs */
  switch (search_defined_equ(parsing_info, line_tracker)) {
  case PARSE_LINE_END:
    return AS_PARSE_OK;
  case PARSE_LINE_ERROR:
    return AS_PARSE_ERROR;
  case PARSE_LINE_WARNING:
    return AS_PARSE_WARNING;
  default:
    break;
  }

  /* Search opcodes */
  switch (search_opcode(parsing_info, line_tracker)) {
  case PARSE_LINE_END:
    return AS_PARSE_OK;
  case PARSE_LINE_ERROR:
    return AS_PARSE_ERROR;
  default:
    break;
  }

  /* Search line equs */
  switch (search_line_equ(parsing_info, line_tracker)) {
  case PARSE_LINE_ERROR:
    return AS_PARSE_ERROR;
  default:
    break;
  }

  /* Search instruction */
  switch (search_instruction(parsing_info, line_tracker)) {
  case PARSE_LINE_END:
    return AS_PARSE_OK;
  case PARSE_LINE_ERROR:
    return AS_PARSE_ERROR;
  default:
    break;
  }

  return AS_PARSE_OK;
}

int as_parse_assemble(as_parse_info *parsing_info, bool stop_on_error) {
  size_t i = 0;
  as_parse_sentence *sentence = {0};
  as_parse_assembled_code *assembled_code = {0};
  unsigned short machine_code = 0;
  size_t *value_ptr = {0};
  size_t value = 0;

  for (; i < parsing_info->sentence_index; i++) {
    sentence = &parsing_info->sentences[i];
    assembled_code =
        &parsing_info->assembled_code[parsing_info->assembled_code_length];

    machine_code = ((unsigned short)sentence->instruction->opcode << 11);

    if (sentence->arg_a.type == AS_ARGUMENT_TYPE_EQU) {
      value_ptr =
          hash_table_get(&parsing_info->equs_ht, sentence->arg_a.value.equ_key);
      if (!value_ptr) {
        trace_log_printf(
            &parsing_info->log,
            "[Error] equ '%s' of instruction '%s' at line %" PRI_SIZET
            " couldn't be resolved\n",
            sentence->arg_a.value.equ_key, sentence->instruction->name,
            sentence->parsing_line_index);
        if (stop_on_error) {
          return AS_PARSE_ERROR;
        } else {
          continue;
        }
      } else {
        value = *value_ptr;
      }
    } else {
      value = sentence->arg_a.value.inm;
    }
    machine_code |= ((unsigned short)value & 0x1Fu) << 8;

    if (sentence->arg_b.type == AS_ARGUMENT_TYPE_EQU) {
      value_ptr =
          hash_table_get(&parsing_info->equs_ht, sentence->arg_b.value.equ_key);
      if (!value_ptr) {
        trace_log_printf(
            &parsing_info->log,
            "[Error] equ '%s' of instruction '%s' at line %" PRI_SIZET
            " couldn't be resolved\n",
            sentence->arg_b.value.equ_key, sentence->instruction->name,
            sentence->parsing_line_index);
        if (stop_on_error) {
          return AS_PARSE_ERROR;
        } else {
          continue;
        }
      } else {
        value = *value_ptr;
      }
    } else {
      value = sentence->arg_b.value.inm;
    }
    machine_code |= (unsigned short)value & 0xFFu;

    assembled_code->machine_code = machine_code;
    assembled_code->parsing_line_index = sentence->parsing_line_index;
    parsing_info->assembled_code_length++;
  }

  return AS_PARSE_OK;
}

char const *as_parse_get_log(as_parse_info *parsing_info) {
  return trace_log_get(&parsing_info->log);
}

char const *as_disassemble_sentence(unsigned short raw_sentence) {
  cs_instruction const *instruction = cs_ins_get_from_sentence(raw_sentence);
  unsigned char arg_a = CS_GET_ARG_A(raw_sentence);
  unsigned char arg_b = CS_GET_ARG_B(raw_sentence);
  char *disassembly = 0;
  bool valid = true;
  if (!instruction->name) {
    return 0;
  }
  disassembly = malloc(sizeof(char) * AS_MAX_DISASSEMBLY_LENGTH);
  if (!disassembly) {
    return 0;
  }

  switch (instruction->format) {
  case CS_INS_FORMAT_A:
    if (arg_b < CS_REGISTERS_RMIN || arg_b > CS_REGISTERS_RMAX) {
      valid = false;
      break;
    }
    if (instruction->index == CS_INS_I_LD) {
      snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s R%u, (R%u)",
               instruction->name, arg_a, arg_b);
    } else if (instruction->index == CS_INS_I_ST) {
      snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s (R%u), R%u",
               instruction->name, arg_b, arg_a);
    } else {
      snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s R%u, R%u",
               instruction->name, arg_a, arg_b);
    }
    break;
  case CS_INS_FORMAT_B:
    if (instruction->index == CS_INS_I_STS) {
      snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH,
               "%s " HEX8_X_FORMAT ", R%u", instruction->name, arg_b, arg_a);
    } else {
      snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s R%u, " HEX8_X_FORMAT,
               instruction->name, arg_a, arg_b);
    }
    break;
  case CS_INS_FORMAT_C:
    snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s " HEX8_X_FORMAT,
             instruction->name, arg_b);
    break;
  case CS_INS_FORMAT_D:
    snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s R%u",
             instruction->name, arg_a);
    break;
  case CS_INS_FORMAT_E:
    snprintf(disassembly, AS_MAX_DISASSEMBLY_LENGTH, "%s", instruction->name);
    break;
  default:
    valid = false;
    break;
  }

  if (!valid) {
    free(disassembly);
    disassembly = 0;
  }
  return disassembly;
}

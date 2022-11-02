/** @file utils.c */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include "utils.h"
#include <ctype.h>

bool read_upper_line(char *line, size_t max_length, char const *const str,
                     size_t *offset) {
  size_t i = 0;
  while (str[*offset] != '\0' && str[*offset] != '\n' && i < max_length - 1) {
    line[i] = toupper((unsigned)str[*offset]);
    (*offset)++;
    i++;
  }
  line[i] = '\0';

  for (; str[*offset] != '\0' && str[*offset] != '\n'; (*offset)++);

  if (str[*offset] == '\n') {
    (*offset)++;
    return true;
  } else {
    return false;
  }
}

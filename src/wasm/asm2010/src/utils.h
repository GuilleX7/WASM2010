/** @file utils.h */

#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

#ifndef bool
#define bool unsigned char
#define true 1
#define false 0
#endif

#define STRGIFY(a) #a
#define STRINGIFY(a) STRGIFY(a)

#ifdef _WIN32
#ifdef _WIN64
#define PRI_SIZET PRIu64
#else
#define PRI_SIZET PRIu32
#endif /* _WIN64 */
#else
#define PRI_SIZET "zu"
#endif /* _WIN32 */

#define BOOLEAN_FORMAT "%u"
#define HEX4_FORMAT "%01X"
#define HEX8_FORMAT "%02X"
#define HEX16_FORMAT "%04X"
#define HEX32_FORMAT "%08X"
#define HEX64_FORMAT "%016X"
#define HEX4_X_FORMAT "0x%01X"
#define HEX8_X_FORMAT "0x%02X"
#define HEX16_X_FORMAT "0x%04X"
#define HEX32_X_FORMAT "0x%08X"
#define HEX64_X_FORMAT "0x%016X"

/**
 * @brief Reads a line of a given string in uppercase
 * @param line Pointer to the buffer used to store the line
 * @param max_length Maximum length of the line to be buffered
 * @param str Pointer to the string to be readed from
 * @param offset Should be a pointer to 0-integer first time this
 *              function is called. Successive calls needs the
 *              same integer object to be passed in order to keep
 *              track of the position
 * @return false if there are no more lines left,
 *         true otherwise
 */
__attribute__((visibility("default"))) bool
read_upper_line(char *line, size_t max_length, char const *const str,
                size_t *offset);

#endif /* UTILS_H */

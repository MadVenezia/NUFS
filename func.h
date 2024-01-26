#ifndef FUNC_H
#define FUNC_H

#include <string.h>

/**
 * @brief Compares two strings for equality.
 *
 * @param aa The first string.
 * @param bb The second string.
 * @return Returns 0 if the strings are equal, a non-zero value otherwise.
 */
static int streq(const char* aa, const char* bb)
{
  return strcmp(aa, bb) == 0;
}

#endif

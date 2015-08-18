/*
 * str_util.cc
 *
 */

#include "util/str_util.h"
#include <errno.h>
#include <cstring>
#include <cstdlib>

namespace kit {

int str_to_long(const std::string &s, int base, long* result) {
  char cstr[s.length() + 1];
  strcpy(cstr, s.c_str());
  char *eptr = NULL;
  *result = strtol(cstr, &eptr, base);
  if (isspace(cstr[0]) || eptr[0] != '\0' || errno == ERANGE) {
    return -1;
  }
  return 0;
}

void trim(char *s) {
  if (s == NULL || s[0] == '\0') {
    return;
  }
  char* begin = s;
  while (isspace((unsigned char) *begin)) {
    ++begin;
  }
  char* end = begin + strlen(begin) - 1;
  while (end > begin && isspace((unsigned char) *end)) {
    *end-- = '\0';
  }
  if (begin != s) {
    strcpy(s, begin);
  }
}

std::vector<std::string> str_split(const char* s, const char* delimiters) {
  std::vector<std::string> result;
  if (s == NULL || s[0] == '\0') {
    return result;
  }
  while (*s) {
    while (*s && strchr(delimiters, *s) != NULL) {
      ++s;
    }
    if (*s == '\0') {
      break;
    }
    const char* begin = s;
    while (*s && strchr(delimiters, *s) == NULL) {
      ++s;
    }
    result.push_back(std::string(begin, s - begin));
  }
  return result;
}

}


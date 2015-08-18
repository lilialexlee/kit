/*
 * str_util.h
 *
 */

#include <string>
#include <vector>

#ifndef SRC_UTIL_STR_UTIL_H_
#define SRC_UTIL_STR_UTIL_H_

namespace kit {

int str_to_long(const std::string &s, int base, long* result);

void trim(char* s);

std::vector<std::string> str_split(const char* s, const char* delimiters);

}

#endif /* SRC_UTIL_STR_UTIL_H_ */

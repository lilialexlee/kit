/*
 * header.h
 *
 */

#ifndef EXAMPLE_HTTP_HEADER_H_
#define EXAMPLE_HTTP_HEADER_H_

#include <string>

namespace http {

struct Header {
  std::string name;
  std::string value;
};

}

#endif /* EXAMPLE_HTTP_HEADER_H_ */

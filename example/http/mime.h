/*
 * mime.h
 *
 */

#ifndef EXAMPLE_HTTP_MIME_H_
#define EXAMPLE_HTTP_MIME_H_

#include <string>

namespace http {

/// Convert a file extension into a MIME type.
std::string ExtensionToType(const std::string& extension);

}

#endif /* EXAMPLE_HTTP_MIME_H_ */

/*
 * mime.cc
 *
 */

#include "example/http/mime.h"

namespace http {

namespace {

struct mapping {
  const char* extension;
  const char* mime_type;
};

static const struct mapping kMappings[] = { { "gif", "image/gif" }, { "htm",
    "text/html" }, { "html", "text/html" }, { "jpg", "image/jpeg" }, { "png",
    "image/png" }, { 0, 0 }  // Marks end of list.
};

}

std::string ExtensionToType(const std::string& extension) {
  for (const mapping* m = kMappings; m->extension; ++m) {
    if (m->extension == extension) {
      return m->mime_type;
    }
  }
  return "text/plain";
}
}


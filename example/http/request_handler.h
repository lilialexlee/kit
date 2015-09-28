/*
 * request_handler.h
 *
 */

#ifndef EXAMPLE_HTTP_REQUEST_HANDLER_H_
#define EXAMPLE_HTTP_REQUEST_HANDLER_H_

#include <string>
#include "example/http/request.h"
#include "example/http/reply.h"

namespace http {

class RequestHandler {
 public:
  /// Construct with a directory containing files to be served.
  explicit RequestHandler(const std::string& doc_root);

  ~RequestHandler();

  /// Handle a request and produce a reply.
  ReplyPtr Handle(const RequestPtr& request);

 private:
  /// The directory containing the files to be served.
  std::string doc_root_;
};

}

#endif /* EXAMPLE_HTTP_REQUEST_HANDLER_H_ */

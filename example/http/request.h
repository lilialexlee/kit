/*
 * request.h
 *
 */

#ifndef EXAMPLE_HTTP_REQUEST_H_
#define EXAMPLE_HTTP_REQUEST_H_

#include <string>
#include <vector>
#include "net/message.h"
#include "example/http/header.h"

namespace http {

enum DecodeState {
  kMethodStart,
  kMethod,
  kUri,
  kHttpVersionH,
  kHttpVersionT1,
  kHttpVersionT2,
  kHttpVersionP,
  kHttpVersionSlash,
  kHttpVersionMajorStart,
  kHttpVersionMajor,
  kHttpVersionMinorStart,
  kHttpVersionMinor,
  kExpectingNewLine1,
  kHeaderLineStart,
  kHeaderLws,
  kHeaderName,
  kSpaceBeforeHeaderValue,
  kHeaderValue,
  kExpectingNewline2,
  kExpectingNewline3
};

/// A request received from a client.
class Request : public kit::Message {
 public:
  Request();
  ~Request();

  int Decode(kit::Buffer *input);
  void Encode(kit::Buffer *output);

 private:
  friend class RequestHandler;

  int Consume(char c);

  /// The request method, e.g. "GET", "POST".
  std::string method_;
  /// The requested URI, such as a path to a file.
  std::string uri_;
  /// Major version number, usually 1.
  int http_version_major_;
  /// Minor version number, usually 0 or 1.
  int http_version_minor_;
  /// The headers included with the request.
  std::vector<Header> headers_;

  DecodeState decode_stat_;
};

typedef boost::shared_ptr<Request> RequestPtr;

}

#endif /* EXAMPLE_HTTP_REQUEST_H_ */

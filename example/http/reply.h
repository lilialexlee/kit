/*
 * reply.h
 *
 */

#ifndef EXAMPLE_HTTP_REPLY_H_
#define EXAMPLE_HTTP_REPLY_H_

#include <vector>
#include "net/message.h"
#include "example/http/header.h"

namespace http {

enum StatusType {
  kOk = 200,
  kCreated = 201,
  kAccepted = 202,
  kNoContent = 204,
  kMultipleChoices = 300,
  kMovedPermanently = 301,
  kMovedTemporarily = 302,
  kNotModified = 304,
  kBadRequest = 400,
  kUnauthorized = 401,
  kForbidden = 403,
  kNotFound = 404,
  kInternalServerError = 500,
  kNotImplemented = 501,
  kBadGateway = 502,
  kServiceUnavailable = 503
};

class Reply;

typedef boost::shared_ptr<Reply> ReplyPtr;

class Reply : public kit::Message {
 public:
  Reply();
  ~Reply();

  int Decode(kit::Buffer *input);
  void Encode(kit::Buffer *output);

  static ReplyPtr StockReply(StatusType status);

 private:
  friend class RequestHandler;

  /// The status of the reply.
  StatusType status_;

  /// The headers to be included in the reply.
  std::vector<Header> headers_;

  /// The content to be sent in the reply.
  std::string content_;
};

}

#endif /* EXAMPLE_HTTP_REPLY_H_ */

/*
 * reply.cc
 *
 */

#include "example/http/reply.h"
#include <sstream>

namespace http {

Reply::Reply()
    : status_(),
      headers_(),
      content_() {
}

Reply::~Reply() {
}

namespace {

static const std::string kOkContent = "";
static const std::string kCreatedContent = "<html>"
    "<head><title>Created</title></head>"
    "<body><h1>201 Created</h1></body>"
    "</html>";
static const std::string kAcceptedContent = "<html>"
    "<head><title>Accepted</title></head>"
    "<body><h1>202 Accepted</h1></body>"
    "</html>";
static const std::string kNoContentContent = "<html>"
    "<head><title>No Content</title></head>"
    "<body><h1>204 Content</h1></body>"
    "</html>";
static const std::string kMultipleChoicesContent = "<html>"
    "<head><title>Multiple Choices</title></head>"
    "<body><h1>300 Multiple Choices</h1></body>"
    "</html>";
static const std::string kMovedPermanentlyContent = "<html>"
    "<head><title>Moved Permanently</title></head>"
    "<body><h1>301 Moved Permanently</h1></body>"
    "</html>";
static const std::string kMovedTemporarilyContent = "<html>"
    "<head><title>Moved Temporarily</title></head>"
    "<body><h1>302 Moved Temporarily</h1></body>"
    "</html>";
static const std::string kNotModifiedContent = "<html>"
    "<head><title>Not Modified</title></head>"
    "<body><h1>304 Not Modified</h1></body>"
    "</html>";
static const std::string kBadRequestContent = "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>";
static const std::string kUnauthorizedContent = "<html>"
    "<head><title>Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1></body>"
    "</html>";
static const std::string kForbiddenContent = "<html>"
    "<head><title>Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1></body>"
    "</html>";
static const std::string kNotFoundContent = "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>";
static const std::string kInternalServerErrorContent = "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";
static const std::string kNotImplementedContent = "<html>"
    "<head><title>Not Implemented</title></head>"
    "<body><h1>501 Not Implemented</h1></body>"
    "</html>";
static const std::string kBadGatewayContent = "<html>"
    "<head><title>Bad Gateway</title></head>"
    "<body><h1>502 Bad Gateway</h1></body>"
    "</html>";
static const std::string kServiceUnavailableContent = "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>";

const std::string& ToContent(StatusType status) {
  switch (status) {
    case kOk:
      return kOkContent;
    case kCreated:
      return kCreatedContent;
    case kAccepted:
      return kAcceptedContent;
    case kNoContent:
      return kNoContentContent;
    case kMultipleChoices:
      return kMultipleChoicesContent;
    case kMovedPermanently:
      return kMovedPermanentlyContent;
    case kMovedTemporarily:
      return kMovedTemporarilyContent;
    case kNotModified:
      return kNotModifiedContent;
    case kBadRequest:
      return kBadRequestContent;
    case kUnauthorized:
      return kUnauthorizedContent;
    case kForbidden:
      return kForbiddenContent;
    case kNotFound:
      return kNotFoundContent;
    case kInternalServerError:
      return kInternalServerErrorContent;
    case kNotImplemented:
      return kNotImplementedContent;
    case kBadGateway:
      return kBadGatewayContent;
    case kServiceUnavailable:
      return kServiceUnavailableContent;
    default:
      return kInternalServerErrorContent;
  }
}

static const std::string kOkStatusLine = "HTTP/1.0 200 OK\r\n";
static const std::string kCreatedStatusLine = "HTTP/1.0 201 Created\r\n";
static const std::string kAcceptedStatusLine = "HTTP/1.0 202 Accepted\r\n";
static const std::string kNoContentStatusLine = "HTTP/1.0 204 No Content\r\n";
static const std::string kMultipleChoicesStatusLine =
    "HTTP/1.0 300 Multiple Choices\r\n";
static const std::string kMovedPermanentlyStatusLine =
    "HTTP/1.0 301 Moved Permanently\r\n";
static const std::string kMovedTemporarilyStatusLine =
    "HTTP/1.0 302 Moved Temporarily\r\n";
static const std::string kNotModifiedStatusLine =
    "HTTP/1.0 304 Not Modified\r\n";
static const std::string kBadRequestStatusLine = "HTTP/1.0 400 Bad Request\r\n";
static const std::string kUnauthorizedStatusLine =
    "HTTP/1.0 401 Unauthorized\r\n";
static const std::string kForbiddenStatusLine = "HTTP/1.0 403 Forbidden\r\n";
static const std::string kNotFoundStatusLine = "HTTP/1.0 404 Not Found\r\n";
static const std::string kInternalServerErrorStatusLine =
    "HTTP/1.0 500 Internal Server Error\r\n";
static const std::string kNotImplementedStatusLine =
    "HTTP/1.0 501 Not Implemented\r\n";
static const std::string kBadGatewayStatusLine = "HTTP/1.0 502 Bad Gateway\r\n";
static const std::string kServiceUnavailableStatusLine =
    "HTTP/1.0 503 Service Unavailable\r\n";

const std::string& ToStatusLine(StatusType status) {
  switch (status) {
    case kOk:
      return kOkStatusLine;
    case kCreated:
      return kCreatedStatusLine;
    case kAccepted:
      return kAcceptedStatusLine;
    case kNoContent:
      return kNoContentStatusLine;
    case kMultipleChoices:
      return kMultipleChoicesStatusLine;
    case kMovedPermanently:
      return kMovedPermanentlyStatusLine;
    case kMovedTemporarily:
      return kMovedTemporarilyStatusLine;
    case kNotModified:
      return kNotModifiedStatusLine;
    case kBadRequest:
      return kBadRequestStatusLine;
    case kUnauthorized:
      return kUnauthorizedStatusLine;
    case kForbidden:
      return kForbiddenStatusLine;
    case kNotFound:
      return kNotFoundStatusLine;
    case kInternalServerError:
      return kInternalServerErrorStatusLine;
    case kNotImplemented:
      return kNotImplementedStatusLine;
    case kBadGateway:
      return kBadGatewayStatusLine;
    case kServiceUnavailable:
      return kServiceUnavailableStatusLine;
    default:
      return kInternalServerErrorStatusLine;
  }
}

static const std::string kNameValueSeparator = ": ";
static const std::string kcrlf = "\r\n";

}

ReplyPtr Reply::StockReply(StatusType status) {
  ReplyPtr reply(new Reply);
  reply->status_ = status;
  reply->content_ = ToContent(status);
  reply->headers_.resize(2);
  reply->headers_[0].name = "Content-Length";
  std::ostringstream oss;
  oss << reply->content_.size();
  reply->headers_[0].value = oss.str();
  reply->headers_[1].name = "Content-Type";
  reply->headers_[1].value = "text/html";
  return reply;
}

int Reply::Decode(kit::Buffer *input) {
  //TODO use in client
  return -1;
}

void Reply::Encode(kit::Buffer *output) {
  output->Write(ToStatusLine(status_));
  for (std::size_t i = 0; i < headers_.size(); ++i) {
    Header& h = headers_[i];
    output->Write(h.name);
    output->Write(kNameValueSeparator);
    output->Write(h.value);
    output->Write(kcrlf);
  }
  output->Write(kcrlf);
  output->Write(content_);
}

}

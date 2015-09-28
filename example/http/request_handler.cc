/*
 * request_handler.cc
 *
 */

#include "example/http/request_handler.h"
#include <sstream>
#include <fstream>
#include "example/http/mime.h"

namespace http {

namespace {

int UrlDecode(const std::string& in, std::string* out) {
  out->clear();
  out->reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          (*out) += static_cast<char>(value);
          i += 2;
        } else {
          return -1;
        }
      } else {
        return -1;
      }
    } else if (in[i] == '+') {
      (*out) += ' ';
    } else {
      (*out) += in[i];
    }
  }
  return 0;
}

}

RequestHandler::RequestHandler(const std::string& doc_root)
    : doc_root_(doc_root) {
}

RequestHandler::~RequestHandler() {
}

ReplyPtr RequestHandler::Handle(const RequestPtr& request) {
  // Decode url to path.
  std::string request_path;
  if (UrlDecode(request->uri_, &request_path) < 0) {
    return Reply::StockReply(kBadRequest);
  }

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos) {
    return Reply::StockReply(kBadRequest);
  }

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/') {
    request_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::string full_path = doc_root_ + request_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is) {
    return Reply::StockReply(kNotFound);
  }

  // Fill out the reply to be sent to the client.
  ReplyPtr reply(new Reply);
  reply->status_ = kOk;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0) {
    reply->content_.append(buf, is.gcount());
  }
  reply->headers_.resize(2);
  reply->headers_[0].name = "Content-Length";
  std::ostringstream oss;
  oss << reply->content_.size();
  reply->headers_[0].value = oss.str();
  reply->headers_[1].name = "Content-Type";
  reply->headers_[1].value = ExtensionToType(extension);
  return reply;
}

}

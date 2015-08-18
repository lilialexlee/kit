/*
 * status.cc
 *
 */

#include "net/status.h"

namespace kit {

Status::Status(Code code, const std::string& message)
    : code_(code),
      message_(message) {
}

Status::Status()
    : code_(kOk),
      message_() {
}

Status::~Status() {
}

bool Status::Ok() const {
  return code_ == kOk;
}

const std::string& Status::GetInfo() const {
  return message_;
}

}

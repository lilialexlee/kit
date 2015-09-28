/*
 * request.cc
 *
 */

#include "example/http/request.h"
#include <iostream>

namespace http {

Request::Request()
    : method_(),
      uri_(),
      http_version_major_(0),
      http_version_minor_(0),
      headers_(),
      decode_stat_(kMethodStart) {

}

Request::~Request() {
}

namespace {

bool is_char(int c) {
  return c >= 0 && c <= 127;
}

bool is_ctl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

bool is_tspecial(int c) {
  switch (c) {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
    case '\t':
      return true;
    default:
      return false;
  }
}

bool is_digit(int c) {
  return c >= '0' && c <= '9';
}

}

int Request::Decode(kit::Buffer *input) {
  char* begin = input->PeekRead();
  char* end = begin + input->ReadableSize();
  char* p = begin;
  while (p != end) {
    int result = Consume(*p++);
    input->MarkRead(1);
    if (result == 1 || result == -1) {
      return result;
    }
  }
  return 0;
}

int Request::Consume(char c) {
  switch (decode_stat_) {
    case kMethodStart:
      if (!is_char(c) || is_ctl(c) || is_tspecial(c)) {
        return -1;
      } else {
        decode_stat_ = kMethod;
        method_.push_back(c);
        return 0;
      }
    case kMethod:
      if (c == ' ') {
        decode_stat_ = kUri;
        return 0;
      } else if (!is_char(c) || is_ctl(c) || is_tspecial(c)) {
        return -1;
      } else {
        method_.push_back(c);
        return 0;
      }
    case kUri:
      if (c == ' ') {
        decode_stat_ = kHttpVersionH;
        return 0;
      } else if (is_ctl(c)) {
        return -1;
      } else {
        uri_.push_back(c);
        return 0;
      }
    case kHttpVersionH:
      if (c == 'H') {
        decode_stat_ = kHttpVersionT1;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionT1:
      if (c == 'T') {
        decode_stat_ = kHttpVersionT2;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionT2:
      if (c == 'T') {
        decode_stat_ = kHttpVersionP;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionP:
      if (c == 'P') {
        decode_stat_ = kHttpVersionSlash;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionSlash:
      if (c == '/') {
        http_version_major_ = 0;
        http_version_minor_ = 0;
        decode_stat_ = kHttpVersionMajorStart;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionMajorStart:
      if (is_digit(c)) {
        http_version_major_ = http_version_major_ * 10 + c - '0';
        decode_stat_ = kHttpVersionMajor;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionMajor:
      if (c == '.') {
        decode_stat_ = kHttpVersionMinorStart;
        return 0;
      } else if (is_digit(c)) {
        http_version_major_ = http_version_major_ * 10 + c - '0';
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionMinorStart:
      if (is_digit(c)) {
        http_version_minor_ = http_version_minor_ * 10 + c - '0';
        decode_stat_ = kHttpVersionMinor;
        return 0;
      } else {
        return -1;
      }
    case kHttpVersionMinor:
      if (c == '\r') {
        decode_stat_ = kExpectingNewLine1;
        return 0;
      } else if (is_digit(c)) {
        http_version_minor_ = http_version_minor_ * 10 + c - '0';
        return 0;
      } else {
        return -1;
      }
    case kExpectingNewLine1:
      if (c == '\n') {
        decode_stat_ = kHeaderLineStart;
        return 0;
      } else {
        return -1;
      }
    case kHeaderLineStart:
      if (c == '\r') {
        decode_stat_ = kExpectingNewline3;
        return 0;
      } else if (!headers_.empty() && (c == ' ' || c == '\t')) {
        decode_stat_ = kHeaderLws;
        return 0;
      } else if (!is_char(c) || is_ctl(c) || is_tspecial(c)) {
        return -1;
      } else {
        headers_.push_back(Header());
        headers_.back().name.push_back(c);
        decode_stat_ = kHeaderName;
        return 0;
      }
    case kHeaderLws:
      if (c == '\r') {
        decode_stat_ = kExpectingNewline2;
        return 0;
      } else if (c == ' ' || c == '\t') {
        return 0;
      } else if (is_ctl(c)) {
        return -1;
      } else {
        decode_stat_ = kHeaderValue;
        headers_.back().value.push_back(c);
        return 0;
      }
    case kHeaderName:
      if (c == ':') {
        decode_stat_ = kSpaceBeforeHeaderValue;
        return 0;
      } else if (!is_char(c) || is_ctl(c) || is_tspecial(c)) {
        return -1;
      } else {
        headers_.back().name.push_back(c);
        return 0;
      }
    case kSpaceBeforeHeaderValue:
      if (c == ' ') {
        decode_stat_ = kHeaderValue;
        return 0;
      } else {
        return -1;
      }
    case kHeaderValue:
      if (c == '\r') {
        decode_stat_ = kExpectingNewline2;
        return 0;
      } else if (is_ctl(c)) {
        return -1;
      } else {
        headers_.back().value.push_back(c);
        return 0;
      }
    case kExpectingNewline2:
      if (c == '\n') {
        decode_stat_ = kHeaderLineStart;
        return 0;
      } else {
        return -1;
      }
    case kExpectingNewline3:
      return (c == '\n');
    default:
      return -1;
  }
}

void Request::Encode(kit::Buffer *output) {
  //TODO use in client
}

}


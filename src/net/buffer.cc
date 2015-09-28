/*
 * buffer.cc
 *
 */

#include "net/buffer.h"
#include <string.h>
#include <stdlib.h>
#include "util/log.h"

namespace kit {

static const int kInitBufSize = 1024;
static const int kCritialBufSize = 1024 * 1024;

Buffer::Buffer()
    : buf_(NULL),
      data_(buf_),
      size_(0),
      capacity_(0) {
}

Buffer::~Buffer() {
  if (buf_) {
    free(buf_);
  }
}

int Buffer::ReadableSize() {
  return size_;
}

int Buffer::WriteableSize() {
  return buf_ + capacity_ - (data_ + size_);
}

char* Buffer::PeekRead() {
  return data_;
}

void Buffer::MarkRead(int len) {
  if (len > size_) {
    len = size_;
    LOG_WARN("buffer MarkRead over limit, size_: %d, mark len: %d", size_, len);
  }
  data_ += len;
  size_ -= len;
  if (size_ == 0) {
    data_ = buf_;
  }
}

char* Buffer::PeekWrite() {
  return data_ + size_;
}

void Buffer::MarkWrited(int len) {
  if (len > WriteableSize()) {
    LOG_ERROR("buffer MarkWrited over limit, WriteableSize: %d, mark len: %d",
              WriteableSize(), len);
    return;
  }
  size_ += len;
}

void Buffer::Write(const char* data, int len) {
  MakeRoomForWrite(len);
  memcpy(data_ + size_, data, len);
  MarkWrited(len);
}

void Buffer::Write(const std::string& data) {
  Write(data.c_str(), data.size());
}

void Buffer::Shrink() {
  if (capacity_ < kCritialBufSize) {
    return;
  }
  if (size_ >= capacity_ / 4) {
    return;
  }
  int need = std::min(2 * size_, kCritialBufSize);
  Resize(need);
}

void Buffer::MakeRoomForWrite(int len) {
  if (WriteableSize() > len) {
    return;
  }
  int need = std::max((len + size_) * 2, kInitBufSize);
  Resize(need);
}

void Buffer::Resize(int len) {
  if (len < size_) {
    LOG_ERROR("buffer resize error, size_: %d, resize len: %d", size_, len);
    return;
  }
  char* new_buf = (char *) malloc(len);
  memcpy(new_buf, data_, size_);
  if (buf_) {
    free(buf_);
  }
  buf_ = new_buf;
  data_ = buf_;
  capacity_ = len;
}

}


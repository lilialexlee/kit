/*
 * buffer.h
 *
 */

#ifndef SRC_NET_BUFFER_H_
#define SRC_NET_BUFFER_H_

#include <string>

namespace kit {

class Buffer {
 public:
  Buffer();
  ~Buffer();

  int ReadableSize();
  int WriteableSize();

  char* PeekRead();
  void MarkRead(int len);
  char* PeekWrite();
  void MarkWrited(int len);
  void Write(const char* data, int len);
  void Write(const std::string& data);

  void Shrink();
  void MakeRoomForWrite(int len);

 private:
  void Resize(int len);
  char* buf_;
  char* data_;
  int size_;
  int capacity_;

  Buffer(const Buffer&);
  void operator=(const Buffer&);
};

}

#endif /* SRC_NET_BUFFER_H_ */

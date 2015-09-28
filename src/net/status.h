/*
 * status.h
 *
 */

#ifndef SRC_NET_STATUS_H_
#define SRC_NET_STATUS_H_

#include <string>

namespace kit {

class Status {
 public:
  enum Code {
    kOk = 0,
    kConnectError,
    kConnectTimeout,
    kSocketError,
    kReadError,
    kWriteError,
    kPeerDisconnect,
    kMessageDecodeError
  };

  Status(Code code, const std::string& message);
  Status();
  ~Status();

  bool Ok() const;
  const std::string& Info() const;

 private:
  Code code_;
  std::string message_;
};

}

#endif /* SRC_NET_STATUS_H_ */

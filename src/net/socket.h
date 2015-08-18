/*
 * socket.h
 *
 */

#ifndef SRC_NET_SOCKET_H_
#define SRC_NET_SOCKET_H_

#include <unistd.h>
#include <string>
#include <boost/shared_ptr.hpp>

namespace kit {

class Socket {
 public:
  Socket();
  ~Socket();

  int CreatSocketHandle();
  int SetupSocketHandle(int sock_fd);

  int Fd();
  bool Ok();

  int Close();
  int ShutDownWrite();

  int SetNonBlock(bool on);
  int SetTcpNoDelay(bool on);
  int SetKeepAlive(bool on);
  int SetReuseAddr(bool on);
  int GetSoError();
  int GetLocalAddr(std::string *ip, int *port);
  int GetPeerAddr(std::string *ip, int *port);

  int Bind(const std::string& ip, int port);
  int Listen(const std::string& ip, int port, int backlog);
  int Accept();
  int Connect(const std::string& ip, int port);

  ssize_t Read(void *buf, size_t count);
  ssize_t Write(const void *buf, size_t count);

 private:
  int sock_fd_;

  Socket(const Socket&);
  void operator=(const Socket&);
};

typedef boost::shared_ptr<Socket> SocketPtr;

}

#endif /* SRC_NET_SOCKET_H_ */

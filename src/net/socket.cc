/*
 * socket.cc
 *
 */

#include "net/socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "util/log.h"

namespace kit {

Socket::Socket()
    : sock_fd_(-1) {
}

Socket::~Socket() {
  Close();
}

int Socket::CreatSocketHandle() {
  if (sock_fd_ > 0) {
    LOG_ERROR("socket already created: %d", sock_fd_);
    return -1;
  }
  sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd_ == -1) {
    LOG_ERROR("create socket handle failed, error: %d", errno);
    return -1;
  }
  return 0;
}

int Socket::SetupSocketHandle(int sock_fd) {
  if (sock_fd_ > 0) {
    LOG_ERROR("Setup fail, already has socket: %d", sock_fd_);
    return -1;
  }
  if (sock_fd <= 0) {
    LOG_ERROR("Setup invalid sock:%d", sock_fd);
    return -1;
  }
  sock_fd_ = sock_fd;
  return 0;
}

int Socket::Fd() {
  return sock_fd_;
}

bool Socket::Ok() {
  return sock_fd_ > 0;
}

int Socket::Close() {
  if (sock_fd_ > 0) {
    if (::close(sock_fd_) == -1) {
      LOG_ERROR("close fail, fd: %d, error: %d", sock_fd_, errno);
      sock_fd_ = -1;
      return -1;
    } else {
      sock_fd_ = -1;
    }
  }
  return 0;
}

int Socket::ShutDownWrite() {
  if (sock_fd_ <= 0) {
    return -1;
  }
  if (::shutdown(sock_fd_, SHUT_WR) < 0) {
    LOG_ERROR("shutdown fail, fd: %d, error: %d", sock_fd_, errno);
    return -1;
  }
  return 0;
}

int Socket::SetNonBlock(bool on) {
  int flags = fcntl(sock_fd_, F_GETFL);
  if (flags == -1) {
    LOG_ERROR("fcntl F_GETFL fail, fd: %d, error: %d", sock_fd_, errno);
    return -1;
  }
  if (on) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }
  if (fcntl(sock_fd_, F_SETFL, flags) == -1) {
    LOG_ERROR("fcntl F_SETFL fail, fd: %d, error: %d", sock_fd_, errno);
    return -1;
  }
  return 0;
}

int Socket::SetTcpNoDelay(bool on) {
  int val = on;
  if (setsockopt(sock_fd_, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1) {
    LOG_ERROR("setsockopt TCP_NODELAY fail, fd: %d, error: %d", sock_fd_,
              errno);
    return -1;
  }
  return 0;
}

int Socket::SetKeepAlive(bool on) {
  int val = on;
  if (setsockopt(sock_fd_, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1) {
    LOG_ERROR("setsockopt SO_KEEPALIVE fail, fd: %d, error: %d", sock_fd_,
              errno);
    return -1;
  }
  return 0;
}

int Socket::SetReuseAddr(bool on) {
  int val = on;
  if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
    LOG_ERROR("setsockopt SO_KEEPALIVE fail, fd: %d, error: %d", sock_fd_,
              errno);
    return -1;
  }
  return 0;
}

int Socket::GetSoError() {
  int val;
  socklen_t len = static_cast<socklen_t>(sizeof val);
  if (::getsockopt(sock_fd_, SOL_SOCKET, SO_ERROR, &val, &len) < 0) {
    return errno;
  } else {
    return val;
  }
}

static int kNetIpLen = 16;

int Socket::GetLocalAddr(std::string *ip, int *port) {
  struct ::sockaddr_in localaddr;
  memset(&localaddr, 0, sizeof(localaddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sock_fd_, (struct sockaddr *) &localaddr, &addrlen) < 0) {
    LOG_ERROR("getsockname failed, socket: %d, error: %d", sock_fd_, errno);
    return -1;
  }
  char ip_str[kNetIpLen];
  inet_ntop(AF_INET, &localaddr.sin_addr, ip_str, kNetIpLen);
  *ip = ip_str;
  *port = ntohs(localaddr.sin_port);
  return 0;
}

int Socket::GetPeerAddr(std::string *ip, int *port) {
  struct ::sockaddr_in peeraddr;
  memset(&peeraddr, 0, sizeof(peeraddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sock_fd_, (struct sockaddr *) &peeraddr, &addrlen) < 0) {
    LOG_ERROR("getpeername failed, socket: %d, error: %d", sock_fd_, errno);
    return -1;
  }
  char ip_str[kNetIpLen];
  inet_ntop(AF_INET, &peeraddr.sin_addr, ip_str, kNetIpLen);
  *ip = ip_str;
  *port = ntohs(peeraddr.sin_port);
  return 0;
}

int Socket::Bind(const std::string& ip, int port) {
  if (sock_fd_ <= 0) {
    return -1;
  }
  struct ::sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((short) port);
  if (ip.empty()) {
    addr.sin_addr.s_addr = INADDR_ANY;
  } else {
    inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr));
  }
  if (::bind(sock_fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    LOG_ERROR("bind fail, fd: %d, ip: %s, port: %d error: %d", sock_fd_,
              ip.c_str(), port, errno);
    return -1;
  }
  return 0;
}

int Socket::Listen(const std::string& ip, int port, int backlog) {
  if (sock_fd_ <= 0) {
    return -1;
  }
  if (Bind(ip, port) < 0) {
    return -1;
  }
  if (::listen(sock_fd_, backlog) < 0) {
    LOG_ERROR("listen fail, fd: %d, ip: %s, port: %d error: %d", sock_fd_,
              ip.c_str(), port, errno);
    return -1;
  }
  return 0;
}

int Socket::Accept() {
  if (sock_fd_ <= 0) {
    return -1;
  }
  int fd = -1;
  struct ::sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);
  while (true) {
    fd = ::accept(sock_fd_, (struct sockaddr*) &client_addr, &addr_len);
    if (fd == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        LOG_ERROR("accept fail, fd: %d, error: %d", sock_fd_, errno);
        return -1;
      }
    }
    break;
  }
  return fd;
}

int Socket::Connect(const std::string& ip, int port) {
  if (sock_fd_ <= 0) {
    return -1;
  }
  struct ::sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((short) port);
  inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
  if (::connect(sock_fd_, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    if (errno != EINPROGRESS) {
      LOG_ERROR("connect fail, fd: %d, ip: %s, port: %d error: %d", sock_fd_,
                ip.c_str(), port, errno);
      return -1;
    } else {
      return 1;
    }
  }
  return 0;
}

ssize_t Socket::Read(void *buf, size_t count) {
  if (sock_fd_ <= 0) {
    return -1;
  }
  ssize_t len = 0;
  while (true) {
    len = ::read(sock_fd_, buf, count);
    if (len == -1) {
      if (errno == EINTR) {
        continue;
      } else if (errno == EWOULDBLOCK) {
      } else {
        LOG_ERROR("fd: %d, read: -1, want: %d, error: %d", sock_fd_, count,
                  errno);
        return -1;
      }
    } else {
      LOG_TRACE("fd: %d, want=%d, read: %d", sock_fd_, count, len);
    }
    break;
  }
  return len;
}

ssize_t Socket::Write(const void *buf, size_t count) {
  if (sock_fd_ <= 0) {
    return -1;
  }
  ssize_t len = 0;
  while (true) {
    len = ::write(sock_fd_, buf, count);
    if (len == -1) {
      if (errno == EINTR) {
        continue;
      } else if (errno == EWOULDBLOCK) {
      } else {
        LOG_ERROR("fd: %d, write: -1, want: %d, error: %d", sock_fd_, count,
                  errno);
        return -1;
      }
    } else {
      LOG_TRACE("fd: %d, want=%d, write: %d", sock_fd_, count, len);
    }
    break;
  }
  return len;
}

}


/*
 * accept.cc
 *
 */

#include <arpa/inet.h>
#include <boost/bind.hpp>
#include "net/acceptor.h"
#include "util/log.h"

namespace kit {

namespace {

class AcceptorCallBack : public FileCallBack {
 public:
  AcceptorCallBack(const AcceptorPtr& acceptor)
      : acceptor_(acceptor) {
  }

  ~AcceptorCallBack() {
  }

  void ProcessRead() {
    acceptor_->ProcessRead();
  }

  void ProcessWrite() {
  }

  void ProcessError() {
    acceptor_->ProcessError();
  }

 private:
  AcceptorPtr acceptor_;
};

}

Acceptor::Acceptor(EventLoop* loop, const NewConnectionCallback& cb)
    : loop_(loop),
      listen_socket_(),
      new_connection_callback_(cb) {
}

Acceptor::~Acceptor() {
}

static const int kListenBackLog = 512;

void Acceptor::Listen(const std::string& ip, int port) {
  loop_->RunInLoopThread(
      boost::bind(&Acceptor::StartListen, shared_from_this(), ip, port));
}

void Acceptor::Stop() {
  loop_->RunInLoopThread(
      boost::bind(&Acceptor::StopListen, shared_from_this()));
}

void Acceptor::ProcessRead() {
  int sockfd = listen_socket_.Accept();
  if (sockfd > 0) {
    SocketPtr socket(new Socket());
    socket->SetupSocketHandle(sockfd);
    ConnectionPtr connection(new Connection(socket));
    new_connection_callback_(connection);
  }
}

void Acceptor::ProcessError() {
  int error = listen_socket_.GetSoError();
  LOG_ERROR("something wrong with the acceptor, listen sock: %d. error: %d",
            listen_socket_.Fd(), error);
}

void Acceptor::StartListen(const std::string& ip, int port) {
  if (listen_socket_.CreatSocketHandle() < 0) {
    return;
  }
  if (listen_socket_.SetReuseAddr(true) < 0) {
    return;
  }
  if (listen_socket_.Listen(ip, port, kListenBackLog) < 0) {
    return;
  }
  LOG_INFO("begin listen, ip: %s, port: %d, listen sock: %d.", ip.c_str(), port,
           listen_socket_.Fd());
  FileCallBackPtr cb(new AcceptorCallBack(shared_from_this()));
  if (loop_->SetFileEvent(listen_socket_.Fd(), kEventRead, cb) < 0) {
    return;
  }
}

void Acceptor::StopListen() {
  loop_->DeleteFileEvent(listen_socket_.Fd(), kEventRead);
  listen_socket_.Close();
}

}


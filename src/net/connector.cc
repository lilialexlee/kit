/*
 * connector.cc
 *
 */

#include <boost/bind.hpp>
#include "net/connector.h"
#include "util/log.h"

namespace kit {

namespace {

class ConnectorCallBack : public FileCallBack {
 public:
  ConnectorCallBack(const ConnectorPtr& connector)
      : connector_(connector) {
  }

  ~ConnectorCallBack() {
  }

  void ProcessRead() {
  }

  void ProcessWrite() {
    connector_->ProcessWrite();
  }

  void ProcessError() {
    connector_->ProcessError();
  }

 private:
  ConnectorPtr connector_;
};

}

Connector::Connector(EventLoop* loop, const std::string& ip, int port,
                     int timeout_ms, const ConnectCallback& cb)
    : loop_(loop),
      ip_(ip),
      port_(port),
      timeout_ms_(timeout_ms),
      socket_(),
      is_connecting_(false),
      call_back_(cb) {
}

Connector::~Connector() {
}

void Connector::Start() {
  loop_->RunInLoopThread(
      boost::bind(&Connector::StartConnect, shared_from_this()));
}

void Connector::Stop() {
  loop_->RunInLoopThread(
      boost::bind(&Connector::StopConnect, shared_from_this()));
}

void Connector::ProcessWrite() {
  loop_->DeleteFileEvent(socket_->Fd(), kEventWrite);
  is_connecting_ = false;
  int err = socket_->GetSoError();
  if (err) {
    LOG_ERROR("connect failed, sock: %d, error: %d", socket_->Fd(), err);
    OnConnectError(Status(Status::kConnectError, "connect error"));
  } else {
    OnConnected();
  }
}

void Connector::ProcessError() {
  int error = socket_->GetSoError();
  LOG_ERROR("connect failed, sock: %d error: %d.", socket_->Fd(), error);
  loop_->DeleteFileEvent(socket_->Fd(), kEventWrite);
  is_connecting_ = false;
  OnConnectError(Status(Status::kConnectError, "connect error"));
}

void Connector::StartConnect() {
  socket_.reset(new Socket());
  socket_->SetNonBlock(true);
  int r = socket_->Connect(ip_, port_);
  if (r < 0) {
    OnConnectError(Status(Status::kConnectError, "connect error"));
  } else if (r == 0) {
    OnConnected();
  } else {
    FileCallBackPtr cb(new ConnectorCallBack(shared_from_this()));
    loop_->SetFileEvent(socket_->Fd(), kEventWrite, cb);
    loop_->AddTimeEvent(
        timeout_ms_,
        boost::bind(&Connector::OnConnectTimeout, shared_from_this()));
    is_connecting_ = true;
  }
}
void Connector::StopConnect() {
  if (is_connecting_) {
    loop_->DeleteFileEvent(socket_->Fd(), kEventWrite);
    socket_.reset();
    is_connecting_ = false;
  }
}

void Connector::OnConnected() {
  ConnectionPtr connection(new Connection(socket_));
  if (connection->IsSelfConnect()) {
    LOG_ERROR("connect failed, sock: %d, self connect", socket_->Fd());
    OnConnectError(Status(Status::kConnectError, "self connect"));
  } else {
    LOG_INFO("connect success, sock: %d, ip: %s, port: %d", socket_->Fd(),
             ip_.c_str(), port_);
    call_back_(Status(), connection);
  }
}

void Connector::OnConnectTimeout() {
  if (is_connecting_) {
    StopConnect();
  }
  OnConnectError(Status(Status::kConnectTimeout, "connect timeout"));
}

void Connector::OnConnectError(const Status& status) {
  socket_.reset();
  ConnectionPtr place_holder;
  call_back_(status, place_holder);
}

}

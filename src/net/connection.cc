/*
 * connection.cc
 *
 */

#include <string.h>
#include <sstream>
#include <boost/bind.hpp>
#include "net/connection.h"
#include "util/log.h"

namespace kit {

namespace {

class ConnectionCallBack : public FileCallBack {
 public:
  ConnectionCallBack(const ConnectionPtr& connection)
      : connection_(connection) {
  }

  ~ConnectionCallBack() {
  }

  void ProcessRead() {
    connection_->ProcessRead();
  }

  void ProcessWrite() {
    connection_->ProcessWrite();
  }

  void ProcessError() {
    connection_->ProcessError();
  }

 private:
  ConnectionPtr connection_;
};

}

Connection::Connection(const SocketPtr& socket)
    : socket_(socket),
      local_ip_(),
      local_port_(0),
      peer_ip_(),
      peer_port_(0),
      identifier_(),
      in_(),
      out_(),
      to_be_send_messages_(),
      parser_(),
      received_message_(),
      loop_(NULL),
      message_received_call_back_(),
      exception_call_back_() {
  socket_->GetLocalAddr(&local_ip_, &local_port_);
  socket_->GetPeerAddr(&peer_ip_, &peer_port_);
  std::ostringstream oss;
  oss << peer_ip_ << ":" << peer_port_ << "-" << local_ip_ << ":"
      << local_port_;
  identifier_ = oss.str();
}

Connection::~Connection() {
}

int Connection::Fd() {
  return socket_->Fd();
}

bool Connection::IsSelfConnect() {
  return local_ip_ == peer_ip_ && local_port_ == peer_port_;
}

void Connection::Init(EventLoop* loop, const MessageParserPtr& parser,
                      const MessageReceivedCallback& message_cb,
                      const ExceptionCallback& excetion_cb) {
  socket_->SetNonBlock(true);
  socket_->SetTcpNoDelay(true);
  socket_->SetKeepAlive(true);
  parser_ = parser;
  message_received_call_back_ = message_cb;
  exception_call_back_ = excetion_cb;
  loop_ = loop;
  loop_->RunInLoopThread(
      boost::bind(&Connection::ConnectionStart, shared_from_this()));
}

void Connection::Send(const MessagePtr& message) {
  loop_->RunInLoopThread(
      boost::bind(&Connection::AppendMessageAndTrySend, shared_from_this(),
                  message));
}

void Connection::Stop() {
  loop_->RunInLoopThread(
      boost::bind(&Connection::ConnectionStop, shared_from_this()));
}

static const int kTryReadSize = 16 * 1024;

void Connection::ProcessRead() {
  in_.MakeRoomForWrite(kTryReadSize);
  ssize_t n = socket_->Read(in_.PeekWrite(), in_.WriteableSize());
  if (n < 0) {
    OnException(Status(Status::kReadError, "connection read error"));
    return;
  } else if (n == 0) {
    LOG_INFO("peer: %s:%d closed connection.", peer_ip_.c_str(), peer_port_);
    OnPeerDisconnect();
    return;
  } else {
    in_.MarkWrited(n);
    while (true) {
      int r = parser_->Decode(&in_, received_message_);
      if (r < 0) {
        LOG_ERROR("decode message failed, socket: %d, peer: %s:%d.",
                  socket_->Fd(), peer_ip_.c_str(), peer_port_);
        OnException(
            Status(Status::kMessageDecodeError, "message decode error"));
        return;
      } else if (r == 0) {
        return;
      } else {
        message_received_call_back_(received_message_, shared_from_this());
        received_message_.reset();
      }
    }
    in_.Shrink();
  }
}

void Connection::ProcessWrite() {
  if (Send() < 0) {
    OnException(Status(Status::kWriteError, "connection write error"));
    return;
  }
  if (out_.ReadableSize() == 0 && to_be_send_messages_.empty()) {
    DisableLoopWriteEvent();
  }
}

void Connection::ProcessError() {
  int error = socket_->GetSoError();
  LOG_ERROR("connection error, sock: %d error: %d.", socket_->Fd(), error);
  OnException(Status(Status::kSocketError, "socket error"));
}

std::string Connection::GetUniqueIdentifier() {
  return identifier_;
}

void Connection::ConnectionStart() {
  socket_->SetNonBlock(true);
  socket_->SetTcpNoDelay(true);
  socket_->SetKeepAlive(true);
  FileCallBackPtr cb(new ConnectionCallBack(shared_from_this()));
  loop_->SetFileEvent(Fd(), kEventRead, cb);
}

void Connection::ConnectionStop() {
  loop_->DeleteFileEvent(Fd(), kEventRead | kEventWrite);
  socket_->Close();
}

void Connection::EnableLoopWriteEvent() {
  int registed_mask = loop_->GetRegisteredFileMask(Fd());
  if (registed_mask & kEventWrite) {
    return;
  }
  FileCallBackPtr cb(new ConnectionCallBack(shared_from_this()));
  loop_->SetFileEvent(Fd(), registed_mask | kEventWrite, cb);
}

void Connection::DisableLoopWriteEvent() {
  int registed_mask = loop_->GetRegisteredFileMask(Fd());
  if (!(registed_mask & kEventWrite)) {
    return;
  }
  loop_->DeleteFileEvent(Fd(), kEventWrite);
}

void Connection::AppendMessageAndTrySend(const MessagePtr& message) {
  to_be_send_messages_.push(message);
  if (Send() < 0) {
    OnException(Status(Status::kWriteError, "connection write error"));
    return;
  }
  if (!(out_.ReadableSize() == 0 && to_be_send_messages_.empty())) {
    EnableLoopWriteEvent();
  }
}

int Connection::WriteOutBuffer() {
  if (out_.ReadableSize() > 0) {
    ssize_t n = socket_->Write(out_.PeekRead(), out_.ReadableSize());
    if (n < 0) {
      return n;
    }
    out_.MarkRead(n);
    return n;
  }
  return 0;
}

int Connection::Send() {
  int n = WriteOutBuffer();
  if (n < 0) {
    return -1;
  }
  while (out_.ReadableSize() == 0 && !to_be_send_messages_.empty()) {
    MessagePtr message = to_be_send_messages_.front();
    to_be_send_messages_.pop();
    parser_->Encode(message, &out_);
    int n = WriteOutBuffer();
    if (n < 0) {
      return -1;
    }
  }
  out_.Shrink();
  return 0;
}

void Connection::OnException(const Status status) {
  ConnectionStop();
  exception_call_back_(status, shared_from_this());
}

void Connection::OnPeerDisconnect() {
  OnException(Status(Status::kPeerDisconnect, "peer closed"));
}

}


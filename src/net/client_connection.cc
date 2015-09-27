/*
 * client_connection.cc
 *
 */

#include <boost/bind.hpp>
#include "net/client_connection.h"

namespace kit {

namespace {

class ClientConnectionCallBack : public FileCallBack {
 public:
  ClientConnectionCallBack(const ClientConnectionPtr& connection)
      : connection_(connection) {
  }

  ~ClientConnectionCallBack() {
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
  ClientConnectionPtr connection_;
};

}

ClientConnection::ClientConnection(const SocketPtr& socket)
    : connection_(new Connection(socket)),
      loop_(NULL),
      message_received_call_back_(),
      exception_call_back_() {
}

ClientConnection::~ClientConnection() {
}

bool ClientConnection::IsSelfConnect() {
  return connection_->IsSelfConnect();
}

void ClientConnection::Init(EventLoop* loop, const MessageParserPtr& parser,
                            const MessageReceivedCallback& message_cb,
                            const ExceptionCallback& excetion_cb) {
  loop_ = loop;
  connection_->Init(parser);
  message_received_call_back_ = message_cb;
  exception_call_back_ = excetion_cb;
  loop_->RunInLoopThread(
      boost::bind(&ClientConnection::StartInLoop, shared_from_this()));
}

void ClientConnection::Close() {
  loop_->RunInLoopThread(
      boost::bind(&ClientConnection::CloseInLoop, shared_from_this()));
}

void ClientConnection::Send(const MessagePtr& message) {
  loop_->RunInLoopThread(
      boost::bind(&ClientConnection::SendInLoop, shared_from_this(), message));
}

void ClientConnection::SendInLoop(const MessagePtr& message) {
  if (connection_->Send(message) < 0) {
    OnException(Status(Status::kWriteError, "connection write error"));
    return;
  }
  if (!connection_->WriteBufferEmpty()) {
    EnableLoopWriteEvent();
  }
}

void ClientConnection::ProcessRead() {
  int r = connection_->Read();
  if (r < 0) {
    OnException(Status(Status::kReadError, "connection read error"));
    return;
  } else if (r == 0) {
    OnPeerDisconnect();
    return;
  }
  while (true) {
    MessagePtr request;
    int r = connection_->RetrieveMessage(request);
    if (r < 0) {
      OnException(Status(Status::kMessageDecodeError, "message decode error"));
      return;
    } else if (r == 0) {
      break;
    } else {
      OnMessageReceived(request);
    }
  }
}

void ClientConnection::ProcessWrite() {
  if (connection_->Write() < 0) {
    OnException(Status(Status::kWriteError, "connection write error"));
    return;
  }
  if (connection_->WriteBufferEmpty()) {
    DisableLoopWriteEvent();
  }
}

void ClientConnection::ProcessError() {
  int error = connection_->Socket()->GetSoError();
  LOG_ERROR("connection error, sock: %d error: %d.", connection_->Fd(), error);
  OnException(Status(Status::kSocketError, "socket error"));
}

void ClientConnection::StartInLoop() {
  SocketPtr socket = connection_->Socket();
  socket->SetNonBlock(true);
  socket->SetTcpNoDelay(true);
  FileCallBackPtr cb(new ClientConnectionCallBack(shared_from_this()));
  loop_->SetFileEvent(connection_->Fd(), kEventRead, cb);
}

void ClientConnection::CloseInLoop() {
  loop_->DeleteFileEvent(connection_->Fd(), kEventRead | kEventWrite);
  connection_->Close();
}

void ClientConnection::EnableLoopWriteEvent() {
  int registed_mask = loop_->GetRegisteredFileMask(connection_->Fd());
  if (registed_mask & kEventWrite) {
    return;
  }
  FileCallBackPtr cb(new ClientConnectionCallBack(shared_from_this()));
  loop_->SetFileEvent(connection_->Fd(), registed_mask | kEventWrite, cb);
}

void ClientConnection::DisableLoopWriteEvent() {
  int registed_mask = loop_->GetRegisteredFileMask(connection_->Fd());
  if (!(registed_mask & kEventWrite)) {
    return;
  }
  loop_->DeleteFileEvent(connection_->Fd(), kEventWrite);

}

void ClientConnection::OnMessageReceived(const MessagePtr& message) {
  message_received_call_back_(message);
}

void ClientConnection::OnPeerDisconnect() {
  OnException(Status(Status::kPeerDisconnect, "peer closed"));
}

void ClientConnection::OnException(const Status status) {
  CloseInLoop();
  exception_call_back_(status);
}

}

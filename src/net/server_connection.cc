/*
 * server_connection.cc
 *
 */

#include <boost/bind.hpp>
#include "net/server_connection.h"

namespace kit {

namespace {

class ServerConnectionCallBack : public FileCallBack {
 public:
  ServerConnectionCallBack(const ServerConnectionPtr& connection)
      : connection_(connection) {
  }

  ~ServerConnectionCallBack() {
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
  ServerConnectionPtr connection_;
};

}

ServerConnection::ServerConnection(const SocketPtr& socket)
    : connection_(new Connection(socket)),
      loop_(NULL),
      message_received_call_back_(),
      exception_call_back_(),
      process_request_in_order_(false),
      is_processing_(false) {
  LOG_INFO("new connection, socket: %d, %s", connection_->Fd(),
           connection_->Identifier().c_str());
}

ServerConnection::~ServerConnection() {
  LOG_TRACE("connection: %s destroy", connection_->Identifier().c_str());
}

void ServerConnection::Init(EventLoop* loop, const MessageParserPtr& parser,
                            const MessageReceivedCallback& message_cb,
                            const ExceptionCallback& excetion_cb,
                            bool process_request_in_order) {
  loop_ = loop;
  connection_->Init(parser);
  message_received_call_back_ = message_cb;
  exception_call_back_ = excetion_cb;
  process_request_in_order_ = process_request_in_order;
  loop_->RunInLoopThread(
      boost::bind(&ServerConnection::StartInLoop, shared_from_this()));
}

void ServerConnection::Close() {
  loop_->RunInLoopThread(
      boost::bind(&ServerConnection::CloseInLoop, shared_from_this()));
}

void ServerConnection::SendReply(const MessagePtr& reply) {
  if (connection_->Send(reply) < 0) {
    OnException(Status(Status::kWriteError, "connection write error"));
    return;
  }
  if (!connection_->WriteBufferEmpty()) {
    EnableLoopWriteEvent();
  }
  if (process_request_in_order_) {
    is_processing_ = false;
    MessagePtr request;
    int r = connection_->RetrieveMessage(request);
    if (r < 0) {
      OnException(Status(Status::kMessageDecodeError, "message decode error"));
    } else if (r > 0) {
      is_processing_ = true;
      OnMessageReceived(request);
    }
  }
}

void ServerConnection::ProcessRead() {
  int r = connection_->Read();
  if (r < 0) {
    OnException(Status(Status::kReadError, "connection read error"));
    return;
  } else if (r == 0) {
    OnPeerDisconnect();
    return;
  }
  //some server should process requests from the same connection in order
  //in this case, just only decode one message
  while (!process_request_in_order_ || !is_processing_) {
    MessagePtr request;
    int r = connection_->RetrieveMessage(request);
    if (r < 0) {
      OnException(Status(Status::kMessageDecodeError, "message decode error"));
      return;
    } else if (r == 0) {
      break;
    } else {
      if (process_request_in_order_) {
        is_processing_ = true;
      }
      OnMessageReceived(request);
    }
  }
}

void ServerConnection::ProcessWrite() {
  if (connection_->Write() < 0) {
    OnException(Status(Status::kWriteError, "connection write error"));
    return;
  }
  if (connection_->WriteBufferEmpty()) {
    DisableLoopWriteEvent();
  }
}

void ServerConnection::ProcessError() {
  int error = connection_->Socket()->GetSoError();
  LOG_ERROR("connection error, sock: %d error: %d.", connection_->Fd(), error);
  OnException(Status(Status::kSocketError, "socket error"));
}

void ServerConnection::StartInLoop() {
  SocketPtr socket = connection_->Socket();
  socket->SetNonBlock(true);
  socket->SetTcpNoDelay(true);
  FileCallBackPtr cb(new ServerConnectionCallBack(shared_from_this()));
  loop_->SetFileEvent(connection_->Fd(), kEventRead, cb);
}

void ServerConnection::CloseInLoop() {
  loop_->DeleteFileEvent(connection_->Fd(), kEventRead | kEventWrite);
  connection_->Close();
}

void ServerConnection::EnableLoopWriteEvent() {
  int registed_mask = loop_->GetRegisteredFileMask(connection_->Fd());
  if (registed_mask & kEventWrite) {
    return;
  }
  FileCallBackPtr cb(new ServerConnectionCallBack(shared_from_this()));
  loop_->SetFileEvent(connection_->Fd(), registed_mask | kEventWrite, cb);
}

void ServerConnection::DisableLoopWriteEvent() {
  int registed_mask = loop_->GetRegisteredFileMask(connection_->Fd());
  if (!(registed_mask & kEventWrite)) {
    return;
  }
  loop_->DeleteFileEvent(connection_->Fd(), kEventWrite);

}

void ServerConnection::OnMessageReceived(const MessagePtr& message) {
  message_received_call_back_(message, shared_from_this());
}

void ServerConnection::OnPeerDisconnect() {
  OnException(Status(Status::kPeerDisconnect, "peer closed"));
}

void ServerConnection::OnException(const Status status) {
  CloseInLoop();
  exception_call_back_(status, shared_from_this());
}

}


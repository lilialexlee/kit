/*
 * connection.cc
 *
 */

#include <string.h>
#include <sstream>
#include "net/connection.h"
#include "util/log.h"

namespace kit {

Connection::Connection(const SocketPtr& socket)
    : socket_(socket),
      local_ip_(),
      local_port_(0),
      peer_ip_(),
      peer_port_(0),
      identifier_(),
      in_(),
      out_(),
      received_message_(),
      parser_() {
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

bool Connection::Ok() {
  return socket_->Ok();
}

bool Connection::IsSelfConnect() {
  return local_ip_ == peer_ip_ && local_port_ == peer_port_;
}

std::string Connection::Identifier() {
  return identifier_;
}

SocketPtr Connection::Socket() {
  return socket_;
}

int Connection::Close() {
  return socket_->Close();
}

void Connection::Init(const MessageParserPtr& parser) {
  parser_ = parser;
}

static const int kTryReadSize = 16 * 1024;

int Connection::Read() {
  in_.MakeRoomForWrite(kTryReadSize);
  ssize_t n = socket_->Read(in_.PeekWrite(), in_.WriteableSize());
  if (n > 0) {
    in_.MarkWrited(n);
  } else if (n == 0) {
    LOG_INFO("peer: %s:%d closed connection, socket: %d.", peer_ip_.c_str(),
             peer_port_, socket_->Fd());
  }
  return n;
}

int Connection::RetrieveMessage(MessagePtr& message) {
  int r = parser_->Decode(&in_, received_message_);
  if (r > 0) {
    message = received_message_;
    received_message_.reset();
    in_.Shrink();
  } else if (r == 0) {
    //message incomplete, need to read more
  } else {
    LOG_ERROR("message decode error, peer: %s:%d, sock: %d.", peer_ip_.c_str(),
              peer_port_, socket_->Fd());
  }
  return r;
}

int Connection::Write() {
  if (!WriteBufferEmpty()) {
    ssize_t n = socket_->Write(out_.PeekRead(), out_.ReadableSize());
    if (n > 0) {
      out_.MarkRead(n);
      out_.Shrink();
    }
    return n;
  }
  return 0;
}

int Connection::Send(const MessagePtr& message) {
  bool write_buffer_empty = WriteBufferEmpty();
  parser_->Encode(message, &out_);
  if (write_buffer_empty) {  // if no thing in output bufer, try writing directly
    if (Write() < 0) {
      return -1;
    }
  }
  return out_.ReadableSize();
}

bool Connection::WriteBufferEmpty() {
  return out_.ReadableSize() == 0;
}

}


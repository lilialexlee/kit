/*
 * client.cc
 *
 */

#include <boost/bind.hpp>
#include "net/client.h"
#include "util/log.h"

namespace kit {

Client::Client(EventLoop* loop)
    : loop_(loop),
      mutex_(),
      state_(kDisConnected),
      connector_(),
      connection_(),
      connect_call_back_(),
      parser_(),
      message_received_call_back_(),
      connection_exception_call_back_() {
}

Client::~Client() {
  boost::mutex::scoped_lock lock(mutex_);
  if (state_ != kDisConnected) {
    Close();
  }
}

void Client::Setup(const MessageParserPtr& parser,
                   const MessageReceivedCallback& message_cb,
                   const ConnectionExceptionCallback& conn_exception_cb) {
  parser_ = parser;
  message_received_call_back_ = message_cb;
  connection_exception_call_back_ = conn_exception_cb;
}

void Client::Connect(const std::string& ip, int port, int timeout_ms,
                     const ConnectCallback cb) {
  boost::mutex::scoped_lock lock(mutex_);
  if (state_ != kDisConnected) {
    return;
  }
  connect_call_back_ = cb;
  connector_.reset(
      new Connector(
          loop_,
          ip,
          port,
          timeout_ms,
          boost::bind(&Client::OnConnectCompleted, shared_from_this(), _1,
                      _2)));
  state_ = kConnecting;
  connector_->Start();
}

void Client::Send(const MessagePtr& message) {
  boost::mutex::scoped_lock lock(mutex_);
  if (state_ == kConnected) {
    connection_->Send(message);
  } else {
    LOG_ERROR("not connected yet! cann't send message now, try later.");
  }
}

void Client::Close() {
  boost::mutex::scoped_lock lock(mutex_);
  if (state_ == kDisConnected) {
    return;
  } else if (state_ == kConnecting) {
    connector_->Stop();
    connector_.reset();
  } else {
    connection_->Stop();
    connection_.reset();
  }
}

void Client::OnConnectCompleted(const Status& status,
                                const ConnectionPtr& connection) {
  boost::mutex::scoped_lock lock(mutex_);
  if (status.Ok()) {
    state_ = kConnected;
    connection_ = connection;
    connection_->Init(
        loop_,
        parser_,
        boost::bind(&Client::OnMessageReceived, shared_from_this(), _1, _2),
        boost::bind(&Client::OnConnectionException, shared_from_this(), _1,
                    _2));
  } else {
    state_ = kDisConnected;
  }
  connector_.reset();
  connect_call_back_(status);
}

void Client::OnMessageReceived(const MessagePtr& message,
                               const ConnectionPtr& connection) {
  message_received_call_back_(message);
}

void Client::OnConnectionException(const Status& status,
                                   const ConnectionPtr& connection) {
  boost::mutex::scoped_lock lock(mutex_);
  state_ = kDisConnected;
  connection_.reset();
  connection_exception_call_back_(status);
}

}

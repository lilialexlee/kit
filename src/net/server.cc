/*
 * server.cc
 *
 */

#include <signal.h>
#include "net/server.h"

namespace kit {

Server::Server()
    : loop_(),
      acceptor_(),
      parser_(),
      message_proc_(),
      process_message_in_order_(false),
      thread_pool_(),
      proc_thread_num_(0),
      max_queue_requests_(0) {
}

Server::~Server() {
}

void Server::Init(const MessageParserPtr& parser,
                  const MessageProcess& message_proc, int proc_thread_num,
                  int max_queue_requests, bool process_request_in_order) {
  parser_ = parser;
  message_proc_ = message_proc;
  process_message_in_order_ = process_request_in_order;
  proc_thread_num_ = proc_thread_num;
  max_queue_requests_ = max_queue_requests;
}

void Server::Start(const std::string& listen_ip, int listen_port) {
  signal(SIGPIPE, SIG_IGN);
  thread_pool_.Init(max_queue_requests_, proc_thread_num_);
  acceptor_.reset(
      new Acceptor(
          &loop_,
          boost::bind(&Server::OnNewConnection, shared_from_this(), _1)));
  acceptor_->ListenInLoop(listen_ip, listen_port);
  loop_.Loop();
}

void Server::Stop() {
  loop_.RunInLoopThread(boost::bind(&Server::StopInLoop, shared_from_this()));
}

void Server::OnNewConnection(const ServerConnectionPtr& new_conn) {
  new_conn->Init(
      &loop_, parser_,
      boost::bind(&Server::OnMessageReceived, shared_from_this(), _1, _2),
      boost::bind(&Server::OnConnectionException, shared_from_this(), _1, _2),
      process_message_in_order_);
}

void Server::OnMessageReceived(const MessagePtr& message,
                               const ServerConnectionPtr& connection) {
  int r = thread_pool_.ExecuteAsync(
      boost::bind(&Server::HandleRequest, shared_from_this(), message,
                  connection));
  if (r < 0) {
    connection->CloseInLoop();
  }
}

void Server::OnConnectionException(const Status& status,
                                   const ServerConnectionPtr& connection) {
}

void Server::HandleRequest(const MessagePtr& request,
                           const ServerConnectionPtr& connection) {
  MessagePtr reply = message_proc_(request);
  loop_.RunInLoopThread(
      boost::bind(&ServerConnection::SendReply, connection, reply));
}

void Server::StopInLoop() {
  acceptor_->Stop();
  thread_pool_.Stop();
  loop_.Stop();
}

}

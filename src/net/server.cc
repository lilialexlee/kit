/*
 * server.cc
 *
 */

#include "net/server.h"

namespace kit {

Server::Server(const MessageParserPtr& parser,
               const MessageProcess& message_proc, int proc_thread_num,
               int max_queue_requests)
    : loop_(),
      acceptor_(),
      conn_manager_(),
      parser_(parser),
      message_proc_(message_proc),
      thread_pool_(),
      proc_thread_num_(proc_thread_num),
      max_queue_requests_(max_queue_requests) {
}

Server::~Server() {
}

void Server::Start(const std::string& listen_ip, int listen_port) {
  thread_pool_.Init(max_queue_requests_, proc_thread_num_);
  acceptor_.reset(
      new Acceptor(
          &loop_,
          boost::bind(&Server::OnNewConnection, shared_from_this(), _1)));
  acceptor_->Listen(listen_ip, listen_port);
  loop_.Loop();
}

void Server::Stop() {
  loop_.RunInLoopThread(boost::bind(&Server::StopServer, shared_from_this()));
}

void Server::OnNewConnection(const ConnectionPtr& new_conn) {
  conn_manager_.Add(new_conn);
  LOG_INFO("new connection, socket: %d  %s", new_conn->Fd(),
           new_conn->GetUniqueIdentifier().c_str());
  new_conn->Init(
      &loop_, parser_,
      boost::bind(&Server::OnMessageReceived, shared_from_this(), _1, _2),
      boost::bind(&Server::OnConnectionException, shared_from_this(), _1, _2));
}

void Server::HandleRequest(const MessagePtr& request,
                           const ConnectionPtr& connection) {
  MessagePtr reply = message_proc_(request);
  connection->Send(reply);
}

void Server::OnMessageReceived(const MessagePtr& message,
                               const ConnectionPtr& connection) {
  int r = thread_pool_.ExecuteAsync(
      boost::bind(&Server::HandleRequest, shared_from_this(), message,
                  connection));
  if (r < 0) {
    connection->Stop();
    conn_manager_.Remove(connection);
  }
}

void Server::OnConnectionException(const Status& status,
                                   const ConnectionPtr& connection) {
  conn_manager_.Remove(connection);
}

void Server::StopServer() {
  acceptor_->Stop();
  conn_manager_.StopAll();
  thread_pool_.Stop();
  loop_.Stop();
}

}

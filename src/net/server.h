/*
 * server.h
 *
 */

#ifndef SRC_NET_SERVER_H_
#define SRC_NET_SERVER_H_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "net/message.h"
#include "net/event_loop.h"
#include "net/acceptor.h"
#include "net/connection.h"
#include "net/connection_manager.h"
#include "util/thread_pool.h"

namespace kit {

class Server : public boost::enable_shared_from_this<Server> {
 public:
  typedef boost::function<MessagePtr(const MessagePtr&)> MessageProcess;

  Server(const MessageParserPtr& parser, const MessageProcess& message_proc,
         int proc_thread_num, int max_queue_requests);
  ~Server();

  void Start(const std::string& listen_ip, int listen_port);

  void Stop();

 private:
  void OnNewConnection(const ConnectionPtr& new_conn);
  void OnMessageReceived(const MessagePtr& message,
                         const ConnectionPtr& connection);
  void OnConnectionException(const Status& status,
                             const ConnectionPtr& connection);

  void HandleRequest(const MessagePtr& request,
                     const ConnectionPtr& connection);

  void StopServer();

  EventLoop loop_;
  AcceptorPtr acceptor_;
  ConnectionManager conn_manager_;
  MessageParserPtr parser_;
  MessageProcess message_proc_;
  ThreadPool thread_pool_;
  int proc_thread_num_;
  int max_queue_requests_;

  Server(const Server&);
  void operator=(const Server&);
};

typedef boost::shared_ptr<Server> ServerPtr;

}

#endif /* SRC_NET_SERVER_H_ */

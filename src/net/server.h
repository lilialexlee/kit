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
#include "util/thread_pool.h"

namespace kit {

class Server : public boost::enable_shared_from_this<Server> {
 public:
  typedef boost::function<MessagePtr(const MessagePtr&)> MessageProcess;

  Server();
  ~Server();

  void Init(const MessageParserPtr& parser, const MessageProcess& message_proc,
            int proc_thread_num, int max_queue_requests,
            bool process_request_in_order = true,
            bool close_connection_after_process = false);

  void Start(const std::string& listen_ip, int listen_port);

  void Stop();

 private:
  void OnNewConnection(const ServerConnectionPtr& new_conn);
  void OnMessageReceived(const MessagePtr& message,
                         const ServerConnectionPtr& connection);
  void OnConnectionException(const Status& status,
                             const ServerConnectionPtr& connection);

  void HandleRequest(const MessagePtr& request,
                     const ServerConnectionPtr& connection);

  void StopInLoop();

  EventLoop loop_;
  AcceptorPtr acceptor_;
  MessageParserPtr parser_;
  MessageProcess message_proc_;
  bool process_message_in_order_;
  bool close_connection_after_process_;
  ThreadPool thread_pool_;
  int proc_thread_num_;
  int max_queue_requests_;

  Server(const Server&);
  void operator=(const Server&);
};

typedef boost::shared_ptr<Server> ServerPtr;

}

#endif /* SRC_NET_SERVER_H_ */

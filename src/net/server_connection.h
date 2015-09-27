/*
 * server_connection.h
 *
 */

#ifndef SRC_NET_SERVER_CONNECTION_H_
#define SRC_NET_SERVER_CONNECTION_H_

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include "net/connection.h"
#include "net/event_loop.h"
#include "net/status.h"

namespace kit {

class ServerConnection;

typedef boost::shared_ptr<ServerConnection> ServerConnectionPtr;

class ServerConnection : public boost::enable_shared_from_this<ServerConnection> {
 public:
  typedef boost::function<void(const MessagePtr&, const ServerConnectionPtr&)> MessageReceivedCallback;
  typedef boost::function<void(const Status&, const ServerConnectionPtr&)> ExceptionCallback;

  explicit ServerConnection(const SocketPtr& socket);
  ~ServerConnection();

  void Init(EventLoop* loop, const MessageParserPtr& parser,
            const MessageReceivedCallback& message_cb,
            const ExceptionCallback& excetion_cb,
            bool process_request_in_order);

  void Close();

  //should be invoked in loop thread
  void CloseInLoop();

  //should be invoked in loop thread
  void SendReply(const MessagePtr& reply);

  void ProcessRead();

  void ProcessWrite();

  void ProcessError();

 private:
  void StartInLoop();
  void EnableLoopWriteEvent();
  void DisableLoopWriteEvent();

  void OnMessageReceived(const MessagePtr& message);
  void OnPeerDisconnect();
  void OnException(const Status status);

  ConnectionPtr connection_;
  EventLoop* loop_;
  MessageReceivedCallback message_received_call_back_;
  ExceptionCallback exception_call_back_;
  bool process_request_in_order_;
  bool is_processing_;

  ServerConnection(const ServerConnection&);
  void operator=(const ServerConnection&);
};

typedef boost::shared_ptr<ServerConnection> ServerConnectionPtr;

}

#endif /* SRC_NET_SERVER_CONNECTION_H_ */

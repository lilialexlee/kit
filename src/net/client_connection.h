/*
 * client_connection.h
 *
 */

#ifndef SRC_NET_CLIENT_CONNECTION_H_
#define SRC_NET_CLIENT_CONNECTION_H_

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include "net/connection.h"
#include "net/event_loop.h"
#include "net/status.h"

namespace kit {

class ClientConnection : public boost::enable_shared_from_this<ClientConnection> {
 public:
  typedef boost::function<void(const MessagePtr&)> MessageReceivedCallback;
  typedef boost::function<void(const Status&)> ExceptionCallback;

  explicit ClientConnection(const SocketPtr& socket);
  ~ClientConnection();

  bool IsSelfConnect();

  void Init(EventLoop* loop, const MessageParserPtr& parser,
            const MessageReceivedCallback& message_cb,
            const ExceptionCallback& excetion_cb);

  void Close();

  void Send(const MessagePtr& message);

  void SendInLoop(const MessagePtr& message);

  void ProcessRead();

  void ProcessWrite();

  void ProcessError();

 private:
  void StartInLoop();
  void CloseInLoop();
  void EnableLoopWriteEvent();
  void DisableLoopWriteEvent();

  void OnMessageReceived(const MessagePtr& message);
  void OnPeerDisconnect();
  void OnException(const Status status);

  ConnectionPtr connection_;
  EventLoop* loop_;
  MessageReceivedCallback message_received_call_back_;
  ExceptionCallback exception_call_back_;

  ClientConnection(const ClientConnection&);
  void operator=(const ClientConnection&);
};

typedef boost::shared_ptr<ClientConnection> ClientConnectionPtr;

}

#endif /* SRC_NET_CLIENT_CONNECTION_H_ */

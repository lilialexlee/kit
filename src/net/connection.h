/*
 * connection.h
 *
 */

#ifndef SRC_NET_CONNECTION_H_
#define SRC_NET_CONNECTION_H_

#include <list>
#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/buffer.h"
#include "net/message.h"
#include "net/status.h"

namespace kit {

class Connection;

typedef boost::shared_ptr<Connection> ConnectionPtr;

class Connection : public boost::enable_shared_from_this<Connection> {
 public:
  typedef boost::function<void(const MessagePtr&, const ConnectionPtr&)> MessageReceivedCallback;
  typedef boost::function<void(const Status&, const ConnectionPtr&)> ExceptionCallback;

  explicit Connection(const SocketPtr& socket);
  ~Connection();

  int Fd();

  bool IsSelfConnect();

  void Init(EventLoop* loop, const MessageParserPtr& parser,
            const MessageReceivedCallback& message_cb,
            const ExceptionCallback& excetion_cb);

  void Send(const MessagePtr& message);

  void Stop();

  std::string GetUniqueIdentifier();

  void ProcessRead();
  void ProcessWrite();
  void ProcessError();

 private:
  void ConnectionStart();
  void ConnectionStop();
  void EnableLoopWriteEvent();
  void DisableLoopWriteEvent();

  void AppendMessageAndTrySend(const MessagePtr& message);
  int WriteOutBuffer();
  int Send();

  void OnPeerDisconnect();
  void OnException(const Status status);

  SocketPtr socket_;
  std::string local_ip_;
  int local_port_;
  std::string peer_ip_;
  int peer_port_;
  std::string identifier_;
  Buffer in_;
  Buffer out_;
  std::queue<MessagePtr> to_be_send_messages_;
  MessageParserPtr parser_;
  MessagePtr received_message_;
  EventLoop* loop_;
  MessageReceivedCallback message_received_call_back_;
  ExceptionCallback exception_call_back_;

  Connection(const Connection&);
  void operator=(const Connection&);
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

}

#endif /* SRC_NET_CONNECTION_H_ */

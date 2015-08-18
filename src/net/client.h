/*
 * client.h
 *
 */

#ifndef SRC_NET_CLIENT_H_
#define SRC_NET_CLIENT_H_

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include "net/message.h"
#include "net/event_loop.h"
#include "net/connector.h"
#include "net/connection.h"

namespace kit {

class Client;
typedef boost::shared_ptr<Client> ClientPtr;

class Client : public boost::enable_shared_from_this<Client> {
 public:
  enum State {
    kDisConnected,
    kConnecting,
    kConnected
  };

  typedef boost::function<void(const Status&)> ConnectCallback;
  typedef boost::function<void(const MessagePtr&)> MessageReceivedCallback;
  typedef boost::function<void(const Status&)> ConnectionExceptionCallback;

  Client(EventLoop* loop);
  ~Client();

  void Setup(const MessageParserPtr& parser,
             const MessageReceivedCallback& message_cb,
             const ConnectionExceptionCallback& conn_exception_cb);

  void Connect(const std::string& ip, int port, int timeout_ms,
               const ConnectCallback cb);

  void Send(const MessagePtr& message);

  void Close();

 private:
  void OnConnectCompleted(const Status& status,
                          const ConnectionPtr& connection);
  void OnMessageReceived(const MessagePtr& message,
                         const ConnectionPtr& connection);
  void OnConnectionException(const Status& status,
                             const ConnectionPtr& connection);

  EventLoop* loop_;
  boost::mutex mutex_;
  State state_;
  ConnectorPtr connector_;
  ConnectionPtr connection_;
  ConnectCallback connect_call_back_;
  MessageParserPtr parser_;
  MessageReceivedCallback message_received_call_back_;
  ConnectionExceptionCallback connection_exception_call_back_;

  Client(const Client&);
  void operator=(const Client&);
};

}

#endif /* SRC_NET_CLIENT_H_ */

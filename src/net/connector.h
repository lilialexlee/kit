/*
 q * connector.h
 *
 */

#ifndef SRC_NET_CONNECTOR_H_
#define SRC_NET_CONNECTOR_H_

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include "event_loop.h"
#include "net/socket.h"
#include "net/client_connection.h"
#include "net/status.h"

namespace kit {

class Connector : public boost::enable_shared_from_this<Connector> {
 public:
  typedef boost::function<void(const Status&, const ClientConnectionPtr&)> ConnectCallback;

  Connector(EventLoop* loop, const std::string& ip, int port, int timeout_ms,
            const ConnectCallback& cb);
  ~Connector();

  void Start();

  void Stop();

  void ProcessWrite();
  void ProcessError();

 private:
  void StartInLoop();

  void StopInLoop();

  void OnConnected();
  void OnConnectTimeout();
  void OnConnectError(const Status& status);

  EventLoop* loop_;
  std::string ip_;
  int port_;
  int timeout_ms_;
  SocketPtr socket_;
  bool is_connecting_;
  ConnectCallback call_back_;

  Connector(const Connector &);
  void operator=(const Connector &);
};

typedef boost::shared_ptr<Connector> ConnectorPtr;
}

#endif /* SRC_NET_CONNECTOR_H_ */

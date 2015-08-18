/*
 * acceptor.h
 *
 */

#ifndef SRC_NET_ACCEPTOR_H_
#define SRC_NET_ACCEPTOR_H_

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "net/socket.h"
#include "net/event_loop.h"
#include "net/connection.h"

namespace kit {

class Acceptor : public boost::enable_shared_from_this<Acceptor> {
 public:
  typedef boost::function<void(const ConnectionPtr&)> NewConnectionCallback;

  Acceptor(EventLoop* loop, const NewConnectionCallback& cb);
  ~Acceptor();

  void Listen(const std::string& ip, int port);

  void Stop();

  void ProcessRead();
  void ProcessError();

 private:
  void StartListen(const std::string& ip, int port);
  void StopListen();

  EventLoop* loop_;
  Socket listen_socket_;
  NewConnectionCallback new_connection_callback_;

  Acceptor(const Acceptor &);
  void operator=(const Acceptor &);
};

typedef boost::shared_ptr<Acceptor> AcceptorPtr;
}

#endif /* SRC_NET_ACCEPTOR_H_ */

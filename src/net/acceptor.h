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
#include "net/server_connection.h"

namespace kit {

class Acceptor : public boost::enable_shared_from_this<Acceptor> {
 public:
  typedef boost::function<void(const ServerConnectionPtr&)> NewAcceptedConnectionCallback;

  Acceptor(EventLoop* loop, const NewAcceptedConnectionCallback& cb);
  ~Acceptor();

  void Listen(const std::string& ip, int port);
  void Stop();
  //should be invoke in loop thread
  void ListenInLoop(const std::string& ip, int port);
  //should be invoke in loop thread
  void StopInLoop();

  void ProcessRead();
  void ProcessError();

 private:

  EventLoop* loop_;
  Socket listen_socket_;
  NewAcceptedConnectionCallback new_accepted_connection_call_back_;

  Acceptor(const Acceptor &);
  void operator=(const Acceptor &);
};

typedef boost::shared_ptr<Acceptor> AcceptorPtr;
}

#endif /* SRC_NET_ACCEPTOR_H_ */

/*
 * connection.h
 *
 */

#ifndef SRC_CONNECTION_H_
#define SRC_CONNECTION_H_

#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include "net/socket.h"
#include "net/buffer.h"
#include "net/message.h"

namespace kit {

class Connection {
 public:
  explicit Connection(const SocketPtr& socket);
  virtual ~Connection();

  int Fd();

  bool Ok();

  bool IsSelfConnect();

  std::string Identifier();

  SocketPtr Socket();

  int Close();

  void Init(const MessageParserPtr& parser);

  //return the number of bytes actually read
  int Read();

  //decode error, return -1
  //message incomplete, return 0
  //get message success, return 1
  int RetrieveMessage(MessagePtr& message);

  //return the number of bytes actually write
  int Write();

  //return the number of bytes to be write
  int Send(const MessagePtr& message);

  bool WriteBufferEmpty();

 private:
  SocketPtr socket_;

  std::string local_ip_;
  int local_port_;
  std::string peer_ip_;
  int peer_port_;
  std::string identifier_;

  Buffer in_;
  Buffer out_;

  MessagePtr received_message_;
  MessageParserPtr parser_;

  Connection(const Connection&);
  void operator=(const Connection&);
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

}

#endif /* SRC_CONNECTION_H_ */

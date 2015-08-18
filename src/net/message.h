/*
 * message.h
 *
 */

#ifndef SRC_NET_MESSAGE_H_
#define SRC_NET_MESSAGE_H_

#include <boost/shared_ptr.hpp>
#include "net/buffer.h"

namespace kit {

class Message {
 public:
  Message() {
  }
  virtual ~Message() {
  }

  virtual int Decode(Buffer *input) = 0;
  virtual void Encode(Buffer *output) = 0;
};

typedef boost::shared_ptr<Message> MessagePtr;

class MessageParser {
 public:
  MessageParser() {
  }

  virtual ~MessageParser() {
  }

  virtual int Decode(Buffer *input, MessagePtr& message) = 0;
  virtual void Encode(const MessagePtr& message, Buffer *output) = 0;
};

typedef boost::shared_ptr<MessageParser> MessageParserPtr;

}

#endif /* SRC_NET_MESSAGE_H_ */

/*
 * echo_message.h
 *
 */

#ifndef SRC_EXAMPLE_ECHO_ECHO_MESSAGE_H_
#define SRC_EXAMPLE_ECHO_ECHO_MESSAGE_H_

#include <string>
#include "net/message.h"
#include "net/buffer.h"

namespace echo {

class EchoMessage : public kit::Message {
 public:
  explicit EchoMessage(const std::string str)
      : str_(str) {
  }

  ~EchoMessage() {
  }

  int Decode(kit::Buffer *input) {
    str_.assign(input->PeekRead(), input->ReadableSize());
    input->MarkRead(input->ReadableSize());
    if (str_.empty()) {
      return 0;
    }
    return 1;
  }

  void Encode(kit::Buffer *output) {
    output->Write(str_.c_str(), str_.size());
  }

  std::string Str() {
    return str_;
  }
 private:
  std::string str_;
};

class EchoMessageParser : public kit::MessageParser {
  int Decode(kit::Buffer *input, kit::MessagePtr& message) {
    message.reset(new EchoMessage(""));
    return message->Decode(input);
  }

  void Encode(const kit::MessagePtr& message, kit::Buffer *output) {
    message->Encode(output);
  }
};

}

#endif /* SRC_EXAMPLE_ECHO_ECHO_MESSAGE_H_ */

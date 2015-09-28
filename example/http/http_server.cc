/*
 * http_server.cc
 *
 */

#include "example/http/request.h"
#include "example/http/reply.h"
#include "example/http/request_handler.h"
#include "net/server.h"

class HttpServerParser : public kit::MessageParser {
  int Decode(kit::Buffer *input, kit::MessagePtr& message) {
    if (!message) {
      message.reset(new http::Request);
    }
    return message->Decode(input);
  }

  void Encode(const kit::MessagePtr& message, kit::Buffer *output) {
    message->Encode(output);
  }
};

http::RequestHandler* g_handler;

kit::MessagePtr Proc(const kit::MessagePtr& request) {
  http::RequestPtr http_request = boost::dynamic_pointer_cast < http::Request
      > (request);
  http::ReplyPtr http_reply = g_handler->Handle(http_request);
  return boost::dynamic_pointer_cast < kit::Message > (http_reply);
}

int main(int argc, char* argv[]) {
  if (argc != 5) {
    printf(
        "Usage: http_server listen_port root_path proc_thread_num queue_max\n");
    printf("Like:  http_server 8080 ./WEB_ROOT 5 100\n");
    return 0;
  }
  int listen_port = atoi(argv[1]);
  g_handler = new http::RequestHandler(argv[2]);
  int proc_thread_num = atoi(argv[3]);
  int queue_max = atoi(argv[4]);
  kit::MessageParserPtr parser(new HttpServerParser);
  kit::ServerPtr server(new kit::Server());
  server->Init(parser, Proc, proc_thread_num, queue_max, true, true);
  server->Start("", listen_port);
  return 0;
}


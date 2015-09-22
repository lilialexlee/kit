/*
 * echo_server.cc
 *
 */

#include "example/echo/echo_message.h"
#include "net/server.h"

kit::MessagePtr Proc(const kit::MessagePtr& request) {
  return request;
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Usage: echo_server listen_port proc_thread_num queue_max\n");
    printf("Like:  echo_server 22222 3 100\n");
    return 0;
  }
  int listen_port = atoi(argv[1]);
  int proc_thread_num = atoi(argv[2]);
  int queue_max = atoi(argv[3]);
  kit::MessageParserPtr parser(new echo::EchoMessageParser());
  kit::ServerPtr server(
      new kit::Server(parser, Proc, proc_thread_num, queue_max));
  server->Start("", listen_port);
  return 0;
}


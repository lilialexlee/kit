/*
 * echo_client.cc
 *
 */

#include "example/echo/echo_message.h"
#include "net/client.h"

void OnConnectFinish(const kit::Status& status, const kit::ClientPtr& client) {
  if (!status.Ok()) {
    printf("connect failed: %s\n", status.Info().c_str());
    client->Close();
    return;
  }
  kit::MessagePtr message(new echo::EchoMessage("hello"));
  client->Send(message);
}

void OnConnectionException(const kit::Status& status,
                           const kit::ClientPtr& client) {
  printf("connection exception: %s\n", status.Info().c_str());
  client->Close();
}

void OnMessageReceived(const kit::MessagePtr& message,
                       const kit::ClientPtr& client) {
  boost::shared_ptr<echo::EchoMessage> received = boost::dynamic_pointer_cast
      < echo::EchoMessage > (message);
  printf("received message:: %s\n", received->Str().c_str());
  client->Close();
  exit(0);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: echo_client server_ip server_port\n");
    printf("Like:  echo_client 127.0.0.1 22222\n");
    return 0;
  }
  std::string server_ip = argv[1];
  int server_port = atoi(argv[2]);
  kit::EventLoop loop;
  kit::MessageParserPtr parser(new echo::EchoMessageParser());
  kit::ClientPtr client(new kit::Client(&loop));
  client->Init(parser, OnMessageReceived, OnConnectionException);
  client->Connect(server_ip, server_port, 100, OnConnectFinish);
  loop.Loop();
  return 0;
}


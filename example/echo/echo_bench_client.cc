/*
 * echo_bench_client.cc
 *
 */

#include <boost/atomic.hpp>
#include "example/echo/echo_message.h"
#include "net/client.h"

int g_total_send_count = 0;

int g_already_send = 0;

int g_already_received = 0;

struct timeval g_begin;
struct timeval g_end;

void OnConnectFinish(const kit::Status& status, const kit::ClientPtr& client) {
  if (!status.Ok()) {
    printf("connect failed: %s\n", status.Info().c_str());
    client->Close();
    return;
  }
  if (g_already_send++ < g_total_send_count) {
    kit::MessagePtr message(new echo::EchoMessage("hello"));
    client->Send(message);
  }
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
  if (received->Str() != "hello") {
    printf("received message:: %s, something wrong happen\n",
           received->Str().c_str());
  }
  if (++g_already_received == g_total_send_count) {
    gettimeofday(&g_end, NULL);
    double interval = (g_end.tv_sec - g_begin.tv_sec) * 1000
        + (g_end.tv_usec - g_begin.tv_usec) % 1000;
    printf("qps: %f\n", g_total_send_count / interval * 1000);
  }
  if (g_already_send++ < g_total_send_count) {
    kit::MessagePtr message(new echo::EchoMessage("hello"));
    client->Send(message);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 5) {
    printf(
        "Usage: echo_bench_client server_ip server_port client_num total_send_count\n");
    printf("Like:  echo_bench_client 127.0.0.1 22222 10 1000000\n");
    return 0;
  }
  std::string server_ip = argv[1];
  int server_port = atoi(argv[2]);
  int client_num = atoi(argv[3]);
  g_total_send_count = atoi(argv[4]);

  kit::EventLoop loop;
  kit::MessageParserPtr parser(new echo::EchoMessageParser());
  gettimeofday(&g_begin, NULL);
  kit::ClientPtr clients[client_num];
  for (int i = 0; i < client_num; ++i) {
    clients[i].reset(new kit::Client(&loop));
    clients[i]->Init(parser, OnMessageReceived, OnConnectionException);
    clients[i]->Connect(server_ip, server_port, 100, OnConnectFinish);
  }
  loop.Loop();
  return 0;
}


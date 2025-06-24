#include <catch2/catch_test_macros.hpp>
//==============================================================================
#include <chess/networking/message.h>
#include <chess/networking/server_interface.h>
#include <chess/networking/client_interface.h>
#include <chess/networking/queue.h>
//==============================================================================
enum class message_tag { A, B, C };

using chess::networking::queue;
using chess::networking::server_interface;
using chess::networking::client_interface;
//==============================================================================
TEST_CASE( "queue::enque, queue:dequeue" ) {
  using message = chess::networking::message<message_tag>;
  queue<message> q;

  q.enqueue_emplaced(message_tag::A);
  q.enqueue_emplaced(message_tag::B);
  q.enqueue(message{message_tag::C});
  REQUIRE(q.front().header.tag == message_tag::A);
  q.dequeue();
  REQUIRE(q.front().header.tag == message_tag::B);
  q.dequeue();
  REQUIRE(q.front().header.tag == message_tag::C);
}
//==============================================================================
TEST_CASE( "message" ) {
  using message = chess::networking::message<message_tag>;
  auto msgA = message{message_tag::A};
  msgA << 1.0f << 2.0;
  double r0; float r1;
  msgA >> r0 >> r1;
  REQUIRE(r0 == 2.0);
  REQUIRE(r1 == 1.0f);
}
//==============================================================================
TEST_CASE( "server-client" ) {
  using message = chess::networking::message<message_tag>;
  server_interface<message_tag> server{8080};
  auto server_start_success = server.start();
  REQUIRE(server_start_success);
  if (server_start_success)
  {
    client_interface<message_tag> client;
    client.connect("localhost", 8080);
    client.send(message{message_tag::A});
  }
}

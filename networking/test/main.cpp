#include <catch2/catch_test_macros.hpp>
//==============================================================================
#include <chess/networking/message.h>
#include <chess/networking/queue.h>
//==============================================================================
enum class test { A, B, C };

using chess::networking::queue;

TEST_CASE( "queue::enque, queue:dequeue" ) {
  // using message = chess::networking::message<test>;
  using message = chess::networking::message<test>;
  queue<message> q;

  q.enqueue(message{test::A});
  q.enqueue(message{test::B});
  q.enqueue(message{test::C});
  REQUIRE(q.front().header.tag == test::A);
  q.dequeue();
  REQUIRE(q.front().header.tag == test::B);
  q.dequeue();
  REQUIRE(q.front().header.tag == test::C);
}

TEST_CASE( "message" ) {
  // using message = chess::networking::message<test>;
  using message = chess::networking::message<test>;
  auto msgA = message{test::A};
  msgA << 1.0f << 2.0;
  double r0; float r1;
  msgA >>r0 >> r1;
  REQUIRE(r0 == 2.0);
  REQUIRE(r1 == 1.0f);
}

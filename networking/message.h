#pragma once

#include <cstdint>
#include <vector>

template <typename T>
struct message_header{
  T message{};
  std::uint32_t size = 0;
};

template <typename T>
struct message {
  message_header<T> header{};
  std::vector<std::uint8_t> body;
};

#pragma once
//==============================================================================
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <vector>
//==============================================================================
namespace chess::networking {
//==============================================================================
template <typename Tag>
struct message_header {
  Tag tag{};
  std::uint32_t size = 0;
};
//------------------------------------------------------------------------------
template <typename Tag, std::integral BodyValueType = std::uint8_t>
struct message {
  using this_t           = message<Tag, BodyValueType>;
  using body_value_t     = BodyValueType;
  using body_t           = std::vector<body_value_t>;
  using message_header_t = message_header<Tag>;
  //----------------------------------------------------------------------------
  message_header_t header{};
  body_t           body{};
  //----------------------------------------------------------------------------
  this_t& operator<<(auto &&data) {
    auto const ints_of_data = reinterpret_cast<body_value_t const*>(&data);
    auto const data_size    = sizeof(decltype(data)) / sizeof(body_value_t);
    std::copy(ints_of_data, ints_of_data + data_size, std::back_inserter(body));
    header.size += data_size;
    return *this;
  }
  //----------------------------------------------------------------------------
  this_t& operator>>(auto &data) {
    auto const ints_of_data = reinterpret_cast<body_value_t *>(&data);
    auto const data_size    = sizeof(decltype(data)) / sizeof(body_value_t);
    std::copy(end(body) - data_size, end(body), ints_of_data);
    body.resize(body.size() - data_size);
    header.size -= data_size;
    return *this;
  }
};
//==============================================================================
} // namespace chess::networking
//==============================================================================

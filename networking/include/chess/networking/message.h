#pragma once
//==============================================================================
#include <cstdint>
#include <memory>
#include <concepts>
#include <type_traits>
#include <vector>
//==============================================================================
namespace chess::networking {
//==============================================================================
template <typename Tag>
struct message_header {
  Tag tag{};
  std::uint32_t body_size = 0;
  constexpr size_t size() const { return sizeof(message_header<Tag>); }
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
  message()                                 = default;
  message(message const &other)             = default;
  message(message &&other) noexcept         = default;
  message& operator= (message const &other) = default;
  message& operator= (message &&other)      = default;
  //----------------------------------------------------------------------------
  message(Tag const tag) : header{tag} {}
  //----------------------------------------------------------------------------
  this_t& operator<<(auto &&data) {
    auto const ints_of_data = reinterpret_cast<body_value_t const*>(&data);
    auto const data_size    = sizeof(decltype(data)) / sizeof(body_value_t);
    std::copy(ints_of_data, ints_of_data + data_size, std::back_inserter(body));
    header.body_size += data_size;
    return *this;
  }
  //----------------------------------------------------------------------------
  this_t& operator>>(auto &data) /* requires (std::is_standard_layout_v<decltype(data)>) */ {
    auto const ints_of_data = reinterpret_cast<body_value_t *>(&data);
    auto const data_size    = sizeof(decltype(data)) / sizeof(body_value_t);
    std::copy(end(body) - data_size, end(body), ints_of_data);
    body.resize(body.size() - data_size);
    header.body_size -= data_size;
    return *this;
  }
  size_t size() const {
    return header.size() + body.size();
  }
};

template <typename MessageTag>
class connection;

template <typename MessageTag>
struct owned_message : message<MessageTag> {
  owned_message(std::shared_ptr<connection<MessageTag>> rem, message<MessageTag> const& msg)
    : message<MessageTag>{msg}
    , remote{rem}
  {}
  std::shared_ptr<connection<MessageTag>> remote = nullptr;
};
//==============================================================================
} // namespace chess::networking
//==============================================================================

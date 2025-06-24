#pragma once
//==============================================================================
#include <asio.hpp>
#include <iostream>
#include "connection.h"
//==============================================================================
namespace chess::networking {
//==============================================================================
template <typename MessageTag>
class client_interface {
 public:
  client_interface() = default;
  //----------------------------------------------------------------------------
  virtual ~client_interface() { disconnect(); }
  //----------------------------------------------------------------------------
  void connect(std::string const &host, std::uint16_t const port) {
    try {
      m_connection = std::make_unique<connection<MessageTag>>(
          connection<MessageTag>::owner::client,
          m_asio_context,
          asio::ip::tcp::socket{m_asio_context},
          m_messages_in);

      asio::ip::tcp::resolver resolver{m_asio_context};
      auto endpoints = resolver.resolve(host, std::to_string(port));

      m_connection->connect_to_server(endpoints);

      m_thread_context = std::thread{[this]{m_asio_context.run();}};
    }
    catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
  }
  //----------------------------------------------------------------------------
  void disconnect() {
    if (is_connected())
      m_connection->disconnect();

    m_asio_context.stop();
    if (m_thread_context.joinable())
      m_thread_context.join();

    m_connection.release();
  }
  //----------------------------------------------------------------------------
  bool is_connected() {
    if (m_connection)
      return m_connection->is_connected();
    return false;
  }
  //----------------------------------------------------------------------------
  void send(const message<MessageTag> &msg) {
    if (!is_connected())
      return;

    m_connection->send(msg);
  }
  //----------------------------------------------------------------------------
  auto& incoming() {
    return m_messages_in;
  }

 protected:
  asio::io_context m_asio_context;
  std::thread m_thread_context;
  std::unique_ptr<connection<MessageTag>> m_connection;
 private:
  queue<owned_message<MessageTag>> m_messages_in;
};
//==============================================================================
} // namespace chess:networking
//==============================================================================

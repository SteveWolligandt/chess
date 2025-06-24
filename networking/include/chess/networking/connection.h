#pragma once
//==============================================================================
#include "message.h"
#include "queue.h"
#include <asio.hpp>
#include <iostream>
//==============================================================================
namespace chess::networking {
//==============================================================================
template <typename MessageTag>
class connection : std::enable_shared_from_this<connection<MessageTag>> {
 public: 
  enum class owner {
    server,
    client
  };
  //----------------------------------------------------------------------------
  connection(
    owner parent,
    asio::io_context &asio_context,
    asio::ip::tcp::socket socket,
    queue<owned_message<MessageTag>> &messages_in)
      : m_asio_context{asio_context}
      , m_socket{std::move(socket)}
      , m_messages_in{messages_in} {}
  //----------------------------------------------------------------------------
  virtual ~connection() = default;
  //----------------------------------------------------------------------------
  void connect_to_client(uint32_t uid = 0) {
    if (m_owner_type == owner::server && m_socket.is_open()) {
      id = uid;
      read_header();
    }
  }
  //----------------------------------------------------------------------------
  void connect_to_server(asio::ip::tcp::resolver::results_type const &endpoints) {
    // Only clients can connect to servers
    if (m_owner_type == owner::client) {
      // Request asio attempts to connect to an endpoint
      asio::async_connect(
          m_socket, endpoints,
          [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
            if (!ec) {
              read_header();
            }
          });
    }
  }
  //----------------------------------------------------------------------------
  auto get_id() const {
    return id;
  }
  //----------------------------------------------------------------------------
  void disconnect() {
    if (is_connected())
      asio::post(m_asio_context, [this]() { m_socket.close(); });
  }
  //----------------------------------------------------------------------------
  bool is_connected() const {
    return m_socket.is_open();
  }
  //----------------------------------------------------------------------------
  void start_listening() {}
  //----------------------------------------------------------------------------
 public:
  // ASYNC - Send a message, connections are one-to-one so no need to specifiy
  // the target, for a client, the target is the server and vice versa
  void send(message<MessageTag> const &msg) {
    asio::post(m_asio_context, [this, msg]() {
      // If the queue has a message in it, then we must
      // assume that it is in the process of asynchronously being written.
      // Either way add the message to the queue to be output. If no messages
      // were available to be written, then start the process of writing the
      // message at the front of the queue.
      auto writing_msg = !m_messages_out.empty();
      m_messages_out.push_back(msg);
      if (!writing_msg) {
        write_header();
      }
    });
  }

 private:
  // ASYNC - Prime context to write a message header
  void write_header() {
    // If this function is called, we know the outgoing message queue must have
    // at least one message to send. So allocate a transmission buffer to hold
    // the message, and issue the work - asio, send these bytes
    asio::async_write(
        m_socket,
        asio::buffer(&m_messages_out.front().header, sizeof(message_header<MessageTag>)),
        [this](std::error_code ec, std::size_t length) {
          // asio has now sent the bytes - if there was a problem
          // an error would be available...
          if (!ec) {
            // ... no error, so check if the message header just sent also
            // has a message body...
            if (!m_messages_out.front().body.empty()) {
              // ...it does, so issue the task to write the body bytes
              write_body();

            } else {
              // ...it didnt, so we are done with this message. Remove it from
              // the outgoing message queue
              m_messages_out.pop_front();

              // If the queue is not empty, there are more messages to send, so
              // make this happen by issuing the task to send the next header.
              if (!m_messages_out.empty()) {
                write_header();
              }
            }
          } else {
            // ...asio failed to write the message, we could analyse why but
            // for now simply assume the connection has died by closing the
            // socket. When a future attempt to write to this client fails due
            // to the closed socket, it will be tidied up.
            std::cout << "[" << id << "] Write Header Fail.\n";
            m_socket.close();
          }
        });
  }

  // ASYNC - Prime context to write a message body
  void write_body() {
    // If this function is called, a header has just been sent, and that header
    // indicated a body existed for this message. Fill a transmission buffer
    // with the body data, and send it!
    asio::async_write(m_socket,
                      asio::buffer(m_messages_out.front().body.data(),
                                   m_messages_out.front().body.size()),
                      [this](std::error_code ec, std::size_t length) {
                        if (!ec) {
                          // Sending was successful, so we are done with the
                          // message and remove it from the queue
                          m_messages_out.pop_front();

                          // If the queue still has messages in it, then issue the
                          // task to send the next messages' header.
                          if (!m_messages_out.empty()) {
                            write_header();
                          }
                        } else {
                          // Sending failed, see WriteHeader() equivalent for
                          // description :P
                          std::cout << "[" << id << "] Write Body Fail.\n";
                          m_socket.close();
                        }
                      });
  }

  // ASYNC - Prime context ready to read a message header
  void read_header() {
    // If this function is called, we are expecting asio to wait until it receives
    // enough bytes to form a header of a message. We know the headers are a fixed
    // size, so allocate a transmission buffer large enough to store it. In fact,
    // we will construct the message in a "temporary" message object as it's
    // convenient to work with.
    asio::async_read(
        m_socket,
        asio::buffer(&m_msg_temp_in.header, sizeof(message_header<MessageTag>)),
        [this](std::error_code ec, std::size_t length) {
          if (!ec) {
            // A complete message header has been read, check if this message
            // has a body to follow...
            if (m_msg_temp_in.header.size() > 0) {
              // ...it does, so allocate enough space in the messages' body
              // vector, and issue asio with the task to read the body.
              m_msg_temp_in.body.resize(m_msg_temp_in.header.size());
              read_body();
            } else {
              // it doesn't, so add this bodyless message to the connections
              // incoming message queue
              add_to_incoming_message_queue();
            }
          } else {
            // Reading form the client went wrong, most likely a disconnect
            // has occurred. Close the socket and let the system tidy it up later.
            std::cout << "[" << id << "] Read Header Fail.\n";
            m_socket.close();
          }
        });
  }

  // ASYNC - Prime context ready to read a message body
  void read_body() {
    // If this function is called, a header has already been read, and that header
    // request we read a body, The space for that body has already been allocated
    // in the temporary message object, so just wait for the bytes to arrive...
    asio::async_read(
        m_socket,
        asio::buffer(m_msg_temp_in.body.data(), m_msg_temp_in.body.size()),
        [this](std::error_code ec, std::size_t length) {
          if (!ec) {
            // ...and they have! The message is now complete, so add
            // the whole message to incoming queue
            add_to_incoming_message_queue();
          } else {
            // As above!
            std::cout << "[" << id << "] Read Body Fail.\n";
            m_socket.close();
          }
        });
  }

  // Once a full message is received, add it to the incoming queue
  void add_to_incoming_message_queue() {
    // Shove it in queue, converting it to an "owned message", by initialising
    // with the a shared pointer from this connection object
    if (m_owner_type == owner::server)
      m_messages_in.enqueue_emplaced(this->shared_from_this(), m_msg_temp_in);
    else
      m_messages_in.enqueue_emplaced(nullptr, m_msg_temp_in);

    // We must now prime the asio context to receive the next message. It
    // wil just sit and wait for bytes to arrive, and the message construction
    // process repeats itself. Clever huh?
    read_header();
  }

 protected:
  asio::ip::tcp::socket             m_socket;
  asio::io_context                 &m_asio_context;
  queue<message<MessageTag>>        m_messages_out;
  queue<owned_message<MessageTag>> &m_messages_in;
  message<MessageTag>               m_msg_temp_in;
  owner                             m_owner_type = owner::server;
  std::uint32_t                     id           = 0;
};
//==============================================================================
} // namespace chess::networking
//==============================================================================

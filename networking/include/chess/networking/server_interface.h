#pragma once
//==============================================================================
#include "queue.h"
#include "message.h"
#include "connection.h"
#include "message_observer.h"
//==============================================================================
namespace chess::networking
{
  template<typename MessageTag>
  class server_interface : public message_observer<MessageTag>
  {
  public:
    // Create a server, ready to listen on specified port
    server_interface(uint16_t port)
      : m_asio_acceptor(m_asio_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {

    }
    //----------------------------------------------------------------------------
    virtual ~server_interface()
    {
      stop();
    }
    //----------------------------------------------------------------------------
    bool start() {
      try
      {
        // Issue a task to the asio context - This is important
        // as it will prime the context with "work", and stop it
        // from exiting immediately. Since this is a server, we 
        // want it primed ready to handle clients trying to
        // connect.
        wait_for_client_connection();

        // Launch the asio context in its own thread
        m_thread_context = std::thread([this]() { m_asio_context.run(); });
      }
      catch (std::exception& e)
      {
        // Something prohibited the server from listening
        std::cerr << "[SERVER] Exception: " << e.what() << "\n";
        return false;
      }

      std::cout << "[SERVER] Started!\n";
      return true;
    }
    //----------------------------------------------------------------------------
    void stop() {
      // Request the context to close
      m_asio_context.stop();

      // Tidy up the context thread
      if (m_thread_context.joinable()) m_thread_context.join();

      // Inform someone, anybody, if they care...
      std::cout << "[SERVER] Stopped!\n";
    }
    //----------------------------------------------------------------------------
    // ASYNC - Instruct asio to wait for connection
    void wait_for_client_connection() {
      // Prime context with an instruction to wait until a socket connects. This
      // is the purpose of an "acceptor" object. It will provide a unique socket
      // for each incoming connection attempt
      m_asio_acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket)
        {
          // Triggered by incoming connection request
          if (!ec)
          {
            // Display some useful(?) information
            std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

            // Create a new connection to handle this client 
            std::shared_ptr<connection<MessageTag>> newconn = 
              std::make_shared<connection<MessageTag>>(connection<MessageTag>::owner::server, 
                m_asio_context, std::move(socket), m_messages_in);
            
            

            // Give the user server a chance to deny connection
            if (on_client_connect(newconn))
            {								
              // Connection allowed, so add to container of new connections
              m_connections.push_back(std::move(newconn));

              // And very important! Issue a task to the connection's
              // asio context to sit and wait for bytes to arrive!
              m_connections.back()->connect_to_client(n_id_counter++);

              std::cout << "[" << m_connections.back()->get_id() << "] Connection Approved\n";
            }
            else
            {
              std::cout << "[-----] Connection Denied\n";

              // Connection will go out of scope with no pending tasks, so will
              // get destroyed automagically due to the wonder of smart pointers
            }
          }
          else
          {
            // Error has occurred during acceptance
            std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
          }

          // Prime the asio context with more work - again simply wait for
          // another connection...
          wait_for_client_connection();
        });
    }

    // Send a message to a specific client
    void message_client(std::shared_ptr<connection<MessageTag>> client, const message<MessageTag>& msg) {
      // Check client is legitimate...
      if (client && client->IsConnected())
      {
        // ...and post the message via the connection
        client->Send(msg);
      }
      else
      {
        // If we cant communicate with client then we may as 
        // well remove the client - let the server know, it may
        // be tracking it somehow
        OnClientDisconnect(client);

        // Off you go now, bye bye!
        client.reset();

        // Then physically remove it from the container
        m_connections.erase(
          std::ranges::remove(m_connections, client), end(m_connections));
      }
    }
    
    // Send message to all clients
    void message_all_clients(
        const message<MessageTag>& msg,
        std::shared_ptr<connection<MessageTag>> pIgnoreClient = nullptr) {
      bool bInvalidClientExists = false;

      // Iterate through all clients in container
      for (auto& client : m_connections)
      {
        // Check client is connected...
        if (client && client->IsConnected())
        {
          // ..it is!
          if(client != pIgnoreClient)
            client->Send(msg);
        }
        else
        {
          // The client couldnt be contacted, so assume it has
          // disconnected.
          OnClientDisconnect(client);
          client.reset();

          // Set this flag to then remove dead clients from container
          bInvalidClientExists = true;
        }
      }

      // Remove dead clients, all in one go - this way, we dont invalidate the
      // container as we iterated through it.
      if (bInvalidClientExists)
        m_connections.erase(
          std::ranges::remove(m_connections, nullptr), end(m_connections));
    }

    // Force server to respond to incoming messages
    void update(size_t nMaxMessages = -1, bool bWait = false) {
      if (bWait) m_messages_in.wait();

      // Process as many messages as you can up to the value
      // specified
      size_t nMessageCount = 0;
      while (nMessageCount < nMaxMessages && !m_messages_in.empty())
      {
        // Grab the front message
        auto msg = m_messages_in.pop_front();

        // Pass to message handler
        OnMessage(msg.remote, msg.msg);

        nMessageCount++;
      }
    }

  protected:
    // This server class should override thse functions to implement
    // customised functionality

    // Called when a client connects, you can veto the connection by returning false
    virtual bool on_client_connect(std::shared_ptr<connection<MessageTag>> client) {
      return false;
    }

    // Called when a client appears to have disconnected
    virtual void on_client_disconnect(std::shared_ptr<connection<MessageTag>> client) {

    }

    // Called when a message arrives
    virtual void on_message(std::shared_ptr<connection<MessageTag>> client, message<MessageTag>& msg) {
      notify_observers(client, msg);
    }


  protected:
    // Thread Safe Queue for incoming message packets
    queue<owned_message<MessageTag>> m_messages_in;

    // Container of active validated connections
    std::deque<std::shared_ptr<connection<MessageTag>>> m_connections;

    // Order of declaration is important - it is also the order of initialisation
    asio::io_context m_asio_context;
    std::thread m_thread_context;

    // These things need an asio context
    asio::ip::tcp::acceptor m_asio_acceptor; // Handles new incoming connection attempts...

    // Clients will be identified in the "wider system" via an ID
    uint32_t n_id_counter = 10000;
  };
//==============================================================================
} // namespace chess::networking
//==============================================================================


#pragma once

#include <set>
#include "connection.h"
//==============================================================================
namespace chess::networking {
//==============================================================================
template <typename MessageTag>
class message_observer {
  virtual void on_message(std::shared_ptr<connection<MessageTag>> client, message<MessageTag>& msg) = 0;
};
//==============================================================================
template <typename MessageTag>
class message_notifier {
  void notify_observers(std::shared_ptr<connection<MessageTag>> conn, message<MessageTag>& msg) {
    for (auto &obs : m_observers) {
      obs->ob_message(conn, msg);
    }
  }

  void add_observer(message_observer<MessageTag> &obs) {
    m_observers.insert(&obs);
  }

  void remove_observer(message_observer<MessageTag> &obs) {
    m_observers.erase(&obs);
  }

 private:
  std::set<message_observer<MessageTag>*> m_observers;
};
//==============================================================================
} // namespace chess::networking
//==============================================================================

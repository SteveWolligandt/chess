#include <deque>
#include <mutex>
//==============================================================================
namespace chess::networking {
//==============================================================================
template <typename T>
class queue {
  std::deque<T> m_data;
  std::mutex    m_mutex;

 public:
  queue() = default;

  ~queue() {
    clear();
  }

 private:
  void push_back(T item) {
    std::scoped_lock l{m_mutex};
    m_data.push_back(std::move(item));
  }

  void pop_back() {
    std::scoped_lock l{m_mutex};
    m_data.pop_back();
  }

  void push_front(T item) {
    std::scoped_lock l{m_mutex};
    m_data.push_front(std::move(item));
  }

  void pop_front() {
    std::scoped_lock l{m_mutex};
    m_data.pop_front();
  }

 public:
  void dequeue() {
    pop_front();
  }

  void enqueue(T item) {
    push_back(std::move(item));
  }

  T const& back() {
    std::scoped_lock l{m_mutex};
    return m_data.back();
  }

  T const& front() {
    std::scoped_lock l{m_mutex};
    return m_data.front();
  }
  
  void clear() {
    std::scoped_lock l{m_mutex};
    m_data.clear();
  }
};
//==============================================================================
} // namespace chess::networking
//==============================================================================

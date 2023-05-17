#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>

class SpinLock {
  std::atomic_flag m_lock = ATOMIC_FLAG_INIT;

  public:

  void lock( ){
    while(m_lock.test_and_set(std::memory_order_acquire));
  }

  void unlock( ){
    m_lock.clear( std::memory_order_release);
  }

};


#endif //SPIN_LOCK_H

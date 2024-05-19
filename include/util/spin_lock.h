#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>
#include <thread>

class SpinLock {
  std::atomic<bool> m_lock{false};

 public:
  void lock() {
    // check if locked read only, if not locked, then try to lock
    // there's a small window b/w the first and the second check which can lead
    // to 2 failures
    for (unsigned short i = 0; m_lock.load(std::memory_order_relaxed) ||
                               m_lock.exchange(true, std::memory_order_acquire);
         ++i) {
      // acquiring lock by this trhead depends on releasing lock on another
      // thread the idea of this part is to let the other thread get scheduled
      // to unlock
      if (i & 8) {  // 8 is a random no. needs benchmarking
        i = 0;
        std::this_thread::yield();  // this could be a sleep statemen. again,
                                    // benchmark!!
      }
    }
  }

  void unlock() { m_lock.store(false, std::memory_order_release); }
};

#endif  // SPIN_LOCK_H

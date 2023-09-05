#ifndef SRC_COMPONENTS_INCLUDE_UTILS_LOCK_H_
#define SRC_COMPONENTS_INCLUDE_UTILS_LOCK_H_

#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <macro.h>
#include "atomic.h"

class SpinMutex {
 public:
  SpinMutex() : state_(0) {}
  void Lock() {
    // Comment below add exception for lint error
    // Reason: FlexeLint doesn't know about compiler's built-in instructions
    /*lint -e1055*/
    if (atomic_post_set(&state_) == 0) {
      return;
    }
    for (;;) {
      sched_yield();
      /*lint -e1055*/
      if (state_ == 0 && atomic_post_set(&state_) == 0) {
        return;
      }
    }
  }
  void Unlock() {
    state_ = 0;
  }
  ~SpinMutex() {}

 private:
  volatile unsigned int state_;
};

class Lock {
 public:
  Lock();
  Lock(bool is_recursive);
  ~Lock();

  bool IsLocked();
  // Ackquire the lock. Must be called only once on a thread.
  // Please consider using AutoLock to capture it.
  void Acquire();
  // Release the lock. Must be called only once on a thread after lock.
  // was acquired. Please consider using AutoLock to automatically release
  // the lock
  void Release();
  // Try if lock can be captured and lock it if it was possible.
  // If it captured, lock must be manually released calling to Release
  // when protected resource access was finished.
  // @returns wether lock was captured.
  bool Try();

 private:
  pthread_mutex_t mutex_;


#ifndef NDEBUG
  /**
  * @brief Basic debugging aid, a flag that signals wether this lock is
  * currently taken
  * Allows detection of abandoned and recursively captured mutexes
  */
  uint32_t lock_taken_;

  /**
  * @brief Describe if mutex is recurcive or not
  */
  bool is_mutex_recursive_;

  void AssertFreeAndMarkTaken();
  void AssertTakenAndMarkFree();
#else
  void AssertFreeAndMarkTaken() {}
  void AssertTakenAndMarkFree() {}
#endif

  void Init(bool is_recursive);

  friend class ConditionalVariable;
  DISALLOW_COPY_AND_ASSIGN(Lock);
};

// This class is used to automatically acquire and release the a lock
class AutoLock {
 public:
  explicit AutoLock(Lock& lock) : lock_(lock) {
    lock_.Acquire();
  }
  ~AutoLock() {
    lock_.Release();
  }

 private:
  Lock& GetLock() {
    return lock_;
  }
  Lock& lock_;

 private:
  friend class AutoUnlock;
  friend class ConditionalVariable;
  DISALLOW_COPY_AND_ASSIGN(AutoLock);
};

// This class is used to temporarly unlock autolocked lock
class AutoUnlock {
 public:
  explicit AutoUnlock(Lock& lock) : lock_(lock) {
    lock_.Release();
  }
  explicit AutoUnlock(AutoLock& lock) : lock_(lock.GetLock()) {
    lock_.Release();
  }
  ~AutoUnlock() {
    lock_.Acquire();
  }

 private:
  Lock& lock_;

 private:
  DISALLOW_COPY_AND_ASSIGN(AutoUnlock);
};
#endif  // SRC_COMPONENTS_INCLUDE_UTILS_LOCK_H_

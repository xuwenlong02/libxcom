#include "lock.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <logger.h>


Lock::Lock()
#ifndef NDEBUG
    : lock_taken_(0)
    , is_mutex_recursive_(false)
#endif  // NDEBUG
{
  Init(false);
}

Lock::Lock(bool is_recursive)
#ifndef NDEBUG
    : lock_taken_(0)
    , is_mutex_recursive_(is_recursive)
#endif  // NDEBUG
{
  Init(is_recursive);
}

Lock::~Lock() {
#ifndef NDEBUG
  if (lock_taken_ > 0) {
    LOG_ERR("Destroying non-released mutex %p", &mutex_);
  }
#endif
  int32_t status = pthread_mutex_destroy(&mutex_);

  if (status != 0) {
    LOG_ERR("Failed to destroy mutex %p:%s", &mutex_,strerror(status));
  }

}

bool Lock::IsLocked()
{
    return lock_taken_>0;
}

void Lock::Acquire() {
  const int32_t status = pthread_mutex_lock(&mutex_);
  if (status != 0) {
    LOG_ERR("Failed to acquire mutex %p:%s", &mutex_ ,strerror(status));
    NOTREACHED();
  } else {
    AssertFreeAndMarkTaken();
  }
}

void Lock::Release() {
  AssertTakenAndMarkFree();
  const int32_t status = pthread_mutex_unlock(&mutex_);
  if (status != 0) {
    LOG_ERR("Failed to unlock mutex %p :%s" , &mutex_ ,strerror(status));
  }
}

bool Lock::Try() {
  const int32_t status = pthread_mutex_trylock(&mutex_);
  if (status == 0) {
#ifndef NDEBUG
    lock_taken_++;
#endif
    return true;
  }
  return false;
}

#ifndef NDEBUG
void Lock::AssertFreeAndMarkTaken() {
  if ((lock_taken_ > 0) && !is_mutex_recursive_) {
    LOG_ERR("Locking already taken not recursive mutex");
    NOTREACHED();
  }
  lock_taken_++;
}

void Lock::AssertTakenAndMarkFree() {
  if (lock_taken_ == 0) {
    LOG_ERR("Unlocking a mutex that is not taken");
    NOTREACHED();
  }
  lock_taken_--;
}
#endif

void Lock::Init(bool is_recursive) {

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);

  const int32_t mutex_type =
      is_recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK;

  pthread_mutexattr_settype(&attr, mutex_type);
  const int32_t status = pthread_mutex_init(&mutex_, &attr);
  pthread_mutexattr_destroy(&attr);
  if (status != 0) {
    LOG_ERR("Failed to initialize mutex. %s", strerror(status));
    DCHECK(status != 0);
  }
}

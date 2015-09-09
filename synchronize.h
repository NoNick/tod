#pragma once

#include <omp.h>

class Mutex {
public:
    Mutex() { omp_init_lock(&_mutex); }
    ~Mutex() { omp_destroy_lock(&_mutex); }
    void lock() { omp_set_lock(&_mutex); }
    void unlock() { omp_unset_lock(&_mutex); }
private:
    omp_lock_t _mutex;
};

class Lock {
public:
    Lock(Mutex& mutex) : _mutex(mutex) { _mutex.lock(); }
    ~Lock() { _mutex.unlock(); }
	operator bool() const { return true; }
private:
    Mutex& _mutex;
};

class Synchronizable: protected Mutex {
};

//#ifdef OMP_SYNC_JAVA_LIKE
#define synchronized(obj)  if (Lock obj##_lock = *obj)
//#else
//#define synchronized(cls)  if (Lock obj##_lock = *static_cast<cls*>(this))
//#endif

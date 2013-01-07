/*
   $Id: Concurrency.h 79 2007-08-18 17:30:26Z hkoelman $

   Set of helpers to handle concurrency
 */

/*
 * Concurrency
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * Concurrency is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Concurrency is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Concurrency; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <pthread.h>
#include <string>
#include <exception>

using namespace std;

#ifndef __CONCURRENCY__
#define __CONCURRENCY__

#include <queue>

class ConcurrencyException : public exception {
  public:

    ConcurrencyException ( string message = NULL ) : msg ("ConcurrencyException") {
      msg = message;
    }

    ConcurrencyException ( const char *message ) {
      msg = message;
    }

    ~ConcurrencyException () throw (){
    };


    virtual inline const char *what () const throw () {
      return msg.c_str ();
    }

  protected:
    string msg;
};

class Mutex {
  public:
    /**
       The mutex object is locked by calling pthread_mutex_lock. If
       the mutex is already locked, the calling thread blocks until the mutex becomes
       available. This operation returns with the mutex object referenced by mutex in
       the locked state with the calling thread as its owner.
     */
    int lock ();

    /**
       The function pthread_mutex_trylock is identical to pthread_mutex_lock except that
       if the mutex object referenced by mutex is currently locked (by any thread,
       including the current thread), the call returns immediately.
     */
    int tryLock ();

    /**
       The pthread_mutex_unlock function releases the mutex object referenced by mutex.
       The manner in which a mutex is released is dependent upon the mutex's type
       attribute. If there are threads blocked on the mutex object referenced by mutex
       when pthread_mutex_unlock is called, resulting in the mutex becoming available,
       the scheduling policy is used to determine which thread shall acquire the mutex.
       (In the case of PTHREAD_MUTEX_RECURSIVE mutexes, the mutex becomes available when
       the count reaches zero and the calling thread no longer has any locks on this
       mutex).
     */
    int unlock ();

    /*
       Constructor/Desctructor
     */
    Mutex ();
    ~Mutex ();

  protected:
    pthread_mutex_t mutex;
};

class Condition : public Mutex {
  public:
    /*
       The wait() and wait( int millis )g functions are used to block
       on a condition variable. They are called with mutex locked by the calling thread
       or undefined behaviour will result.
       These functions atomically release mutex and cause the calling thread to block on
       the condition variable cond; atomically here means "atomically with respect to access
       by another thread to the mutex and then the condition variable". That is, if another
       thread is able to acquire the mutex after the about-to-block thread has released it,
       then a subsequent call to pthread_cond_signal() or pthread_cond_broadcast() in that
       thread behaves as if it were issued after the about-to-block thread has blocked.

       Upon successful return, the mutex has been locked and is owned by the calling thread.

       When using condition variables there is always a boolean predicate involving shared variables
       associated with each condition wait that is true if the thread should proceed. Spurious wakeups
       from the wait() or wait( int millis )g functions may occur. Since the return
       from wait() or wait( int millis )g does not imply anything about the value
       of this predicate, the predicate should be re-evaluated upon such return.

       The effect of using more than one mutex for concurrent wait() or
       wait( int millis )g operations on the same condition variable is undefined; that is, a
       condition variable becomes bound to a unique mutex when a thread waits on the condition variable,
       and this (dynamic) binding ends when the wait returns.
     */
//  int wait () ;
    int simple_wait();
    int wait ( int millis = 0);
    int waitAndLock ( int millis  = 0 );

    /*
       The pthread_cond_signal() call unblocks at least one of the threads that are blocked
       on the specified condition variable cond (if any threads are blocked on cond).
     */
    int signal ();

    /*
       The pthread_cond_broadcast() call unblocks all threads currently blocked on the specified condition variable cond.
     */
    int broadcast ();

    // properties ------------------------------------------------------------

    // constructor/destructor ------------------------------------------------

    Condition ();
    ~Condition();

  private:
    pthread_cond_t cond;
};

class ScopedMutex {
/*
   This class was designed to encapsulate a mutex and automatically control the lock attribute.
   The ScopedMutex lock the associated mutex once we instanciate the class and the lock is automatically unlocked
   once the object is destroyed. This allow us to correlate the lock with the scope of the object.
 */
  private:
    Mutex *mutex;

  public:
/*
   constructors and destructors. The constructor is forced to only accept a mutex object or any object of a subclass.
 */
    explicit ScopedMutex(Mutex &m) : mutex(&m) {
      mutex->lock();
    }
    ~ScopedMutex() {
      mutex->unlock();
    }

/*
   Desabling the = operator.
 */
    void operator= (ScopedMutex &);
};

/**
 * The pipe class is a one-way communication channel between Threads. It consists of a fifo queue wich stocks the messages sent.
 * And allow to retrieve the oldest message stocked one message at a time. The condtion objet c_pipe allow us to synchronized the read/write of a message.
 * One operation can not be done if another is not completely terminated.
 */
template<class T> class Pipe {
  private:
    Condition c_pipe;
    std::queue<T *> msg_list;

  public:
/**
 * Cosntructors and destructors
 */
    Pipe();
    ~Pipe();

/**
 * We can send a message by using the push function. this will stock it at the end of the queue.
 *
 * @param msg message to queue.
 */
    void push(T*);
/**
 * We then can retrieve the oldest message using the pop function. This function will remove the oldest message from the queue and
 * send it to the receiver.
 *
 *  @return message queued.
 */
    T* pop();

    /**
     * Get the number of message in the queue.
     *
     * @return size of queue
     */
    int size();

/**
 * Desabling the = operator.
 *
 */
    void operator= (Pipe &);
};

#endif

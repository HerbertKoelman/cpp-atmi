/* $Id: Thread.h 79 2007-08-18 17:30:26Z hkoelman $

   A POSIX based thread class.
 */

/*
 * Copyright (C) 2006 - herbert koelman
 *
 * Thread is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Thread is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Thread; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <pthread.h>
#include <Concurrency.h>
#include <vector>

#ifndef __THREAD__
#define __THREAD__

using namespace std;

/**
 * Interface of a runnable class.
 *
 * Yiou can write code to be run through a Thread by implementing this interface.
 */
class Runnable {
  private:

  public:
    /**
     * This method must be overriden
     */
    virtual void run () = 0;
};

/**
 * Handles POSIX (Portable Operating System Interface) threads. 
 *
 * <code>
 *     class ReaderThread: public Thread {
 *     public:
 *       void run() {...}
 *     };
 *
 *     ReaderThread rt;
 *     rt.start();
 * </code>
 */
class Thread : public Runnable {
  public:
    // construct/copy/destruct

    /**
     * create a new thread
     *
     * @param destroy indicates that the thread should delete itself when ended.
     */
    Thread( int destroy  = 0 );

    /**
     * cleanup thread ressources.
     */
    ~Thread();

    /**
       Override this operation with the code that the POSIX thread should run. It is
       actually called by the extern "C" function ThreadStartup which then calls the
       thread's run operation.
     */
    virtual void run ();

    /**
       The start subroutine creates a new thread and initializes its attributes
       using the thread attributes object specified by the attr parameter. The new
       thread inherits its creating thread's signal mask; but any pending signal of the
       creating thread will be cleared for the new thread.

       The new thread is made runnable, and will start executing the run routine, with.

       After thread creation, the thread attributes object can be reused to create
       another thread, or deleted.

       The thread terminates in the following cases:

       o The thread returned from its starting routine (the main routine for the initial thread)
       o The thread called the pthread_exit subroutine
       o The thread was canceled
       o The thread received a signal that terminated it
       o The entire process is terminated due to a call to either the exec or exit subroutines.

       Note: The pthread.h header file must be the first included file of each
       source file using the threads library. Otherwise, the -D_THREAD_SAFE
       compilation flag should be used, or the cc_r compiler used. In this case,
       the flag is automatically set.

       When multiple threads are created in a process, the FULL_CORE flag is set for all
       signals. This means that if a core file is produced, it will be much bigger than
       a single_threaded application. This is necessary to debug multiple-threaded
       processes.
     */
    int start ();

    /**
       The join subroutine blocks the calling thread until the thread thread
       terminates. The target thread's termination status is returned in the status
       parameter.

       If the target thread is already terminated, but not yet detached, the subroutine
       returns immediately. It is impossible to join a detached thread, even if it is
       not yet terminated. The target thread is automatically detached after all joined
       threads have been woken up.

       This subroutine does not itself cause a thread to be terminated. It acts like the
       pthread_cond_wait subroutine to wait for a special condition.
     */
    int join();

    /**
       The cancel subroutine requests the cancellation of the thread thread. The
       action depends on the cancelability of the target thread:

       o If its cancelability is disabled, the cancellation request is set pending.
       o If its cancelability is deferred, the cancellation request is set pending till the thread reaches a cancellation point.
       o If its cancelability is asynchronous, the cancellation request is acted upon immediately; in some cases, it may result in unexpected behaviour.

       The cancellation of a thread terminates it safely, using the same termination
       procedure as the pthread_exit subroutine.
     */
    int cancel();

    inline int getStatus () {
      return status;
    };

    inline int destroy_when_ended () {
      return destroy;
    };

  private:
    pthread_t thread;
    pthread_attr_t attr;

    int status;
    int destroy;
};

#endif

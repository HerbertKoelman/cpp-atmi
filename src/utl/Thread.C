/* $Id: Thread.C 28 2007-08-06 22:29:28Z hkoelman $
 */
#include "Thread.h"
#include <pthread.h>

extern "C" void * ThreadStartup (void *);

/*
   This function is a helper function. It has normal C linkage, and is
   the base for newly created Thread objects. It runs the
   run method on the thread object passed to it (as a void *).
 */
void *ThreadStartup(void *args) {
  Thread *thread = (Thread *)args;
  thread->run();
  if ( thread->destroy_when_ended () ) delete thread;
  return (NULL);
}

Thread::Thread ( int destroy ) {
  this->destroy = destroy;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

Thread::~Thread () {
  pthread_attr_destroy(&attr);
}

void Thread::run () {
}

int Thread::start () {

  int rc = 0;

  rc = pthread_create(&thread, &attr, ThreadStartup, (void *) this);

  return rc;
}

int Thread::join () {
  int rc = 0;

  rc = pthread_join(thread, (void **)&status);

  return rc;
}

int Thread::cancel () {
  int rc = 0;

  rc = pthread_cancel ( thread );

  return rc;
}

/*
   $Id: Condition.C 28 2007-08-06 22:29:28Z hkoelman $
 */
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include <Concurrency.h>

Condition::Condition () {
  pthread_cond_init ( &cond, NULL );
}

Condition::~Condition () {
}

int Condition::simple_wait() {
  int rc = 0;

  rc  = pthread_cond_wait (&cond, &mutex);

  return rc;
}

/*
   int Condition::wait () {
   int rc = 0 ;

   lock () ;
   rc  = pthread_cond_wait ( &cond, &mutex );

   return rc ;
   }
 */

/* Default millis is 0 */
int Condition::wait ( int millis ) {
  int rc = 0;

  rc = waitAndLock ( millis );

  if ( rc == 0 || rc == ETIMEDOUT ) rc = unlock ();

  return rc;
}

/* Default millis is 0 */
int Condition::waitAndLock ( int millis ) {
  int rc = 0;
  timeval current;
  timespec timeout;

  gettimeofday ( &current, NULL );
  timeout.tv_sec = current.tv_sec + (millis * 0.001);

  rc = lock ();
  if ( rc == 0 ) {
    if ( millis == 0 ) {
      rc  = pthread_cond_wait ( &cond, &mutex );
    } else {
      rc  = pthread_cond_timedwait ( &cond, &mutex, &timeout );
    }
  }

  return rc;
}

int Condition::signal() {
  int rc = 0;

  rc = pthread_cond_signal ( &cond );

  return rc;
}

int Condition::broadcast () {

  int rc = 0;

  rc = pthread_cond_broadcast ( &cond );

  return rc;
}

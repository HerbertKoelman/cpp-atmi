/* $Id: Mutex.C 78 2007-08-18 17:18:18Z hkoelman $
 */
#include <stdio.h>
#include <exception>
#include <pthread.h>
#include "Concurrency.h"

using namespace std;

Mutex::Mutex () {
  if ( pthread_mutex_init (&mutex, NULL) != 0 ) {
    throw ConcurrencyException ( "In constructor of Mutex pthread_mutex_init(&mutex, NULL) failed." );
  }
}

Mutex::~Mutex () {
  pthread_mutex_destroy (&mutex);
}

int Mutex::lock () {

  return pthread_mutex_lock ( &mutex );
}

int Mutex::tryLock () {

  return pthread_mutex_trylock ( &mutex );
}

int Mutex::unlock () {

  return pthread_mutex_unlock ( &mutex );
}

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <ctime>
#include <stdarg.h>
#include <cstdio>
#include <Logger.h>

using namespace std;

namespace atmi {

  /** Create a standard output logger.
   *
   */
  StandardLogger::StandardLogger ( const char *id, LoggingLevel level ){

    setup_level_names();

    set_log_level ( level );
    this->id = id;
    pid = getpid();
  }

  void StandardLogger::print ( LoggingLevel at, const char *msg, va_list args){

    char now [80]; // current date and time string

    if ( get_log_level() <= at) {
      time_t tim;  //create variable of time_t
      time(&tim); //pass variable tim to time function

      strftime ( now, 80, "%Y-%m-%dT%H:%M:%S", localtime (&tim));

      cout << now << "-" << ( id == NULL ? "ANONYMOUS" : id )  << "." << pid << "." << (unsigned int)pthread_self() << "-" << LEVELS[at] << ": ";
      vfprintf (stdout, msg, args);
      cout << endl;
    }
  }
}

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <atmi/logger.hpp>

using namespace std;

namespace atmi {

  /** Create a standard output logger.
   *
   */
  standard_logger::standard_logger ( const char *id, LoggingLevel level ){

    setup_level_names();

    set_log_level ( level );
    this->id = id;
    pid = getpid();
  }

  void standard_logger::log ( LoggingLevel at, const char *msg, va_list args){ //NOSONAR this is a ATMI name it certainly will not change

    char now [80]; // current date and time string
    memset ( now, 0, sizeof (now));

    if ( get_log_level() <= at) {
      time_t tim;  //create variable of time_t
      time(&tim); //pass variable tim to time function
      strftime ( now, 80, "%Y-%m-%dT%H:%M:%S", localtime (&tim));

      if ( at >= WARNING ){
        cerr << now << "-" << ( id == NULL ? "ANONYMOUS" : id )  << "." << pid << "." << (unsigned int)pthread_self() << "-" << LEVELS[at] << ": ";
        vfprintf (stderr, msg, args);
        cerr << endl;
      }
      cout << now << "-" << ( id == NULL ? "ANONYMOUS" : id )  << "." << pid << "." << (unsigned int)pthread_self() << "-" << LEVELS[at] << ": ";
      vfprintf (stdout, msg, args);
      cout << endl;
    }
  }
}

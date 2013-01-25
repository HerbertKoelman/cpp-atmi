#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include <ctime>
#include <iostream>
#include <cstdio>
#include <stdarg.h>
#include <userlog.h>
#include <Logger.h>

using namespace std ;

namespace atmi {

  ULOGLogger::ULOGLogger ( LoggingLevel level ){

    setup_level_names();

    set_log_level ( level );
  }

  void ULOGLogger::print ( LoggingLevel at, const char *msg, va_list args){

    if ( get_log_level() <= at) {

      string m;
      int len = 50;
      char *buff = new char[len];

      // vsnprintf returns the number of characters that are needed if the initial buffer size 
      // was not big enough.
      if ( (len = vsnprintf ( buff, len, msg, args )) > 100 ){
        delete buff;
        buff = new char[( len > BUFSIZ ? BUFSIZ : len )];

        vsnprintf ( buff, len, msg, args );
      }

      m = LEVELS[at]+ ": " + string (msg);
      userlog ( (char *)m.c_str());

      delete buff ;
    }
  }
}

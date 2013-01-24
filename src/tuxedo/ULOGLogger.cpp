#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include <ctime>
#include <cstdio>
#include <stdarg.h>
#include <userlog.h>
#include <Logger.h>

namespace atmi {

  ULOGLogger::ULOGLogger ( LoggingLevel level ){

    setup_level_names();

    set_log_level ( level );
  }

  void ULOGLogger::print ( LoggingLevel at, const char *msg, va_list args){

    if ( get_log_level() <= at) {

      string m;
      char buff[1024];

      vsnprintf ( buff, 1024, msg, args );

      m = LEVELS[at]+ ": " + string (msg);
      userlog ( (char *)m.c_str());
    }
  }
}

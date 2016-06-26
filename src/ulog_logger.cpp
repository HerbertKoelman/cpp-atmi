#include <sys/types.h>
#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <userlog.h>
#include <atmi/logger.hpp>

using namespace std ;

namespace atmi {

  ulog_logger::ulog_logger ( log_levels level ): logger(level){
  }

  void ulog_logger::log ( log_levels at, const char *msg, va_list args){ // NOSONAR this is a ATMI name it certainly will not change

    if ( get_log_level() <= at) {

        memset ( buffer, 0, sizeof (buffer) );

        vsnprintf ( buffer, sizeof(buffer), msg, args );
cout << "vsnprintf returned: " << msg << endl;

        string m = LEVELS[at]+ ": " + string (msg);
cout << "print message : " << m << endl;
        userlog ( (char *)m.c_str());
cout << "done" << endl;
    }
  }
}

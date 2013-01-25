#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdarg>
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

      int len = 512;
      char *buff = NULL;
      buff = new char[len];
      if ( buff != NULL ){
        memset ( buff, 0, sizeof (buff) );

        // try to fit message into default buffer size
        len = vsnprintf ( buff, len-1, msg, args );
cout << "vsnprintf returned: " << msg << ". len : " << len << endl;

        string m = LEVELS[at]+ ": " + string (msg);
cout << "print message : " << m << endl;
        userlog ( (char *)m.c_str());
cout << "done" << endl;

        delete[] buff ;
cout << "freed allocated memory..." << endl;
      }
    }
  }
}

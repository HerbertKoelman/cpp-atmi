#include <iostream>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi/atmi++.hpp>

using namespace std;

namespace atmi {

  const char *Exception::what() throw () {

    return message.c_str();
  }

  Exception::Exception ( const char *msg, ... ) throw () {

    if ( msg != NULL ) {
      va_list ap;

      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }
  }

  void Exception::setup_message ( const char *msg, va_list args ) {

    if ( msg == NULL )
      message = " error occured. Check ULOG.";
    else {
      int len = 512;
      char *buff = new char[len+1];
      memset ( buff, 0, sizeof (buff) );
      vsnprintf ( buff, len, msg, args );

      // vsnprintf returns the number of characters that are needed if the initial buffer size 
      // was not big enough.
/* needs debugging
      if ( (len = vsnprintf ( buff, len, msg, args )) > 50 ){
        delete[] buff;
        buff = new char[(len > BUFSIZ ? BUFSIZ : len +1 )];
        memset ( buff, 0, ( len > BUFSIZ ? BUFSIZ : len  +1) );

        vsnprintf ( buff, len, msg, args );
      }
*/

      message = buff;

      delete[] buff;
    }
  }
}

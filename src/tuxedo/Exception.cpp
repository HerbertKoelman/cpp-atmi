/* $Id: Exception.C 79 2007-08-18 17:30:26Z hkoelman $ */
#include <iostream>
using namespace std;

#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi++.h>

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
      char buff [512];

      // try to fit message into default buffer size
      vsnprintf ( buff, 512, msg, args );

      message = buff;
    }
  }
}

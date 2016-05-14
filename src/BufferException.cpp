/* $Id: TuxedoException.C 79 2007-08-18 17:30:26Z hkoelman $ */
#include <iostream>
using namespace std;

#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <fml.h>
#include <atmi/atmi++.hpp>

namespace atmi {

  int BufferException::getFerror () {

    return ferror;
  }

  const char *BufferException::getFmlerrmsg () {

    return Fstrerror32 ( ferror );
  }

  const char *BufferException::getMessage() {

    return message.c_str();
  }

  const char *BufferException::what() throw () {

    message = message + " " + getFmlerrmsg ();

    return message.c_str();
  }

  BufferException::BufferException ( int err, const char *msg, ... ) throw () {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    this->ferror = err;
  }
}

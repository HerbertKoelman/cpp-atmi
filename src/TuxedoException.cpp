/* $Id: TuxedoException.C 79 2007-08-18 17:30:26Z hkoelman $ */

#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi/atmi++.h>

using namespace std;

namespace atmi {

  const char *TuxedoException::getTperrdetail () {
    return tpstrerrordetail(errdetail, 0);
  }

  const char *TuxedoException::getTperrmsg () {

    return tpstrerror ( tuxerror );
  }

  const char *TuxedoException::getMessage() {

    return message.c_str();
  }

  const char *TuxedoException::what() throw () {

    message = message + " " + getTperrmsg ();

    return message.c_str();
  }

  TuxedoException::TuxedoException ( int err, const char *msg, ... ) throw () {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    this->tuxerror = err;
    errdetail = tperrordetail ( 0 );
  }

}

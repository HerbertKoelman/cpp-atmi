/* $Id: TimeoutException.C 79 2007-08-18 17:30:26Z hkoelman $

   Handles TPETIME specific exceptions.
 */

#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi/atmi++.h>

namespace atmi {

  TimeoutException::TimeoutException ( int err, const char * msg, ... ) throw () : TuxedoException ( err )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

  }
}

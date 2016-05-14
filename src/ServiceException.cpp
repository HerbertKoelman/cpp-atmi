/* $Id: ServiceException.C 79 2007-08-18 17:30:26Z hkoelman $

   Handles Queue specific exceptions.
 */

#include <iostream>
using namespace std;

#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>

#include <atmi/atmi++.hpp>

namespace atmi {

  ServiceException::ServiceException ( int err, const char * msg, ... ) throw () : TuxedoException ( err )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

  }
}

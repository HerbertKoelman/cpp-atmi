/* $Id: AbortedException.C 79 2007-08-18 17:30:26Z hkoelman $

 */

#include <iostream>
using namespace std;

#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi++.h>

namespace atmi {

  AbortedException::AbortedException ( int err, int diagno, const char * msg, ... ) throw () : DiagnosticException ( err, diagno )
  {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
  }
}

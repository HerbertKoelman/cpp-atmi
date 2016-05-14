/* $Id: InterruptException.C 79 2007-08-18 17:30:26Z hkoelman $

 */

#include <stdarg.h>
#include <atmi/atmi++.hpp>

namespace atmi {

  InterruptException::InterruptException ( int err, const char * msg, ... ) throw () : TuxedoException ( err )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

  }
}

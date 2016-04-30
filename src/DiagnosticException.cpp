/* $Id: DiagnosticException.C 79 2007-08-18 17:30:26Z hkoelman $

   Handles Queue specific exceptions.
 */

#include <iostream>
using namespace std;
#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi/atmi++.h>

namespace atmi {

  DiagnosticException::DiagnosticException ( int err, long diagno, const char * msg, ... ) throw () : TuxedoException ( err )
  {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
    this->diagno = diagno;
  }

  const char *DiagnosticException::what () throw () {

    string buffer;
    if ( tuxerror == TPEDIAGNOSTIC ) {
      buffer += message + getDiagmsg ();
    } else {
      buffer += message + getTperrmsg ();
    }

    return buffer.c_str();
  }

  const char *DiagnosticException::getDiagmsg () {
    switch ( diagno ) {
      case QMEINVAL:
        return "QMEINVAL: An invalid flag value was specified.";

      case QMEBADRMID:
        return "QMEBADRMID: An invalid resource manager identifier was specified.";

      case QMENOTOPEN:
        return "QMENOTOPEN: The resource manager is not currently open.";

      case QMETRAN:
        return "QMETRAN: Failed to start transaction to enqueue/dequeue a message.";

      case QMEBADMSGID:
        return "QMEBADMSGID: An invalid message identifier was specified.";

      case QMESYSTEM:
        return "QMESYSTEM: A system error occurred. The exact nature of the error is written to a log file.";

      case QMEOS:
        return "QMEOS: An operating system error occurred.";

      case QMEABORTED:
        return "QMEABORTED: The operation was aborted.";

      case QMEPROTO:
        return "QMEPROTO: An enqueue was done when the transaction state was not active.";

      case QMEBADQUEUE:
        return "QMEBADQUEUE: An invalid or deleted queue name was specified.";

      case QMENOSPACE:
        return "QMENOSPACE: insufficient resource, such as no space on the queue.";

      case QMERELEASE:
        return "QMERELEASE: System does not support a newer feature.";

      case QMESHARE:
        return "QMESHARE: The queue is already opened for exclusive read and/or write.";

      default:
        return "Never heard about this diagno !!??";
    }
  }
}

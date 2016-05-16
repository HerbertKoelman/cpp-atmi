/* $Id: aborted_exception.C 79 2007-08-18 17:30:26Z hkoelman $

 */

#include <iostream>
#include <cerrno>
#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <atmi/atmi++.hpp>

namespace atmi {

  aborted_exception::aborted_exception ( int err, int diagno, const char * msg, ... ) throw () : diagnostic_exception ( err, diagno )
  {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
  }

  const char *atmi_exception::what() throw () {

    return message.c_str();
  }

  atmi_exception::atmi_exception ( const char *msg, ... ) throw () {

    if ( msg != NULL ) {
      va_list ap;

      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }
  }

  void atmi_exception::setup_message ( const char *msg, va_list args ) {

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

  blocking_exception::blocking_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err )
  {

    va_list ap;
    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
  }

  int buffer_exception::getFerror () {

    return ferror;
  }

  const char *buffer_exception::getFmlerrmsg () {

    return Fstrerror32 ( ferror );
  }

  const char *buffer_exception::getMessage() {

    return message.c_str();
  }

  const char *buffer_exception::what() throw () {

    message = message + " " + getFmlerrmsg ();

    return message.c_str();
  }

  buffer_exception::buffer_exception ( int err, const char *msg, ... ) throw () {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    this->ferror = err;
  }

  diagnostic_exception::diagnostic_exception ( int err, long diagno, const char * msg, ... ) throw () : tuxedo_exception ( err )
  {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
    this->diagno = diagno;
  }

  const char *diagnostic_exception::what () throw () {

    string buffer;
    if ( tuxerror == TPEDIAGNOSTIC ) {
      buffer += message + getDiagmsg ();
    } else {
      buffer += message + getTperrmsg ();
    }

    return buffer.c_str();
  }

  const char *diagnostic_exception::getDiagmsg () {
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

  interrupt_exception::interrupt_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

  }

  nomsg_exception::nomsg_exception ( int err, int diagno, const char * msg, ... ) throw () : diagnostic_exception ( err, diagno )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
  }

  service_exception::service_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

  }

  timeout_exception::timeout_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err )
  {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

  }

  const char *tuxedo_exception::getTperrdetail () {
    return tpstrerrordetail(errdetail, 0);
  }

  const char *tuxedo_exception::getTperrmsg () {

    return tpstrerror ( tuxerror );
  }

  const char *tuxedo_exception::getMessage() {

    return message.c_str();
  }

  const char *tuxedo_exception::what() throw () {

    message = message + " " + getTperrmsg ();

    return message.c_str();
  }

  tuxedo_exception::tuxedo_exception ( int err, const char *msg, ... ) throw () {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    this->tuxerror = err;
    errdetail = tperrordetail ( 0 );
  }

  int unix_exception::get_errno () {

    return error;
  }

  const char *unix_exception::what() throw () {

    message = message + ": " + strerror ( error );

    return message.c_str();
  }

  unix_exception::unix_exception ( int err, const char *msg, ... ) throw () {

    if ( msg != NULL ) {
      va_list ap;

      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }

    this->error = ( err == 0 ? errno : err );;
  }

  unix_exception::unix_exception ( const char *msg, ... ) throw () {

    if ( msg != NULL ) {
      va_list ap;

      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }

    this->error = errno;
  }

}


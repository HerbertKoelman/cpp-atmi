#include <iostream>
#include <cerrno>
#include <stdarg.h>
#include <stdio.h>
#include <atmi.h>
#include <memory>
#include <atmi/atmi++.hpp>

namespace atmi {


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
      _message = "ATMI error occured, check ULOG.";
    else {
      int len = 1024;
      char buff[len+1];

      memset ( buff, 0, len+1 );
      vsnprintf ( buff, len, msg, args );

      _message = buff ;
    }
  }

#if __cplusplus < 201103L
  const char *atmi_exception::what() throw () {
#else
  const char *atmi_exception::what() const noexcept {
#endif

    return message();
  }

#if __cplusplus < 201103L
  const char *atmi_exception::message() throw () {
#else
  const char *atmi_exception::message() const noexcept {
#endif

    return _message.c_str();
  }


  // unix_exception ----------------------------------------
  //

  unix_exception::unix_exception ( int err, const char *msg, ... ) throw () {

    if ( msg != NULL ) {
      va_list ap;

      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }

    _error = ( err == 0 ) ? errno : err ;

    _message.append(" ");
    _message.append(strerror ( _error ));
  }

  unix_exception::unix_exception ( const char *msg, ... ) throw () {

    if ( msg != NULL ) {
      va_list ap;

      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }

    _error = errno;

    _message.append(" ");
    _message.append(strerror ( _error ));
  }

  int unix_exception::error () const {

    return _error;
  }

  // buffer_exception ---------------------------------------
  //

  buffer_exception::buffer_exception ( int err, const char *msg, ... ) throw () {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    _error = err;
    // this call depends on the value of _error and must placed after the aboce assignement.
    _what = _message + " " + error_message (); 
  }

  int buffer_exception::error () const {

    return _error;
  }

  const char *buffer_exception::error_message () const {

    return Fstrerror32 ( _error );
  }

#if __cplusplus < 201103L
  const char *buffer_exception::what() throw () {
#else
  const char *buffer_exception::what() const noexcept {
#endif

    return _what.c_str();
  }

  // tuxedo_exception --------------------------------------
  //

//  tuxedo_exception::tuxedo_exception ( int err) throw (): _error(err) {
//  }

  tuxedo_exception::tuxedo_exception ( int err, const char *msg, ... ) throw () {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    _error = err;
    _detail = tperrordetail ( 0 );
    
    // this call depends on the value of _error and must placed after the aboce assignement.
    _what = _message + " " + error_message (); 
  }
  
#if __cplusplus < 201103L
  const char *tuxedo_exception::what() throw () {
#else
  const char *tuxedo_exception::what() const noexcept {
#endif

    return (_what.size() > 0 )?_what.c_str() : message();
  }

  const char *tuxedo_exception::error_detail () const {
    return tpstrerrordetail(_detail, 0);
  }

  const char *tuxedo_exception::error_message () const {

    return tpstrerror ( _error );
  }

  // derived from tuxedo_exception ------------------
  //
  interrupt_exception::interrupt_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err ) {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
    
    // this call depends on the value of _error and must placed after the aboce assignement.
    _what = _message + " " + error_message (); 
  }

  service_exception::service_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err ) {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    // this call depends on the value of _error and must placed after the aboce assignement.
    _what = _message + " " + error_message (); 
  }

  timeout_exception::timeout_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err ) {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    // this call depends on the value of _error and must placed after the aboce assignement.
    _what = _message + " " + error_message (); 
  }

  blocking_exception::blocking_exception ( int err, const char * msg, ... ) throw () : tuxedo_exception ( err ) {

    if ( msg != NULL){
      va_list ap;
      va_start ( ap, msg );
      setup_message ( msg, ap );
      va_end (ap);
    }
    
    // this call depends on the value of _error and must placed after the aboce assignement.
    _what = _message + " " + error_message (); 
  }

  // /Q related exceptions ---------------------------------------------------------
  //
  // diagnostic exception -----------------------------------
  //
  
  diagnostic_exception::diagnostic_exception ( int err, long diagno, const char * msg, ... ) throw () : tuxedo_exception ( err )
  {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    _diagno = diagno;

    if ( error() == TPEDIAGNOSTIC ) {
      _what = _message + " " + diagnostic_message ();
    } else {
      _what = _message + " " + error_message ();
    }
  }

  const char *diagnostic_exception::diagnostic_message () const {
    switch ( _diagno ) {
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

  nomsg_exception::nomsg_exception ( int err, int diagno, const char * msg, ... ) throw () : diagnostic_exception ( err, diagno ) {

    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    if ( error() == TPEDIAGNOSTIC ) {
      _what = _message + " " + diagnostic_message ();
    } else {
      _what = _message + " " + error_message ();
    }
  }

  aborted_exception::aborted_exception ( int err, int diagno, const char * msg, ... ) throw () : diagnostic_exception ( err, diagno )
  {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);

    if ( error() == TPEDIAGNOSTIC ) {
      _what = _message + " " + diagnostic_message ();
    } else {
      _what = _message + " " + error_message ();
    }
  }

}

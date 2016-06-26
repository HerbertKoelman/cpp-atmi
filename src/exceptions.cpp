#include <iostream>
#include <cerrno>
// #include <stdio.h>
#include <atmi.h>
#include <memory>
#include <atmi/atmi++.hpp>

namespace atmi {

  atmi_exception::atmi_exception (): _message("ATMI error occured, check ULOG for more informations"){
  }

#if __cplusplus < 201103L
  const char *atmi_exception::what() throw(){
#else
  const char *atmi_exception::what() const noexcept {
#endif

//    printf("Atmi what:  [%s], what to [%s] (size: %d).\n", _message.c_str(), _what.c_str(), _what.size());
    return (_what.size() > 0 ) ? _what.c_str() : _message.c_str();
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

  unix_exception::unix_exception (): _error(errno), atmi_exception(strerror (errno)) {
  }

  int unix_exception::error () const {

    return _error;
  }

  // buffer_exception ---------------------------------------
  //

  buffer_exception::buffer_exception (): _error(Ferror32), atmi_exception("FML buffer error occured.") {
  }

  int buffer_exception::error () const {

    return _error;
  }

  const char *buffer_exception::error_message () const {

    return Fstrerror32 ( _error );
  }

  // tuxedo_exception --------------------------------------
  //

  const char *tuxedo_exception::error_detail () const {
    return tpstrerrordetail(_detail, 0);
  }

  const char *tuxedo_exception::error_message () const {

    return tpstrerror ( _error );
  }


  // /Q related exceptions ---------------------------------------------------------
  //
  // diagnostic exception -----------------------------------
  //
  
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
}

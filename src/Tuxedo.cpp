/*
   $Id: Tuxedo.C 79 2007-08-18 17:30:26Z hkoelman $
 */

#include <limits.h>
#include <stdarg.h>
#include <iostream>
#include <atmi.h>
#include <string>
#include <nl_types.h>
#include <atmi/atmi++.hpp>

#define CATD_ATMI_SET 100

using namespace std;

namespace atmi {

  Tuxedo::Tuxedo () {
    errorno =  0;
    errornodetail = 0;
    flags = TPNOFLAGS;
    context = 0;
    catd  = catopen (const_cast<char *> ("atmi++"), 0 );
  }

  Tuxedo::~Tuxedo() {

    catclose ( catd );
  }

  void Tuxedo::updateErrno () {
    errorno = tperrno;
    errornodetail = tperrordetail ( 0 );
  };

  char * Tuxedo::allocate ( const char *type, const char *subtype, long size ) {

    char *buffer = NULL;

    buffer  = tpalloc( const_cast<char *>(type), const_cast<char *>(subtype), size );

    updateErrno ();

    if ( buffer == NULL ) {
      throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 28, "TPALLOCATE failed. Check ULOG for more information.") );
    }

    return buffer;
  }

  char *Tuxedo::extend ( char *buff, long extent ) {

    char *b = tprealloc ( buff, extent );

    updateErrno ();

    if ( b == NULL ) {
      throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 29, "TPREALLOC failed. Check ULOG for more information.") );
    }

    return b;
  };


  void Tuxedo::free ( char *buffer ) {
    if ( buffer != NULL) tpfree ( buffer );
  };


  int Tuxedo::begin ( int timeout ) {

    int rc  = -1;

    rc = tpbegin ( timeout, 0 );

    updateErrno ();

    if ( rc < 0 ) {
      throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 30, "TPBEGIN failed. Check ULOG for more information.") );
    }

    return rc;
  }

  int Tuxedo::commit () {

    int rc  = -1;

    rc = tpcommit ( 0 );

    updateErrno ();

    if ( rc < 0 ) {
      throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 31, "TPCOMMIT failed. Check ULOG for more information.") );
    }

    return rc;
  }

  int Tuxedo::abort () {

    int rc  = -1;

    rc = tpabort ( 0 );

    updateErrno ();

    if ( rc < 0 ) {
      throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 32, "TPABORT failed. Check ULOG for more information.") );
    }

    return rc;
  }

  void Tuxedo::switch_context () {

    TPCONTEXT_T ctxt = 0;
    int rc = -1;

    if ( context > 0 ) {
      rc = tpgetctxt ( &ctxt, 0 );
      if ( rc < 0 ) {
        throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 34, "Context switch failed while calling tpgetctxt ( target context was %d )."), context );
      } else {
        if ( context != ctxt ) {                         // check if we have to switch at all
          rc = tpsetctxt ( context, 0 );
          if ( rc < 0 ) {
            throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 35, "Context switch failed ( target context was %d )."), context );
          }
        }
      }
    }
  }

  long Tuxedo::set ( long f, long sf ){

    f |= sf;

    return f;
  }

  long Tuxedo::unset ( long f, long uf ){

    f &= ( LONG_MAX ^ uf );

    return f;
  }

  int Tuxedo::handleTperrno ( int _tperrno, const char *msg, ... ) {

    va_list ap;
    va_start ( ap, msg );

    switch ( _tperrno ) {
      case TPEINVAL:
      case TPEPERM:
      case TPENOENT:
      case TPEITYPE:
      case TPEOTYPE:
      case TPETRAN:
      case TPEPROTO:
      case TPESYSTEM:
      case TPEOS:
      case TPELIMIT:
      {                           // required because we are declaring variables
        TuxedoException err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPEBLOCK:
      {
        BlockingException err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPGOTSIG:
      {
        InterruptException err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPESVCERR:
      {
        ServiceException err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPETIME:
      {
        TimeoutException err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPESVCFAIL:
        // return application specific error number instead
        // as the application will probably know what to do
        _tperrno = ( tpurcode > 0 ? tpurcode : -1 );
        break;

      default:
        throw TuxedoException (_tperrno,catgets ( catd, CATD_ATMI_SET, 33,"Never heard about this tperrno (%d)."), _tperrno );
    };

    va_end (ap);

    return _tperrno;
  }
}

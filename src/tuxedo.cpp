/*
   $Id: tuxedo.C 79 2007-08-18 17:30:26Z hkoelman $
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

  tuxedo::tuxedo () {
    errorno =  0;
    errornodetail = 0;
    flags = TPNOFLAGS;
    context = 0;
    catd  = catopen (const_cast<char *> ("atmi++"), 0 );
  }

  tuxedo::~tuxedo() {
    catclose ( catd );
  }

  void tuxedo::updateErrno () {
    errorno = tperrno;
    errornodetail = tperrordetail ( 0 );
  };

  char * tuxedo::allocate ( const char *type, const char *subtype, long size ) {

    char *buffer = NULL;

    buffer  = tpalloc( const_cast<char *>(type), const_cast<char *>(subtype), size );

    updateErrno ();

    if ( buffer == NULL ) {
      throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 28, "TPALLOCATE failed. Check ULOG for more information.") );
    }

    return buffer;
  }

  char *tuxedo::extend ( char *buff, long extent ) {

    char *b = tprealloc ( buff, extent );

    updateErrno ();

    if ( b == NULL ) {
      throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 29, "TPREALLOC failed. Check ULOG for more information.") );
    }

    return b;
  };


  void tuxedo::free ( char *buffer ) {
    if ( buffer != NULL) tpfree ( buffer );
  };


  int tuxedo::begin ( int timeout ) {

    int rc  = -1;

    rc = tpbegin ( timeout, 0 );

    updateErrno ();

    if ( rc < 0 ) {
      throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 30, "TPBEGIN failed. Check ULOG for more information.") );
    }

    return rc;
  }

  int tuxedo::commit () {

    int rc  = -1;

    rc = tpcommit ( 0 );

    updateErrno ();

    if ( rc < 0 ) {
      throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 31, "TPCOMMIT failed. Check ULOG for more information.") );
    }

    return rc;
  }

  int tuxedo::abort () {

    int rc  = -1;

    rc = tpabort ( 0 );

    updateErrno ();

    if ( rc < 0 ) {
      throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 32, "TPABORT failed. Check ULOG for more information.") );
    }

    return rc;
  }

  void tuxedo::switch_context () {

    TPCONTEXT_T ctxt = 0;
    int rc = -1;

    if ( context > 0 ) {
      rc = tpgetctxt ( &ctxt, 0 );
      if ( rc < 0 ) {
        throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 34, "Context switch failed while calling tpgetctxt ( target context was %d )."), context );
      } else {
        if ( context != ctxt ) {                         // check if we have to switch at all
          rc = tpsetctxt ( context, 0 );
          if ( rc < 0 ) {
            throw tuxedo_exception ( tperrno, catgets ( catd, CATD_ATMI_SET, 35, "Context switch failed ( target context was %d )."), context );
          }
        }
      }
    }
  }

  long tuxedo::set ( long f, long sf ){

    f |= sf;

    return f;
  }

  long tuxedo::unset ( long f, long uf ){

    f &= ( LONG_MAX ^ uf );

    return f;
  }

  int tuxedo::handleTperrno ( int _tperrno, const char *msg, ... ) {

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
        tuxedo_exception err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPEBLOCK:
      {
        blocking_exception err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPGOTSIG:
      {
        interrupt_exception err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPESVCERR:
      {
        service_exception err ( _tperrno );
        err.setup_message ( msg,  ap );
        throw err;
      }
      break;
      case TPETIME:
      {
        timeout_exception err ( _tperrno );
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
        throw tuxedo_exception (_tperrno,catgets ( catd, CATD_ATMI_SET, 33,"Never heard about this tperrno (%d)."), _tperrno );
    };

    va_end (ap);

    return _tperrno;
  }
}

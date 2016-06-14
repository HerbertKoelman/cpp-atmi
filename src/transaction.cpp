/*
   $Id: transaction.C 86 2007-08-19 19:55:28Z hkoelman $

   Helper for handling tuxedo TP calls
 */

#include <unistd.h>
#include <iostream>
#include <atmi/atmi++.hpp>

#include <string.h>

using namespace std;

namespace atmi {

  transaction::transaction ( const char *service ): _service(service) {

    // length 32 chars (see definition in atmi.h
    if ( _service.size() > 32 ) throw tuxedo_exception ( 0, "Given service name '%s' is too long.", service );

    _call_descriptor = 0;
  }


  int transaction::call ( char *idata, long ilen, char **odata, long *olen, int *urcode, int retries, int delay ) {

    int retry = 0;
    int rc = -1;

    // switch context if needed
    switch_context ();

    do {

      rc = -1;
      if ((rc = tpcall ( (char *) _service.c_str(), idata, ilen, odata, olen,flags )) < 0 ) {

        switch ( tperrno ) {
          case TPETIME:
          case TPESVCERR:
            if ( delay > 0 ) sleep (delay);
        };
      } else {
        if ( urcode != NULL ) *urcode = tpurcode;
      }

      retry++;
    } while ( retry <= retries && rc != 0 );

    updateErrno ();

    if ( rc < 0 ) {
      rc = handle_tperrno ( tperrno, "TPCALL to %s failed.", _service.c_str() );
    }

    return rc;
  }

  int transaction::call(atmi::buffer *buffer, int *urcode,int retries, int delay ){

    int rc = -1;
    long osize = buffer->size();
    FBFR32 *b = buffer->get_buffer();

    rc = call ( (char *)buffer->get_buffer(), buffer->size(), (char **) &b, &osize, urcode, retries, delay );
    buffer->set_buffer ( b );

    return rc;
  };

  int transaction::call ( char **idata, long *ilen, int *urcode, int retries, int delay ) {

    int rc = -1;

    rc = call ( *idata, *ilen, idata, ilen, urcode, retries, delay );

    return rc;
  }

  int transaction::acall (buffer *buffer ) {

    int ret = -1;
    _call_descriptor = -1;

    ret = acall ( (char *) buffer->get_buffer(), buffer->size() );

    updateErrno ();

    if ( ret < 0 ) {
      ret = handle_tperrno ( tperrno, "TPACALL to %s failed.", _service.c_str() );
    } else {
      _call_descriptor = ret;
    }

    return ret;
  }


  int transaction::reply (atmi::buffer *buffer, int *urcode, int *cd ) {

    int rc = -1;
    FBFR32 *b = buffer->get_buffer ();
    long osize = buffer->size();

    rc = reply( (char **) &b, &osize, urcode, cd);
    if ( rc != -1 ) {
      buffer->set_buffer ( b );
    }

    return rc;
  }

  int transaction::acall ( char *idata, long ilen ) {

    int ret = -1;
    _call_descriptor = -1;

    // check if need switch context
    switch_context ();

    ret = tpacall ( (char *) _service.c_str(), idata, ilen, flags );

    updateErrno ();

    if ( ret < 0 ) {
      ret = handle_tperrno ( tperrno, "TPACALL to %s failed.", _service.c_str() );
    } else {
      _call_descriptor = ret;
    }

    return ret;
  }


  int transaction::reply (char **data, long *len, int *urcode, int *cd ) {

    int rc = -1;

    if ( cd == NULL ) {
      rc = tpgetrply(&_call_descriptor, data, len, flags);
    } else {
      rc = tpgetrply(cd, data, len, flags);
      if ( *cd == _call_descriptor ) _call_descriptor = 0;
    }

    if ( urcode != NULL ) *urcode = tpurcode;

    updateErrno ();

    if ( rc < 0 ) {
      rc = handle_tperrno ( tperrno, "TPGETREPLY from %s failed.", _service.c_str() );
    }

    return rc;
  }
  int transaction::cancel ( int cd ) {

    int rc = -1;

    if ( cd == 0 )
      rc = tpcancel ( _call_descriptor );
    else
      rc = tpcancel ( cd  );

    updateErrno ();

    if ( rc < 0 ) {
      rc = handle_tperrno ( tperrno, "TPCANCEL of a call to %s failed." );
    }

    return rc;
  }

}

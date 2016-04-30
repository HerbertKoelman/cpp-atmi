/*
   $Id: Tp.C 86 2007-08-19 19:55:28Z hkoelman $

   Helper for handling tuxedo TP calls
 */

#include <unistd.h>
#include <iostream>
#include <atmi/atmi++.h>

#include <string.h>

using namespace std;

namespace atmi {

  Tp::Tp ( const char *service ) {

    // length 32 chars (see definition in atmi.h
    if ( strlen ( service ) > 32 ) throw TuxedoException ( 0, "Given service name '%s' is too long.", service );

    this->service.clear();
    this->service = service;
    calldesc = 0;
  }


  int Tp::call ( char *idata, long ilen, char **odata, long *olen, int *urcode, int retries, int delay ) {

    int retry = 0;
    int rc = -1;

    // switch context if needed
    switch_context ();

    do {

      rc = -1;
      if ((rc = tpcall ( (char *) service.c_str(), idata, ilen, odata, olen,flags )) < 0 ) {

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
      rc = handleTperrno ( tperrno, "TPCALL to %s failed.", service.c_str() );
    }

    return rc;
  }

  int Tp::call( Buffer *buffer, int *urcode,int retries, int delay ){

    int rc = -1;
    long osize = buffer->size();
    FBFR32 *b = buffer->get_buffer();

    rc = call ( (char *)buffer->get_buffer(), buffer->size(), (char **) &b, &osize, urcode, retries, delay );
    buffer->set_buffer ( b );

    return rc;
  };

  int Tp::call ( char **idata, long *ilen, int *urcode, int retries, int delay ) {

    int rc = -1;

    rc = call ( *idata, *ilen, idata, ilen, urcode, retries, delay );

    return rc;
  }

  int Tp::acall ( Buffer *buffer ) {

    int ret = -1;
    calldesc = -1;

    ret = acall ( (char *) buffer->get_buffer(), buffer->size() );

    updateErrno ();

    if ( ret < 0 ) {
      ret = handleTperrno ( tperrno, "TPACALL to %s failed.", service.c_str() );
    } else {
      calldesc = ret;
    }

    return ret;
  }


  int Tp::reply (Buffer *buffer, int *urcode, int *cd ) {

    int rc = -1;
    FBFR32 *b = buffer->get_buffer ();
    long osize = buffer->size();

    rc = reply( (char **) &b, &osize, urcode, cd);
    if ( rc != -1 ) {
      buffer->set_buffer ( b );
    }

    return rc;
  }

  int Tp::acall ( char *idata, long ilen ) {

    int ret = -1;
    calldesc = -1;

    // check if need switch context
    switch_context ();

    ret = tpacall ( (char *) service.c_str(), idata, ilen, flags );

    updateErrno ();

    if ( ret < 0 ) {
      ret = handleTperrno ( tperrno, "TPACALL to %s failed.", service.c_str() );
    } else {
      calldesc = ret;
    }

    return ret;
  }


  int Tp::reply (char **data, long *len, int *urcode, int *cd ) {

    int rc = -1;

    if ( cd == NULL ) {
      rc = tpgetrply(&calldesc, data, len, flags);
    } else {
      rc = tpgetrply(cd, data, len, flags);
      if ( *cd == calldesc ) calldesc = 0;
    }

    if ( urcode != NULL ) *urcode = tpurcode;

    updateErrno ();

    if ( rc < 0 ) {
      rc = handleTperrno ( tperrno, "TPGETREPLY from %s failed.", service.c_str() );
    }

    return rc;
  }
  int Tp::cancel ( int cd ) {

    int rc = -1;

    if ( cd == 0 )
      rc = tpcancel ( calldesc );
    else
      rc = tpcancel ( cd  );

    updateErrno ();

    if ( rc < 0 ) {
      rc = handleTperrno ( tperrno, "TPCANCEL of a call to %s failed." );
    }

    return rc;
  }

}

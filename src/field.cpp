/*
   $Id$
 */

#include <iostream>

#include <stdarg.h>
#include <atmi.h>
#include <fml.h>
#include <memory>
#include <string>
#include <sstream>

#include <atmi/atmi++.hpp>

using namespace std;

namespace atmi {

/* fields --------------------------------------------------------------------*/

  const char *field::tname () {
    return Ftype32 ( fid );
  };

  FLDID32 field::id () {
    return fid;
  };

  const char *field::name () {
    return fname;
  };

  int field::number () {
    return Fldno32 ( fid );
  };

  FLDOCC32 field::occurence() {

    return focc;
  };

  int field::error( ){

    return last_err;
  };

  void field::setFocc( FLDOCC32 occ) {

    focc = occ < 0 ? 0 : occ;              // check that we don't have a negative occurence
  };

  int field::type () {
    return Fldtype32 ( fid );
  };

  const char*field::what() {

    stringstream buff;
    buff << "Fid: " << fid << ", occurence: " << focc << ", name: " << fname << ", len: " << length() << ", type: " << tname () << ".";
    return buff.str().c_str ();
  }

  long field::needed (){

    int rc = Fneeded32 ( 1, (length() > 0 ? length() : 1 ));
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "FNEEDED32i: failed to estimate needed space for field %s (id: %d, occ: %d). Make sure field contains data (length() > 0).", fname, fid, focc );
    }

    return rc;              // this the size in bytes needed
  };

  int field::remove ( buffer *b) {

    int rc = Fdel32 ( b->_buffer, fid, focc );
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "FDEL32 failed for field %s (id: %d, occ: %d) failed", fname, fid, focc );
    }

    return rc;
  };

  void field::setup ( FLDID32 fid){
    if ( fid < 8191 ) {
      fname = Fname( fid );
    } else {
      fname = Fname32( fid );
    }
    if ( fname != NULL ) {
      this->fid = fid;
      this->focc = 0;
    } else {
      throw buffer_exception ( Ferror32, "Failed to initialize field %d. Check the values of FIELDTBLS32 and FLDTBLDIR32.", fid );
    }
  };

  int field::handleFerror ( int err, const char *msg, ... ) {

    this->last_err = err;

    va_list ap;
    va_start ( ap, msg );

    if ( last_err != FNOTPRES ) {
      throw buffer_exception ( last_err, msg, ap );
    }

    va_end ( ap );

    return last_err;
  }

// operators --------------------------------------------------------------------------------

  ostream &operator<< ( ostream &out, Tfield<string> &f ){

    return out << (string)f;
  };

}

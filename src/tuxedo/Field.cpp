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

#include <atmi++.h>

using namespace std;

namespace atmi {

/* Fields --------------------------------------------------------------------*/

  const char *Field::tname () {
    return Ftype32 ( fid );
  };

  FLDID32 Field::id () {
    return fid;
  };

  const char *Field::name () {
    return fname;
  };

  int Field::number () {
    return Fldno32 ( fid );
  };

  FLDOCC32 Field::occurence() {

    return focc;
  };

  int Field::error( ){

    return last_err;
  };

  void Field::setFocc( FLDOCC32 occ) {

    focc = occ < 0 ? 0 : occ;              // check that we don't have a negative occurence
  };

  int Field::type () {
    return Fldtype32 ( fid );
  };

  const char*Field::what() {

    stringstream buff;
    buff << "Fid: " << fid << ", occurence: " << focc << ", name: " << fname << ", len: " << length() << ", type: " << tname () << ".";
    return buff.str().c_str ();
  }

  long Field::needed (){

    int rc = Fneeded32 ( 1, (length() > 0 ? length() : 1 ));
    if ( rc < 0 ) {
      throw BufferException ( Ferror32, "FNEEDED32i: failed to estimate needed space for field %s (id: %d, occ: %d). Make sure field contains data (length() > 0).", fname, fid, focc );
    }

    return rc;              // this the size in bytes needed
  };

  int Field::remove ( Buffer *b) {

    int rc = Fdel32 ( b->buffer, fid, focc );
    if ( rc < 0 ) {
      throw BufferException ( Ferror32, "FDEL32 failed for field %s (id: %d, occ: %d) failed", fname, fid, focc );
    }

    return rc;
  };

  void Field::setup ( FLDID32 fid){
    fname = Fname32( fid );
    if ( fname != NULL ) {
      this->fid = fid;
      this->focc = 0;
    } else {
      throw BufferException ( Ferror32, "Failed to initialize field %d. Check the values of FIELDTBLS32 and FLDTBLDIR32 and make sure you have read permission on the FML tables files.", fid );
    }
  };

  int Field::handleFerror ( int err, const char *msg, ... ) {

    this->last_err = err;

    va_list ap;
    va_start ( ap, msg );

    if ( last_err != FNOTPRES ) {
      throw BufferException ( last_err, msg, ap );
    }

    va_end ( ap );

    return last_err;
  }

// operators --------------------------------------------------------------------------------

  ostream &operator<< ( ostream &out, TField<string> &f ){

    return out << (string)f;
  };

}

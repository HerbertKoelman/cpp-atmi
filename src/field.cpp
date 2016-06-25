/* author: herbert koelman (herbert.koelman@me.com)
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
    return Ftype32 ( _field_id );
  };

  FLDID32 field::id () {
    return _field_id;
  };

  const char *field::name () {
    return _field_name;
  };

  int field::number () {
    return Fldno32 ( _field_id );
  };

  FLDOCC32 field::occurence() {

    return _field_occurence;
  };

  int field::error( ){

    return _ferror;
  };

  void field::set_field_occurence( FLDOCC32 occ) {

    _field_occurence = occ < 0 ? 0 : occ;              // check that we don't have a negative occurence
  };

  int field::type () {
    return Fldtype32 ( _field_id );
  };

  const char *field::what() {

    stringstream buff;
    buff << "Fid: " << _field_id << ", occurence: " << _field_occurence << ", name: " << _field_name << ", len: " << length() << ", type: " << tname () << ".";
    _what = buff.str();
    return _what.c_str ();
  }

  long field::needed (){

    int rc = Fneeded32 ( 1, (length() > 0 ? length() : 1 ));
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "FNEEDED32: failed to estimate needed space for field %s (id: %d, occ: %d). Make sure field contains data (length() > 0).", _field_name, _field_id, _field_occurence );
    }

    return rc;              // this the size in bytes needed
  };

  int field::remove ( buffer *b) {//NOSONAR this FML grammar and cannot be renamed.

    int rc = Fdel32 ( b->_buffer, _field_id, _field_occurence );
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "FDEL32 failed for field %s (id: %d, occ: %d) failed", _field_name, _field_id, _field_occurence );
    }

    return rc;
  };

  void field::setup ( FLDID32 field_id){
    if ( field_id < 8191 ) {
      _field_name = Fname( field_id );
    } else {
      _field_name = Fname32( field_id );
    }
    if ( _field_name != NULL ) {
      this->_field_id = field_id;
      this->_field_occurence = 0;
    } else {
      throw buffer_exception ( Ferror32, "Failed to initialize field %d. Check the values of FIELDTBLS32 and FLDTBLDIR32.", field_id );
    }
  };

//  int field::ferror_handler ( int ferror, const char *format, ... ) {
//
//    _ferror = ferror;
//
//    va_list ap;
//    va_start ( ap, format );
//
//    if ( _ferror != FNOTPRES ) {
//      throw buffer_exception ( _ferror, format, ap );
//    }
//
//    va_end ( ap );
//
//    return _ferror;
//  }

// operators --------------------------------------------------------------------------------

  ostream &operator<< ( ostream &out, Tfield<string> &f ){

    return out << (string)f;
  };

}

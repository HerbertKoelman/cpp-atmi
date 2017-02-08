/* author: herbert koelman (herbert.koelman@me.com)
 */

#include <iostream>

#include <atmi.h>
#include <fml.h>
#include <memory>
#include <string>
#include <sstream>

#include <atmi/field.hpp>

namespace atmi {

/* fields --------------------------------------------------------------------*/

  field::field(): _field_id(BADFLDID), _field_occurence(0), _field_name(NULL), _ferror(0){
    // intentional
  }

  field::field( FLDID32 id ): _field_id(id), _field_occurence(0), _field_name(NULL), _ferror(0){
    if ( id > 0 ){

      // check that field exists in FML table referenced by FIELDTBLS32 
      // and FLDTBLDIR32
      _field_name = Fname32( id ); 

      if ( ! _field_name.empty() ) { // Fname32 found the field's name
        _field_id = id;
      } else {
        throw field_exception ( Ferror32, "Failed to find the name for field ID %d, check the values of FIELDTBLS32 and FLDTBLDIR32.", id );
      }
    } else {
        throw atmi_exception ( "Bad field id (id: %d).", id );
    }
  }

  field::field( const std::string &name ): _field_id(BADFLDID), _field_occurence(0), _field_name(name), _ferror(0){

    _field_id = (FLDID32) Fldid32 ( const_cast<char *>(name.c_str()) ) ;

    if ( _field_id == BADFLDID ){
      throw field_exception ( Ferror32, "Failed to find the ID of field %s, check the values of FIELDTBLS32 and FLDTBLDIR32.",
          name.c_str() );
    } else {
      // name was already set
    }
  }

  const char *field::tname () {
    return Ftype32 ( _field_id );
  };

  void field::setup( FLDID32 field_id){
    // TODO remove this field::set_id(field_id);
  }

  FLDID32 field::id () const {
    return _field_id;
  };

  const std::string field::name () const{
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

    _field_occurence = (occ < 0) ? 0 : occ;              // check that we don't have a negative occurence
  };

  int field::type () {
    return Fldtype32 ( _field_id );
  };

  const char *field::what() {

    std::stringstream buff;
    buff << "Fid: " << _field_id << ", occurence: " << _field_occurence << ", name: " << _field_name << ", len: " << length() << ", type: " << tname () << ".";
    _what = buff.str();
    return _what.c_str ();
  }

  long field::needed (){

    int rc = Fneeded32 ( 1, ((length() > 0) ? length() : 1 ));
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "FNEEDED32: failed to estimate needed space for field %s (id: %d, occ: %d). Make sure field contains data (length() > 0).",
          _field_name.c_str(),
          _field_id,
          _field_occurence );
    }

    return rc;              // this the size in bytes needed
  };

  int field::remove ( buffer &b) {//NOSONAR this FML grammar and cannot be renamed.

    int rc = Fdel32 ( b._buffer, _field_id, _field_occurence );
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "FDEL32 failed for field %s (id: %d, occ: %d) failed", _field_name.c_str(), _field_id, _field_occurence );
    }

    return rc;
  };

}

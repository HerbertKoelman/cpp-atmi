/*
   $Id$
 */

#include <iostream>

#include <atmi.h>
#include <fml32.h>
#include <string>

#include <atmi/buffer.hpp>

#include "atmi/field.hpp"
#include "atmi/call_info.hpp"

namespace atmi {

/* buffer --------------------------------------------------------------------*/

  buffer::buffer () : _buffer (NULL) {

    _extent    = 1024;
    _allocated = true;
    _buffer    = (FBFR32 *)allocate ( const_cast<char *>(FMLTYPE32), NULL, _extent );
  }

  buffer::buffer ( FBFR32 *b ){
    _allocated = false;
    set_buffer ( b );
  }

  buffer::buffer ( FLDLEN32 len ) : _buffer (NULL) {

    _extent    = len;
    _allocated = true;
    _buffer    = (FBFR32 *)allocate ( const_cast<char *>(FMLTYPE32), NULL, _extent );
  }

  buffer::~buffer () {
    if ( _allocated ) {
      free ( (char *) _buffer );
    }
  }

  void buffer::get_call_info( call_info &callinfo){
    FBFR32 *cib = callinfo ;
    tpgetcallinfo((char *) _buffer, &cib, 0 );
    callinfo._buffer.set_buffer(cib);
  }

  void buffer::set_call_info( call_info &callinfo){
    tpsetcallinfo((char *) _buffer, (FBFR32 *)callinfo, 0 );
  }

  bool buffer::is_fml32_buffer( char *buffer){

    char type[MAXTIDENT+1];
    if ( tptypes ((char *) buffer, type, NULL ) == -1 ) {
      throw atmi_exception ( tpstrerror(tperrno));
    }

    return (strncmp ( FMLTYPE32, type, 8) == 0 );
  }

  field &buffer::set ( field &f ){

    f.set ( *this );
    return f;
  }

  /** add the field into the buffer.
   *
   * @param f the field to add
   */
  field &buffer::add ( field &f ){

    f.add ( *this );

    return f;
  }

  void buffer::pack () {

    resize ( used() );
  };

  void buffer::resize ( size_t extent ){

    _extent = extent;
    _buffer = (FBFR32 *) tuxedo::extend ( (char *) _buffer, _extent );
  };

  void buffer::extend() {

    resize ( size() + _extent );
  };

  size_t buffer::extent() const {
    return _extent;
  }

  void buffer::set_extent( size_t extent ){
    _extent = extent ;
  }

  field &buffer::append ( field &f ){

    throw atmi_exception ( "Method append is not implemented yet !!??;" ); //NOSONAR this desired implementation.
    return f;
  }

  /** remove a field from the buffer
   *
   * @param f field to remove
   */
  void buffer::remove ( field &f ){ //NOSONAR this is a ATMI name it certainly will not change

    f.remove ( *this );
  }

  /** gets the value (if exsists) of passed field.
   *
   * @param f field for which we want to get the value
   */
  field &buffer::get ( field &f, FLDOCC32 occ ) {

    f.get( *this, occ );
    return f;
  };

  /** gets the value (if exsists) of passed field
   *
   * @param f field for which we want to get the value
   */
  field &buffer::get ( field &f ) {

    f.get( *this );
    return f;
  };

  FLDOCC32 buffer::occurences ( const field &f ){

    int rc = Foccur32 ( _buffer, f.id() );
    if ( rc < 0 ) {
      throw buffer_exception ( Ferror32, "Failed to find occurences  of field %s (%d) in buffer.",
          f.name().c_str(),
          f.id() );
    }

    return rc;
  };

  size_t buffer::size () const {
    return ( (_buffer == NULL) ? 0 : Fsizeof32 ( _buffer ));
  };

  size_t buffer::used () const {
    return ((_buffer == NULL)? 0 : Fused32 ( _buffer ));
  };

  size_t buffer::unused () const {
    return ( (_buffer == NULL) ? 0 : Funused32 ( _buffer ));
  };

  long buffer::chksum() {

    long sum = Fchksum32 ( _buffer );
    if ( sum < 0 ) {
      throw buffer_exception (Ferror32, "Fchksum call on buffer failed." );
    }

    return sum;
  };

  FLDOCC32 buffer::field_count() const {

    return Fnum32 ( _buffer );
  };

  size_t buffer::print_buffer_size() const {

    return (field_count() * MAXTIDENT) + used();
  }

  void buffer::print (char *buffer) const {
    Fsprint32(_buffer, buffer);
  }

  void buffer::print () const {

    Fprint32 ( _buffer );
  };

  bool buffer::is_handling_memory(){
    return _allocated;
  }

  void buffer::set_handling_memory ( bool b) {
    _allocated = b;
  }

  /**
   * @return the reference the current internal buffer.
   */
  FBFR32 *buffer::get_buffer () {

    return _buffer;
  };

  void buffer::set_buffer ( FBFR32 *b ) {

    if ( is_fml32_buffer ( (char *)b)) { // this a lazy way to go and will need some update

      _buffer = b;

    } else {
      throw atmi_exception ( "This buffer reference is not of type FMLTYPE32." );
    }

  };

  bool buffer::operator== ( buffer &b) {

    return ( this->chksum () == b.chksum() );
  };

  /** Copy's the content of one buffer into the other.
   *
   * @param b source buffer
   * @return target buffer
   */
  buffer & buffer::operator= ( buffer &b) {

    int rc = -1;

    rc = Fcpy32 ( _buffer, b._buffer );
    if ( rc == -1 ) {
      throw buffer_exception ( Ferror32, "FCOPY32 failed.");
    }
    return *this;
  };

  buffer & buffer::operator= ( FBFR32 *b) {

    int rc = -1;

    rc = Fcpy32 ( _buffer, b );
    if ( rc == -1 ) {
      throw buffer_exception ( Ferror32, "FCOPY32 failed.");
    }
    return *this;
  };

  buffer::operator FBFR32 *(){
    return _buffer;
  }
}

/*
   $Id$
 */

#include <iostream>

#include <stdarg.h>
#include <atmi.h>
#include <fml.h>
#include <fml32.h>
#include <string>

#include <atmi++.h>

using namespace std;

namespace atmi {

/* Buffer --------------------------------------------------------------------*/

  Buffer::Buffer () : buffer (NULL) {

    extent = 1024;
    allocated = true ;
    buffer = (FBFR32 *)allocate ( const_cast<char *>(FMLTYPE32), NULL, extent );
  }

  Buffer::Buffer ( FBFR32 *b ){
      allocated = false ;
      set_buffer ( b );
  }

  Buffer::Buffer ( FLDLEN32 len ) : buffer (NULL) {

    this->extent = len;
    allocated = true ;
    buffer = (FBFR32 *)allocate ( const_cast<char *>(FMLTYPE32), NULL, extent );
  }

  Buffer::~Buffer () {
// cout << "Buffer destructor ... allocated is " << allocated << endl;
    if ( allocated ) {
// cout << "is freeing a buffer..." << endl;
// print();
      free ( (char *) buffer );
    }
  }

  bool Buffer::is_fml32_buffer( FBFR32 *buffer){

    char type[MAXTIDENT+1] ;
    if ( tptypes ((char *) buffer, type, NULL ) == -1 ){
      throw Exception ( tpstrerror(tperrno));
    }

    return (strncmp ( FMLTYPE32, type, 8) == 0 );
  }

  Field *Buffer::set ( Field *f ){

    if ( f == NULL ) {
      throw Exception ("Setting a NULL field !!??");
    } else {
      f->set ( this );
    }
    return f;
  }

/** add the field into the buffer.
 *
 * @param f the field to add
 */
  Field *Buffer::add ( Field *f ){

    if ( f == NULL ) {
      throw Exception ("Adding a NULL field !!??");
    } else {
      f->add ( this );
    }

    return f;
  }

  void Buffer::pack () {

    resize ( used() );
  };

  void Buffer::resize ( long extent ){

    this->extent = extent;
    buffer = (FBFR32 *) Tuxedo::extend ( (char *) buffer, this->extent );
  };

  void Buffer::extend() {

    resize ( size() + extent );
  };

  Field *Buffer::append ( Field *f ){

    throw Exception ( "Method append is not implemented yat !!??;" );
    return NULL;
  }

/** remove a field from the buffer
 *
 * @param f field to remove
 */
  void Buffer::remove ( Field *f ){

    if ( f != NULL ) {
      f->remove ( this );
    }else {
      throw Exception ( "Remove failed. You passed a NULL pointer as field !!" );
    }
  }

/** gets the value (if exsists) of passed field
 *
 * @param f field for which we want to get the value
 */
  Field *Buffer::get ( Field *f, FLDOCC32 occ ) {

    if ( f != NULL ) {
      f->get( this, occ );
    } else {
      throw Exception ( "Buffer get failed. You passed a NULL pointer as field !!" );
    }
    return f;
  };

/** gets the value (if exsists) of passed field
 *
 * @param f field for which we want to get the value
 */
  Field *Buffer::get ( Field *f ) {

    if ( f != NULL ) {
      f->get( this );
    } else {
      throw Exception ( "Buffer get failed. You passed a NULL pointer as field !!" );
    }
    return f;
  };

  /** returns the number of occurence of the given field into the buffer
   *
   * @param f a field for which to search for occurences
   */
  FLDOCC32 Buffer::occurences ( const Field *f ){

    int rc = Foccur32 ( buffer, f->fid );
    if ( rc < 0 ) {
      throw BufferException ( Ferror32, "Get field occurences in buffer failed for %s (%d).", f->fname, f->fid );
    }

    return rc;
  };

  /** 
   * @return the size of the buffer (in bytes)
   */
  long Buffer::size () {
    return ( buffer == NULL ? 0 : Fsizeof32 ( buffer ));
  };

  /** @return number of bytes stored into the buffer */
  long Buffer::used () {
    return ( buffer == NULL ? 0 : Fused32 ( buffer ));
  };

  /** @return number of bytes not yet used into the buffer */
  long Buffer::unused () {
    return ( buffer == NULL ? 0 : Funused32 ( buffer ));
  };

  /**
   * @return the checksum value of the current buffer content
   */
  long Buffer::chksum() {

    long sum = Fchksum32 ( buffer );
    if ( sum < 0 ) {
      throw BufferException (Ferror32, "Fchksum call on buffer failed." );
    }

    return sum;
  };

  /**
   * @return  return count of all occurrences in buffer
   */
  FLDOCC32 Buffer::num() {

    return Fnum32 ( buffer );
  };

  /** print's on stdout the content of the buffer (debugging purpose mainly)
   */
  void Buffer::print () {

    Fprint32 ( buffer );
  };

  bool Buffer::is_handling_memory(){
    return allocated;
  }

  void Buffer::set_handling_memory ( bool b) {
    allocated = b ;
  }

  /**
   * @return the reference the current internal buffer.
   */
  FBFR32 *Buffer::get_buffer () {

    return buffer;
  };

  /** replaces the current buffer reference by the given one.
   *
   * If preveous reference was allocated by this instance, then the buffer is freed before setting the new buffer reference. 
   * The buffer instance is then flagged as not allocated. Meaning that it's up to you to free the memory that was allocated.
   *
   * @param b reference of an FML buffer.
   * @throws Exception if the buffer is not an FML buffer.
   */
  void Buffer::set_buffer ( FBFR32 *b ) {

    if ( is_fml32_buffer (b)){

      // if ( allocated ) { /* check if we are in charge of handling memory allocation */
      //   free ( (char *) buffer );
      // }

      //allocated = false;
      buffer = b;

    } else {
      throw Exception ( "This buffer reference is not of type FMLTYPE32." );
    }

    /* moved into static method is_fml32_buffer
    char type[9];
    if ( tptypes ( (char *) b, type, NULL) > 0 ) {
      if ( strcmp ( FMLTYPE32, type ) == 0 ) {

        free ( (char *) buffer );
        buffer = b;

      } else {
        throw Exception ( "This buffer reference is not of type FMLTYPE32." );
      }
    }else {

      throw TuxedoException ( tperrno, "Method set_buffer failed to get buffer type (tptypes)." );
    }
    */
  };

  /** @return true if both buffer's checksums were equal
   */
  bool Buffer::operator== ( Buffer &b) {

    return ( this->chksum () == b.chksum() );
  };

  /** Copy's the content of one buffer into the other.
   *
   * @param b source buffer
   * @return target buffer
   */
  Buffer & Buffer::operator= ( Buffer &b) {

    int rc = -1;

    rc = Fcpy32 ( this->buffer, b.buffer );
    if ( rc == -1 ) {
      throw BufferException ( Ferror32, "FCOPY32 failed.");
    }
    return *this;
  };
}

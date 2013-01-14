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

  Buffer::Buffer () : buffer (NULL), extent (0)  {

    extent = 1024;
    buffer = (FBFR32 *)allocate ( const_cast<char *>(FMLTYPE32), NULL, extent );
  }

  Buffer::Buffer ( FBFR32 *b ){
    // check that we got an FML buffer
    char type[9];
    if ( tptypes ( (char *) b, type, NULL) > 0 ) {
      if ( strcmp ( FMLTYPE32, type ) == 0 ) {

        buffer = b;

      } else {

        throw Exception ( "This buffer reference is not of type FMLTYPE32." );
      }
    }else {

      throw TuxedoException ( tperrno, "Method set_buffer failed to get buffer type (tptypes)." );
    }
  }

  Buffer::Buffer ( FLDLEN32 len ) : buffer (NULL) {

    this->extent = len;
    buffer = (FBFR32 *)allocate ( const_cast<char *>(FMLTYPE32), NULL, extent );
  }

  Buffer::~Buffer () {
    free ( (char *) buffer );
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

/*
   AField Buffer::get ( FLDID32 fid ){

        AField f = Buffer::field_factory ( fid );
        get ( f.get () );

        return f ;
   };
 */

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

  long Buffer::chksum() {

    long sum = Fchksum32 ( buffer );
    if ( sum < 0 ) {
      throw BufferException (Ferror32, "Fchksum call on buffer failed." );
    }

    return sum;
  };

  FLDOCC32 Buffer::num() {

    return Fnum32 ( buffer );
  };

  void Buffer::print () {

    Fprint32 ( buffer );
  };

  FBFR32 *Buffer::get_buffer () {

    return buffer;
  };

  void Buffer::set_buffer ( FBFR32 *b ) {

    // check that we got an FML buffer
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
  };

  bool Buffer::operator== ( Buffer &b) {

    return ( this->chksum () == b.chksum() );
  };

  Buffer & Buffer::operator= ( Buffer &b) {

    int rc = -1;

    rc = Fcpy32 ( this->buffer, b.buffer );
    if ( rc == -1 ) {
      throw BufferException ( Ferror32, "FCOPY32 failed.");
    }
    return *this;
  };

/* we'll see this later
   AField Buffer::field_factory ( FLDID32 fid ){

        AField pf ;

        int ftype =  Fldtype32 ( fid );
   switch ( ftype ){

        case FLD_SHORT:
                pf.reset ( new TField<short>(fid) );
                break;

        case FLD_LONG:
                pf.reset ( new TField<long>(fid) );
                break;

        case FLD_CHAR:
                pf.reset ( new TField<char>(fid) );
                break;

        case FLD_FLOAT:
                pf.reset ( new TField<float>(fid) );
                break;

        case FLD_DOUBLE:
                pf.reset ( new TField<double>(fid) );
                break;

        case FLD_STRING:
                pf.reset ( new TField<string>(fid) );
                break;

        default:
                throw Exception ( "Unsupported field type %d." , ftype );
   };

        return pf ;
   };
 */

}

/*
   $Id$

   Helper class for dealing with FML buffer and fields.
 */

/*
 * Buffers
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * Tuxedo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Logging Tuxedo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tuxedo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */


#ifndef __BUFFERS__
#define __BUFFERS__

#include <typeinfo>
#include <fml.h>
#include <fml32.h>
#include <string>
#include <string.h>
#include <memory>

using namespace std;

namespace atmi {

//----------------------------------------------------------------------------------------------
  class Field;

  typedef char * Carray;
  typedef auto_ptr<Carray> ACarray;

 // class Tuxedo;

  /**
   * FML buffer
	 * 
	 * This class handles the content of FML buffer
   */
  class Buffer : public Tuxedo {


public:

    friend class Field;

    /** Allocates a buffer of 1024bytes */
    Buffer ();

    /** Allocates a buffer of byte size.
     * @param len bytes space of field value in bytres
     */
    Buffer ( FLDLEN32 len );

    /** Allocates a buffer to fit the number of occurencies of bytes size.
     * @param occ number of fields
     * @param bytes space of field value in bytres
     */
    Buffer ( FLDOCC32 occ, FLDLEN32 bytes );

    /** default destructor */
    ~Buffer ();

    /** @return the size of the buffer (in bytes) */
    long size ();

    /** @return number of bytes stored into the buffer */
    long used ();

    /** @return number of bytes not yet used into the buffer */
    long unused ();


    /** Removes unused space allocation
     */
    void pack ();

    /** Extends the buffer size with the given bytes
     *
     */
    void extend ();

    /** Extends the buffer size with the given bytes
     *
     * @param extent size in bytes of the extent
     * @throw BufferException upon failure
     */
    void resize ( long extent );

    /** Set a field's value into the buffer
     *
     * @param f a pointer to the field for which to change the value
     * @throw BufferException upon failure
     */
    Field *set    ( Field *f );

    /** Adds a field into the buffer
     *
     * @param f a pointer to the field to add into the buffer
     * @throw BufferException upon failure
     */
    Field *add    ( Field *f );           // FLDID fieldid, char *value, FLDLEN len );

    /** Appends a field to the buffer
     *
     * @param f a pointer to the field to add into the buffer
     */
    Field *append ( Field *f );           // FLDID fieldid, char *value, FLDLEN len );

    /** Removes the field from the buffer
     *
     * @param f a pointer to the field to remove
     * @throw BufferException upon failure
     */
    void remove ( Field *f );           // FLDID fieldid );

    /** Get the field value set in the buffer
     * @param f the field to set
     * @return the field when set
     * @throw BufferException upon failure
     */
    Field *get ( Field *f );

    /** Get the field value set in the buffer
     * @param f the field to set
     * @param occ the field occurence to set
     * @return the field when set
     * @throw BufferException upon failure
     */
    Field *get ( Field *f, FLDOCC32 occ);

    /** @return the current check sum of the buffer
     *
     * @throw BufferException when calcutaion fails.
     */
    long chksum();

    /** @return the number of occurences of the field into the buffer */
    FLDOCC32 occurences ( const Field *f );

    /** @return the number of fields into the buffer */
    FLDOCC32 num ();

    /** print on stdout the content of the buffer. Mainly used for debugging
     */
    void print ();

    /** @return return FML32 buffer reference
     */
    FBFR32 * get_buffer ();

    /** Set a new buffer reference. Previous buffer is deallocated before assign the passed value.
     * @param b set FML32 buffer reference
     */
    void set_buffer ( FBFR32 *b );

    // operators -------------------------------------------------------------------------------

    /** Checks equality of two buffers (based upon chksum)
     * @return true if both checksums are equal.
     */
    bool operator== ( Buffer &);

    /** Copies the content of a fielded buffer into another
     */
    Buffer &operator= (Buffer &);

private:

    /** FML buffer reference */
    FBFR32 *buffer;

    /** value by which the buffer size will be extended */
    long extent;
  };

// ---------------------------------------------------------------------------------


//  class Buffer;

/** Abstract class to manipulate a Field
 */
  class Field {

public:

    friend class Buffer;

    /** Default destructor
     */
    virtual ~Field () {
    };

    /**
     *  Possible values are:
     *  FLD_SHORT       0       short int
     *  FLD_LONG        1       long int
     *  FLD_CHAR        2       character
     *  FLD_FLOAT       3       single-precision float
     *  FLD_DOUBLE      4       double-precision float
     *  FLD_STRING      5       string - null terminated
     *  FLD_CARRAY      6       character array
     *  FLD_PTR         9       pointer to a buffer
     *  FLD_FML32      10       embedded FML32 buffer
     *  FLD_VIEW32     11       embedded VIEW32 buffer
     *  FLD_MBSTRING   12       multibyte character array
     *
     * @return the field's type
     */
    int type ();

    /** @return the name of the fml field type
     */
    const char * tname ();

    /** @return the Field ID of the field
     */
    FLDID32 id ();

    /** Extracts the field number from the field identifier.
     *
     * @return field number.
     */
    int number ();

    /** @return the name of the field */
    const char *name ();

    /** @return the fields occurence (as last found in a buffer)*/
    FLDOCC32 occurence ();

    /** @return the last Ferror32 value */
    int error ();

    /** @return the length of the field's value
     */
    virtual FLDLEN32  length () = 0;

    virtual long needed ();

    /** @return a string describing the field
     */
    const char * what ();

    virtual Field &operator= ( const string & ) {
      return *this;
    };

protected:

    int handleFerror(int, const char *, ...);

    virtual void setFocc ( FLDOCC32 occ );

    /** utility method to setup field's data
     */
    virtual void setup ( FLDID32 fid );

    /** Retrieves the value of the field found into the buffer
     *
     * @param b buffer from which to retrieve the field's value
     */
    virtual int get ( Buffer *b ) = 0;

    /** Retrieves the value of the field's occurence found into the buffer
     *
     * Upon success the value of focc is set to the retrieved occurence.
     *
     * @param b buffer from which to retrieve the field's value
     * @param occ occurence to retreive
     */
    virtual int get ( Buffer *b, FLDOCC32 occ ) = 0;

    /** add the fiels into the buffer
     *
     * When successfull the value of focc is set
     *
     * @param b buffer in which to add the field.
     */
    virtual int add ( Buffer *b ) = 0;

    /** set the value of the field's value
     *
     * @param b the buffer in which the value must be changed
     */
    virtual int set ( Buffer *b ) = 0;

    /** removes the field from the buffer
     *
     * @param b the buffer from which to remove the field
     */
    virtual int remove ( Buffer *b );

private:
    FLDID32 fid;    // Field ID
    FLDOCC32 focc;                      // Field occurence
    char *fname;
    int last_err;
  };

/** Template that handles non pointer data types (short, long, char, double, ...)
 *
 */
  template <class T> class TField : public Field {
public:

    /** Constructs a TField for the passed field id
     *
     * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
     *
     * @param fid the fml field id to setup (as defined in the FML tables)
     */
    TField ( FLDID32 fid ) {

      setup ( fid );

      // check type matching
      if ( type() > 5 ) {
        throw Exception ( "This template doesn't support the give type TField<%s>.", tname ());
      } else if ( strcmp (typeid(this->value).name (), tname ()) != 0 ) {
        throw Exception ( "TField %s's value is of type %s and the FML table decalares a type %s.", name (), typeid(this->value).name (), tname());
      }
    }

    /** Constructs a TField for the passed name
     *
     * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
     *
     * @param n the fml field name to setup (as defined in the FML tables)
     */
    TField ( char *n ) {

      setup ( (FLDID32) Fldid32 ( n ) );

      // check type matching
      if ( strcmp (typeid(this->value).name (), tname ()) != 0 ) {
        throw Exception ( "TField value is of type %s and the FML table decalares a type %s for %s.", typeid(this->value).name (), tname(), name());
      }
    }

    virtual ~TField() {
    };

    /** @return de length (or size) of the field's data
     */
    virtual FLDLEN32 length () {

      return sizeof ( value );
    };

    /** Assigns a value to the field
     * ...
     * TField<int> f (fid);
     * f = 156 ; // Set value to 156
     * ...
     */
    T operator= ( T v) {

      value = v;

      return *this;
    };

    /** casts the field value
     * ...
     * TField<int> f (fid);
     * f = 156 ;
     * int x = f ; // returns 156
     * ...
     */
    operator T(){

      return value;
    };

protected:

    virtual int set ( Buffer *b) {

      int rc = -1;

      rc = Fchg32 ( b->get_buffer(), id(), occurence(), (char *) &value, length() );
      if ( rc < 0 ) {
        throw BufferException ( Ferror32, "FCHG32 TField::set failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
      }

      return rc;
    };

    virtual int add ( Buffer *b) {

      int rc = -1;
      try {
        if ( (needed () + b->used()) >= b->size()) {
          b->extend ();
        }
      } catch ( BufferException buffErr ) {
        throw  Exception ( "Add failed to estimate needed memory extension. Original message was : %s", buffErr.what() );
      };

      rc = Fadd32 ( b->get_buffer(), id(), (char *) &value, length() );
      if ( rc < 0 ) {
        throw BufferException ( Ferror32, "FADD32 TField::add failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
      } else {
        setFocc ( (b->occurences ( this ) == 0 ? 0 : b->occurences ( this )-1));
      }

      return rc;
    };

    virtual int get ( Buffer *b ){

      return get ( b, occurence() );
    };

    virtual int get ( Buffer *b, FLDOCC32 occ ){

      int rc = -1;
      FLDLEN32 l = length();
      setFocc (occ);

      rc = Fget32 ( b->get_buffer(), id(), occurence(), (char *) &value, &l );
      if ( rc == -1 ) {
        throw BufferException (Ferror32, "FGET32 failed to get field %s (id: %d, occ: %d).", name(), id(), occurence() );
      }

      return rc;
    };

    T value;
  };

/** Specialization of template TField which handles string typed fields.
 *
 * This class is using a string object to hold and handle string data
 */
  template <> class TField<string>: public Field {
public:

    /** Constructs a TField for the passed field id
     *
     * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
     *
     * @param fid the fml field id to setup (as defined in the FML tables)
     */
    TField ( FLDID32 fid ) {

      setup ( fid );

      // check taht we have a string declaration in the FML table
      if ( type () != 5 ) {
        throw Exception ( "TField %s's value is of type string and the FML table decalares a type %s.", name (), tname());
      }
    }

    /** Constructs a TField for the passed name
     *
     * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
     *
     * @param n the fml field name to setup (as defined in the FML tables)
     */
    TField ( char *n ) {

      setup ( (FLDID32) Fldid32 ( n ) );

      // check type matching
      if ( type() !=5 ) {
        throw Exception ( "TField value is of type string and the FML table decalares a type %s for %s.", tname(), name());
      }
    }

    virtual ~TField() {
    };

    /** @return returns the string's length
     */
    virtual FLDLEN32 length () {

      return (FLDLEN32)value.length();
    };

    virtual TField<string> &operator= ( const char* v ) {

      value = v;
      return *this;
    };

    /** Assigns a value to the field
     */
    virtual TField<string> &operator= ( const string &v ) {

      value = v;

      return *this;
    };

    /** casts the field value
     */
    operator string(){

      return value;
    };


protected:

    virtual int set ( Buffer *b) {

      int rc = -1;

      rc = Fchg32 ( b->get_buffer(), id(), occurence(), (char *) value.c_str(), length() );
      if ( rc < 0 ) {
        throw BufferException ( Ferror32, "FCHG32 TField::set failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
      }

      return rc;
    };

    virtual int add ( Buffer *b) {

      int rc = -1;
      if ( length () > 0 ) {
        try {
          if ( (needed () + b->used()) >= b->size()) {
            b->extend ();
          }
        } catch ( BufferException buffErr ) {
          throw  Exception ( "Add failed to estimate needed memory extension. Original message was : %s", buffErr.what() );
        };

        rc = Fadd32 ( b->get_buffer(), id(), (char *) value.c_str(), length() );
        if ( rc < 0 ) {
          throw BufferException ( Ferror32, "FADD32 TField::add failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
        } else {
          setFocc ( (b->occurences ( this ) == 0 ? 0 : b->occurences ( this )-1));
        }
      } else {
        throw Exception ("Field %s's value is empty !!?? Cannot add an empty field value.", name ());
      }

      return rc;
    };

    virtual int get ( Buffer *b ){

      return get ( b, occurence() );
    };

    virtual int get ( Buffer *b, FLDOCC32 occ ){

      int rc = -1;
      FLDLEN32 l = 0;
      char *v = NULL;
      setFocc(occ);

      v = Fgetsa32 ( b->get_buffer(), id(), occurence(), &l );
      if ( v != NULL ) {

        rc=0;
        value = v;                      // copy v into string value
        delete v;

      } else {
        throw BufferException (Ferror32, "FGETSA32 TField::get failed to get field %s (id: %d, occ: %d).", name(), id(), occurence() );
      }

      return rc;
    };

    string value;
  };

/** Specialization of template TField which handles CARRAY typed fields.
 *
 * This class is using a string object to hold and handle char * data
 */
  template <> class TField<char *>: public Field {
public:

    /** Constructs a TField for the passed field id
     *
     * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
     *
     * @param fid the fml field id to setup (as defined in the FML tables)
     */
    TField ( FLDID32 fid ) : len (0), value (NULL) {

      setup ( fid );

      // check taht we have a string declaration in the FML table
      if ( type () != 6 ) {
        throw Exception ( "TField %s's value is of type carray and the FML table decalares a type %s.", name (), tname());
      }
    }

    /** Constructs a TField for the passed name
     *
     * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
     *
     * @param n the fml field name to setup (as defined in the FML tables)
     */
    TField ( char *n ) : len (0), value (NULL) {

      setup ( (FLDID32) Fldid32 ( n ) );

      // check type matching
      if ( type() != 6 ) {
        throw Exception ( "TField value is of type carray and the FML table decalares a type %s for %s.", tname(), name());
      }
    }

    virtual ~TField() {

      if ( value != NULL) delete[] value;
    };

    /** @return returns the size of the carray
     */
    virtual FLDLEN32 length () {

      return len;
    };

    /** Assigns a value to the field
     */
    void setCarray ( const char * c, long s ) {

      if ( value == NULL ) delete value;

      len = s;
      value = new char [len];
      memcpy ( value, c, len );
    };

    /**
     * @return new allocated  pointer to the value of the carray
       virtual ACarray getCarrayAlloc () {

            Carray carray = new char[length()];
            memcpy ( carray, value, length () );

       return new ACarray ( &carray );
       };
     */

    /** copy (memcpy) the field's value into carray. If size is smaller than the length of the field,
     * then only size characters are copied. If size is bigger then the field's length, then length characters
     * are copied.
     *
     * @param carray pointer to a character buffer
     * @param size size of the buffer
     */
    virtual void getCarray ( char *carray, long size ) {

      memcpy ( carray, value, ( size < length () ? size : length() ) );
    };

    /** copy the field's value
     */
    TField<char *> &operator= ( TField<char*> &carray ){

      setCarray ( carray.value, carray.len );

      return *this;
    };

protected:

    virtual int set ( Buffer *b) {

      int rc = -1;

      rc = Fchg32 ( b->get_buffer(), id(), occurence(), value, length() );
      if ( rc < 0 ) {
        throw BufferException ( Ferror32, "FCHG32 TField::set failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
      }

      return rc;
    };

    virtual int add ( Buffer *b) {

      int rc = -1;
      if ( length () > 0 ) {
        try {
          if ( (needed () + b->used()) >= b->size()) {
            b->extend ();
          }
        } catch ( BufferException buffErr ) {
          throw  Exception ( "Add failed to estimate needed memory extension. Original message was : %s", buffErr.what() );
        };

        rc  = Fadd32 ( b->get_buffer(), id(), value, length() );
        if ( rc < 0 ) {
          throw BufferException ( Ferror32, "FADD32 TField::add failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
        } else {
          setFocc ( (b->occurences ( this ) == 0 ? 0 : b->occurences ( this )-1));
        }

      } else {
        throw Exception ( "Field %s's value is empty !!?? Cannot add an empty field value.", name () );
      }

      return rc;
    };

    virtual int get ( Buffer *b ){

      return get ( b, occurence() );
    };

    virtual int get ( Buffer *b, FLDOCC32 occ ){

      int rc = -1;
      FLDLEN32 l = 0;
      setFocc(occ);
      char *v = NULL;

      v = Fgetalloc32 ( b->get_buffer(), id(), occurence(), &l );
      if ( v != NULL ) {

        len = l;
        if ( value != NULL ) delete[] value;

        value = new char[len];
        memcpy (value, v, len);

        rc = 0;
        delete[] v;

      } else {
        throw BufferException (Ferror32, "FGETSA32 TField::get failed to get field %s (id: %d, occ: %d).", name(), id(), occurence() );
      }

      return rc;
    };

    FLDLEN32 len;
    char    *value;
  };

/** Helper that handles the operator << between output streams and field value */
  ostream &operator<< ( ostream &o, TField<string> &f );


}
#endif
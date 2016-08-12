/*
 * FML manipulation classes
 *
 * author: herbert koelman herbert.koelman@me.com
 * creation date: 1/1/2006
 */

#include <fml.h>
#include <fml32.h>
#include <typeinfo>
#include <string>
#include <cstring>
#include <memory>

#ifndef CPP_ATMI_FIELDS_HPP
#define CPP_ATMI_FIELDS_HPP

#include <atmi/exceptions.hpp>
#include <atmi/buffer.hpp> // this MUST be here to avoid recursive include

using namespace std;

namespace atmi {

/** \addtogroup fml
 *
 * @{
 */

  /** 
   * When a new instance is created, occurence is set to 0 making it possible to set values withour a prior call to add.
   */
  class field {

    public:

      friend class buffer;

      /** default constructor
       *
       * default occurence value is 0.
       */
      field();

      /** Default destructor
       */
      virtual ~field () {
        // Intentionally unimplemented...
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

      /** @return the field ID of the field
       */
      FLDID32 id () const;

      /** this will set the current field ID
       *
       * @param id field ID
       * @since v4.2.1
       */
      virtual void set_id(FLDID32 id);

      /** Extracts the field number from the field identifier.
       *
       * @return field number.
       */
      int number ();

      /** @return the name of the field */
      const char *name ();

      /** @return the fields occurence (as last found in a buffer)*/
      FLDOCC32 occurence ();

      /** @return the last Ferror32 value that was returned by the FML library */
      int error ();

      /** @return the length of the field's value
       */
      virtual FLDLEN32  length () = 0;

      /** @return number bytes needed to store the fields value */
      virtual long needed ();

      /** @return a string describing the field
       */
      const char *what ();

      /** @param value value to assign to the field.
       */
      virtual field &operator= ( const string &value ) {
        return *this;
      };

    protected:

      /* Field manipulation error handler.
       *
       * @param ferror Ferror32 value
       * @param format error message formatting string.
       * @param ...    error message viariadic
       * @return the handled ferror value.
       * @deprecated TIDO remove this
       */
      // int ferror_handler(int ferror, const char *format, ...);

      /** set the field occurence.
       *
       * Occurences start counting from 0.
       *
       * @param occ occurence.
       */
      virtual void set_field_occurence ( FLDOCC32 occ );

      /** utility method to setup field's data.
       *
       * checks if field exists, if so it fetch all available metadata (name, type, ...)
       *
       * @param field_id field identifier.
       * @deprecated replaced by atmi::field.set_id()
       */
      virtual void setup ( FLDID32 field_id );

      /** Retrieves the value of the field found into the buffer.
       *
       * @param b buffer from which to retrieve the field's value
       */
      virtual int get ( buffer &b ) = 0;

      /** Retrieves the value of the field's occurence found into the buffer
       *
       * Upon success the value of occurence is set to the retrieved occurence.
       *
       * @param b buffer from which to retrieve the field's value
       * @param occ occurence to retreive
       * @see occurence
       */
      virtual int get ( buffer &b, FLDOCC32 occ ) = 0;

      /** add the fiels into the buffer
       *
       * When successfull the value of occurence is set
       *
       * @param b buffer in which to add the field.
       * @see occurence
       */
      virtual int add ( buffer &b ) = 0;

      /** set the value of the field's value
       *
       * @param b the buffer in which the value must be changed
       */
      virtual int set ( buffer &b ) = 0;

      /** removes the field from the buffer
       *
       * @param b the buffer from which to remove the field
       */
      virtual int remove ( buffer &b );

    private:

      FLDID32   _field_id;
      FLDOCC32  _field_occurence;
      char     *_field_name;
      int       _ferror;
      std::string _what; // used to return a desciption
  };

  /** Template that handles non pointer data types (short, long, char, double, ...)
   * \copydoc atmi::field
   */
  template <class T> class Tfield : public field {
    public:

      /** default constructor.
       *
       * The instance cannot be used until set_field_id is successfully called.
       *
       * @since v4.2.0
       */
      Tfield(): value(0){};

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param field_id the fml field id to setup (as defined in the FML tables)
       */
      explicit Tfield ( FLDID32 field_id ){
        set_id(field_id);
        value = 0; // wa are now sure that this is a number
      }

      /** Constructs a Tfield for the passed name
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param name the fml field name to setup (as defined in the FML tables)
       */
      explicit Tfield ( const char *name ) {

        set_id ( (FLDID32) Fldid32 ( const_cast<char *>(name) ) );
        value = 0; // wa are now sure that this is a number

        // check type matching
        if ( strcmp (typeid(this->value).name (), tname ()) != 0 ) {
          throw atmi_exception ( "Tfield value is of type %s and the FML table decalares a type %s for %s.", typeid(this->value).name (), tname(), this->name());
        }
      }

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param field_id the fml field id to setup (as defined in the FML tables)
       */
      virtual void set_id(FLDID32 field_id){

        /** This array is used to check that FML type matches template Tfield's type */
        const char *TYPEID_NAMES[5] = {
          typeid(short).name(),
          typeid(long).name(),
          typeid(char).name(),
          typeid(float).name(),
          typeid(double).name()
        };

        field::set_id ( field_id );

        // check if type is matching
        if ( type() > 5 ) {
          throw atmi_exception ( "This template doesn't support the given type Tfield<%s>.", tname ());
        } else if ( strcmp (typeid(this->value).name (), TYPEID_NAMES[type()]) != 0 ) {
          throw atmi_exception ( "Tfield %s's value is of type %s and the FML table decalares a type %s.", name (), typeid(this->value).name (), tname());
        }
      };

      virtual ~Tfield() {
        // Intentionally unimplemented...
      };

      /** @return de length (or size) of the field's data
       */
      virtual FLDLEN32 length () {

        return sizeof ( value );
      };

      /** Assigns a value to the field
       * ...
       * Tfield<int> f (fid);
       * f = 156 ; // Set value to 156
       * ...
       */
      T operator= ( T v) {

        value = v;

        return *this;
      };

      /** casts the field value
       * ...
       * Tfield<int> f (fid);
       * f = 156 ;
       * int x = f ; // returns 156
       * ...
       */
      operator T(){

        return value;
      };

    protected:

      virtual int set ( buffer &b) {

        int rc = -1;

        rc = Fchg32 ( b.get_buffer(), id(), occurence(), (char *) &value, length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FCHG32 Tfield::set failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
        }

        return rc;
      };

      virtual int add ( buffer &b) {

        int rc = -1;
        try {
          if ( (needed () + b.used()) >= b.size()) {
            b.extend ();
          }
        } catch ( buffer_exception &buffErr ) {
          throw  atmi_exception ( "Add failed to estimate needed memory extension. Original message was : %s", buffErr.what() );
        };

        rc = Fadd32 ( b.get_buffer(), id(), (char *) &value, length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FADD32 Tfield::add failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
        } else {
          set_field_occurence ( (b.occurences ( *this ) == 0 ? 0 : b.occurences ( *this )-1));
        }

        return rc;
      };

      virtual int get ( buffer &b ){

        return get ( b, occurence() );
      };

      virtual int get ( buffer &b, FLDOCC32 occ ){

        int rc = -1;
        FLDLEN32 l = length();
        set_field_occurence (occ);

        rc = Fget32 ( b.get_buffer(), id(), occurence(), (char *) &value, &l );
        if ( rc == -1 ) {
          throw buffer_exception (Ferror32, "FGET32 failed to get field %s (id: %d, occ: %d).", name(), id(), occurence() );
        }

        return rc;
      };

      T value; //!< field's current value

  };


  /** Specialization of template Tfield which handles string typed fields.
   *
   * This class is using a string object to hold and handle string data
   * \copydoc atmi::field
   */
  template <> class Tfield<std::string>: public field {
    public:

      Tfield(){
        // intentional
      };

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param fid the fml field id to setup (as defined in the FML tables)
       */
      explicit Tfield ( FLDID32 fid ) {

        set_id ( fid );

        // check taht we have a string declaration in the FML table
        if ( type () != 5 ) {
          throw atmi_exception ( "Tfield %s's value is of type string and the FML table decalares a type %s.", name (), tname());
        }
      }

      /** Constructs a Tfield for the passed name
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param n the fml field name to setup (as defined in the FML tables)
       */
      explicit Tfield ( const char *n ) {

        set_id ( (FLDID32) Fldid32 ( const_cast<char *>(n) ) );

        // check type matching
        if ( type() !=5 ) {
          throw atmi_exception ( "Tfield value is of type string and the FML table decalares a type %s for %s.", tname(), name());
        }
      }

      virtual ~Tfield() {
        // Intentionally unimplemented...
      };

      /** @return the string's length
       */
      virtual FLDLEN32 length () {

        return (FLDLEN32)value.length();
      };

      /** @return the string's size
       */
      virtual FLDLEN32 size () {

        return (FLDLEN32)value.size();
      };

      /** @return a C string (with \0) of current string's content.
       */
      const char* c_str ( ) const {
        return value.c_str();
      }

      /** Appends a copy of the argument to the string.
       *
       * The new string content is the content existing in the string object before the call followed by the content of
       * the argument.
       *
       * The append member function provides a similar functionality with additional options.
       *
       * @param str a copy of the content of this object is appended to the object's content.
       * @return *this
       */
      string& operator+= ( const string& str ){

        return value += str;
      }

      /** Appends a copy of the argument to the string.
       *
       * The new string content is the content existing in the string object before the call followed by the content of
       * the argument.
       *
       * The append member function provides a similar functionality with additional options.
       *
       * @param  s  a pointer to an array containing a null-terminated character sequence (C string), which is appended to the object's content.
       * @return *this
       */
      string& operator+= ( const char* s ){

        return value += s;
      }

      /** Appends character to the string field.
       *
       * @param c   character. This single character is appended to the string object's content.
       * @return *this
       */
      string& operator+= ( char c ){
        return value += c;
      }

      /**
       * Returns a reference the character at position pos in the string.
       *
       * The function actually returns data()[ pos ].
       *
       * The at member function has the same behavior as this operator function, except that at also performs a range
       * check.
       *
       * @param pos position within the string of the character to be retrieved. Notice that the first character in the string has a position of 0, not 1. size_t is an unsigned integral type.
       * @return The character at the specified position in the string.
       */
      const char& operator[] ( size_t pos ) const {

        return value[pos];
      }

      /**
       * Returns a reference the character at position pos in the string.
       *
       * The function actually returns data()[ pos ].
       *
       * The at member function has the same behavior as this operator function, except that at also performs a range
       * check.
       *
       * @param pos position within the string of the character to be retrieved. Notice that the first character in the string has a position of 0, not 1. size_t is an unsigned integral type.
       * @return The character at the specified position in the string.
       */
      char& operator[] ( size_t pos ){

        return value[pos];
      }

      /**
       * assigns values to the string field.
       *
       * @param str   null terminated character array.
       */
      virtual Tfield<string> &operator= ( const char* str ) {

        value = str;
        return *this;
      };

      /**
       * assigns values to the string field.
       *
       * @param str string to assign
       */
      virtual Tfield<string> &operator= ( const string &str ) {

        value = str;

        return *this;
      };

      /**
       * assigns values to the string field.
       *
       * @param  c  the content is set to a single character.
       */
      virtual Tfield<string> &operator= ( char &c ) {

        value = c;

        return *this;
      };

      /** casts the field value
       */
      operator string(){

        return value;
      };

    protected:

      virtual int set ( buffer &b) {

        int rc = -1;

        rc = Fchg32 ( b.get_buffer(), id(), occurence(), (char *) value.c_str(), length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FCHG32 Tfield::set failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
        }

        return rc;
      };

      virtual int add ( buffer &b) {

        int rc = -1;
        if ( length () > 0 ) {
          try {
            if ( (needed () + b.used()) >= b.size()) {
              b.extend ();
            }
          } catch ( buffer_exception &buffErr ) {
            throw  atmi_exception ( "Add failed to estimate needed memory extension. Original message was : %s", buffErr.what() );
          };

          rc = Fadd32 ( b.get_buffer(), id(), (char *) value.c_str(), length() );
          if ( rc < 0 ) {
            throw buffer_exception ( Ferror32, "FADD32 Tfield::add failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
          } else {
            set_field_occurence ( (b.occurences ( *this ) == 0 ? 0 : b.occurences ( *this )-1));
          }
        } else {
          throw atmi_exception ("field %s's value is empty !!?? Cannot add an empty field value.", name ());
        }

        return rc;
      };

      virtual int get ( buffer &b ){

        return get ( b, occurence() );
      };

      virtual int get ( buffer &b, FLDOCC32 occ ){

        int rc = -1;
        FLDLEN32 l = 0;
        char *v = NULL;
        set_field_occurence(occ);

        v = Fgetsa32 ( b.get_buffer(), id(), occurence(), &l );
        if ( v != NULL ) {

          rc=0;
          value = v;                    // copy v into string value
          delete v;

        } else {
          throw buffer_exception (Ferror32, "FGETSA32 Tfield::get failed to get field %s (id: %d, occ: %d).", name(), id(), occurence() );
        }

        return rc;
      };

      string value; //!< string field's value
  };

  /** Specialization of template Tfield which handles CARRAY typed fields.
   *
   * This class is using a string object to hold and handle char * data
   * \copydoc atmi::field
   */
  template <> class Tfield<char *>: public field {
    public:

      Tfield(): len (0), value (NULL) {
        // intentional
      };

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param fid the fml field id to setup (as defined in the FML tables)
       */
       explicit Tfield ( FLDID32 fid ) : len (0), value (NULL) {

        set_id ( fid );

        // check taht we have a string declaration in the FML table
        if ( type () != 6 ) {
          throw atmi_exception ( "Tfield %s's value is of type carray and the FML table decalares a type %s.", name (), tname());
        }
      }

      /** Constructs a Tfield for the passed name
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param n the fml field name to setup (as defined in the FML tables)
       */
      explicit Tfield ( const char *n ) : len (0), value (NULL) {

        setup ( (FLDID32) Fldid32 ( const_cast<char *>(n) ) );

        // check type matching
        if ( type() != 6 ) {
          throw atmi_exception ( "Tfield value is of type carray and the FML table decalares a type %s for %s.", tname(), name());
        }
      }

      virtual ~Tfield() {

        if ( value != NULL) {
          delete[] value;
        }
      };

      /** @return returns the size of the carray
       */
      virtual FLDLEN32 length () {

        return len;
      };

      /** Assigns a value to the field
       *
       * @param c character buffer
       * @param s character buffer size
       */
      void set_char_array ( const char * c, long s ) {

        if ( value != NULL ) {
          delete value;
        }

        len = s;
        value = new char [len];
        memcpy ( value, c, len );
      };

      /** copy (memcpy) the field's value into carray.
       *
       * if the given size is smaller than the actual field size, then size characters are copied into your buffer. If size is bigger than
       * the actual field size then field length characters are copied.
       *
       * @param carray pointer to a previously allocted character buffer
       * @param size   character buffer size
       */
      virtual void get_char_array ( char *carray, long size ) {

        memcpy ( carray, value, ( size < length () ? size : length() ) );
      };

      /** copy the field's value
       */
      Tfield<char *> &operator= ( Tfield<char*> &carray ){

        set_char_array ( carray.value, carray.len );

        return *this;
      };

    protected:

      virtual int set ( buffer &b) {

        int rc = -1;

        rc = Fchg32 ( b.get_buffer(), id(), occurence(), value, length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FCHG32 Tfield::set failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
        }

        return rc;
      };

      virtual int add ( buffer &b) {

        int rc = -1;
        if ( length () > 0 ) {
          try {
            if ( (needed () + b.used()) >= b.size()) {
              b.extend ();
            }
          } catch ( buffer_exception &buffErr ) {
            throw  atmi_exception ( "Add failed to estimate needed memory extension. Original message was : %s", buffErr.what() );
          };

          rc  = Fadd32 ( b.get_buffer(), id(), value, length() );
          if ( rc < 0 ) {
            throw buffer_exception ( Ferror32, "FADD32 Tfield::add<char *> failed for field %s (id: %d, occ: %d)", name(), id(), occurence() );
          } else {
            set_field_occurence ( (b.occurences ( *this ) == 0 ? 0 : b.occurences ( *this )-1));
          }

        } else {
          throw atmi_exception ( "field %s's value is empty !!?? Cannot add an empty field value.", name () );
        }

        return rc;
      };

      virtual int get ( buffer &b ){

        return get ( b, occurence() );
      };

      virtual int get ( buffer &b, FLDOCC32 occ ){

        int rc = -1;
        set_field_occurence(occ);

        if ( value != NULL ) {
          delete[] value;
        }

        value = CFgetalloc32 ( b.get_buffer(), id(), occurence(), FLD_CARRAY, &len );
        if ( value != NULL ) {

          printf("DEBUG len is %d\n", len );

          rc = 0;

        } else {
          throw buffer_exception (Ferror32, "FGETALLOC32 Tfield::get<char *> failed to get field %s (id: %d, occ: %d).", name(), id(), occurence() );
        }

        return rc;
      };

      FLDLEN32 len;   //!< field length (bytes)
      char    *value; //!< chacater buffer
  };

  /** Helper that handles the operator << between output streams and field value */
  ostream &operator<< ( ostream &o, Tfield<string> &f );

/** @} */
} // namespace atmi
#endif

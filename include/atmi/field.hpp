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
#include <atmi/definitions.hpp>

#ifndef CPP_ATMI_FIELDS_HPP
#define CPP_ATMI_FIELDS_HPP

#include <atmi/exceptions.hpp>
#include <atmi/buffer.hpp> // this MUST be here to avoid recursive include

// using namespace std;

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

      /** Default destructor
       */
      virtual ~field () {
        // Intentionally unimplemented...
      };

      /** Tuxedo field type.
       *
       *  Possible values are:
       *  - FLD_SHORT       0       short in
       *  - FLD_LONG        1       long in
       *  - FLD_CHAR        2       character
       *  - FLD_FLOAT       3       single-precision floa
       *  - FLD_DOUBLE      4       double-precision floa
       *  - FLD_STRING      5       std::string - null terminated
       *  - FLD_CARRAY      6       character array
       *  - FLD_PTR         9       pointer to a buffer
       *  - FLD_FML32      10       embedded FML32 buffer
       *  - FLD_VIEW32     11       embedded VIEW32 buffer
       *  - FLD_MBSTRING   12       multibyte character array
       *
       * @return the field's type
       */
      field_type type ();

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
      // TODO remove this
      //virtual void set_id(FLDID32 id);

      /** Extracts the field number from the field identifier.
       *
       * @return field number.
       */
      int number ();

      /** @return the name of the field */
      const std::string name () const ;

      /** @return the fields occurence (as last found in a buffer)*/
      FLDOCC32 occurence ();

      /** @return the last Ferror32 value that was returned by the FML library */
      int error ();

      /** @return the length of the field's value
       */
      virtual FLDLEN32  length () = 0;

      /** @return number bytes needed to store the fields value */
      virtual long needed ();

      /** @return a std::string describing the field
       */
      const char *what ();

      /** @param value value to assign to the field.
       */
      virtual field &operator= ( const std::string &value ) {
        return *this;
      };

      virtual const std::string to_string() const = 0;

    protected:

      /** default constructor
       *
       * default occurence value is 0.
       */
      field();

      /** new field instance
       *
       * @param id field ID
       */
      explicit field( FLDID32 id);

      /** new field instance.
       *
       * the name field must exists in known FML table (see FIELDTBLS32 and FLDTBLDIR32).
       *
       * @param name field name 
       */
      explicit field( const std::string &name);

      /* Field manipulation error handler.
       *
       * @param ferror Ferror32 value
       * @param format error message formatting std::string.
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
       * When successfull the value of occurence is se
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

      FLDID32     _field_id;
      FLDOCC32    _field_occurence;
      std::string _field_name;
      int         _ferror;
      std::string _what; // used to return a desciption
  };

  /** Template that handles short, long, char, double data types.
   *
   * \copydoc atmi::field
   *
   * @tparam T field's backend data type.
   * @tparam fid field ID (defaults to BADFLDID )
   *
   */
  template <class T, FLDID32 fid = BADFLDID>
  class Tfield : public field {
    public:

      /** default constructor.
       *
       * The instance cannot be used until set_field_id is successfully called.
       *
       * @since v4.2.0
       */
      Tfield(): _value(0), field(fid){
      };

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param field_id the fml field id to setup (as defined in the FML tables)
       */
      explicit Tfield ( FLDID32 field_id ): _value(0), field(field_id){
        // check data type is compatible
        check_type_match();
      }

      /** Constructs a Tfield for the passed name
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param name the fml field name to setup (as defined in the FML tables)
       */
      explicit Tfield ( const std::string &name ): _value(0), field (name) {

        // check data type is compatible
        check_type_match();

      }

      /** dispose of resources
       */
      virtual ~Tfield() {
        // Intentional...
      };

      /** @return de length (or size) of the field's data
       */
      virtual FLDLEN32 length () {

        return sizeof ( _value );
      };

      /** Assigns a value to the field
       * ...
       * Tfield<int> f (fid);
       * f = 156 ; // Set value to 156
       * ...
       */
      T operator= ( T value) {

        _value = value;

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

        return _value;
      };

    protected:

      virtual int set ( buffer &b) {

        int rc = -1;

        rc = Fchg32 ( b.get_buffer(), id(), occurence(), (char *) &_value, length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FCHG32 Tfield::set failed for field %s (id: %d, occ: %d)",
              name().c_str(),
              id(),
              occurence() );
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
        }

        rc = Fadd32 ( b.get_buffer(), id(), (char *) &_value, length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FADD32 Tfield::add failed for field %s (id: %d, occ: %d)",
              name().c_str(),
              id(),
              occurence() );
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

        rc = Fget32 ( b.get_buffer(), id(), occurence(), (char *) &_value, &l );
        if ( rc == -1 ) {
          throw buffer_exception (Ferror32, "FGET32 failed to get field %s (id: %d, occ: %d).",
              name().c_str(),
              id(),
              occurence() );
        }

        return rc;
      };

      /** check if backend type is compatible with FML type
       */
      void check_type_match(){
        // check type matching
        if (strcmp (typeid(_value).name(), tname ()) != 0 ) {
          throw atmi_exception ( "Tfield [%s] (id: %d) backend value is of type %s and the FML table decalares a type %s.",
              name().c_str(),
              id(),
              typeid(_value).name(),
              tname());
        }
      }

      T _value; //!< field's current value

  };

  /** Helper that handles the operator << between output streams and field value */
  std::ostream &operator<< ( std::ostream &o, field &f );

/** @} */
} // namespace atmi
#endif

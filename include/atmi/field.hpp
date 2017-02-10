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

#ifndef CPP_ATMI_FIELD_HPP
#define CPP_ATMI_FIELD_HPP

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
       *  FLD_SHORT       0       short in
       *  FLD_LONG        1       long in
       *  FLD_CHAR        2       character
       *  FLD_FLOAT       3       single-precision floa
       *  FLD_DOUBLE      4       double-precision floa
       *  FLD_STRING      5       std::string - null terminated
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

      /** Extracts the field number from the field identifier.
       *
       * @return field number.
       */
      int number ();

      /** @return the name of the field */
      const std::string name () const;

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

      FLDID32      _field_id;
      FLDOCC32     _field_occurence;
      std::string _field_name;
      int         _ferror;
      std::string _what; // used to return a desciption
  };

/** @} */
} // namespace atmi
#endif

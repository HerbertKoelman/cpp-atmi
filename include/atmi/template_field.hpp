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
#include <atmi/field.hpp>
#include <atmi/definitions.hpp>

#ifndef CPP_ATMI_FIELDS_HPP
#define CPP_ATMI_FIELDS_HPP

namespace atmi {

/** \addtogroup fml
 *
 * @{
 */

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

      const std::string to_string() const {
        return __FUNCTION__;
      }

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

/** @} */
} // namespace atmi
#endif

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
#include <atmi/template_field.hpp>

#ifndef CPP_ATMI_STRING_FIELD_HPP
#define CPP_ATMI_STRING_FIELD_HPP

// using namespace std;

namespace atmi {

/** \addtogroup fml
 *
 * @{
 */

  /** Specialization of template Tfield which handles std::string typed fields.
   *
   * This class is using a std::string object to hold and handle std::string data
   * \copydoc atmi::field
   */
  template <> class Tfield<std::string>: public field {
    public:

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param fid the fml field id to setup (as defined in the FML tables)
       */
      explicit Tfield ( FLDID32 fid ): _value(""), field(fid) {

        check_type_match();
      }

      /** Constructs a Tfield for the passed name
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param name the fml field name to setup (as defined in the FML tables)
       */
      explicit Tfield ( const char *name ): _value(""), field(name) {

        check_type_match();
      }

      virtual ~Tfield() {
        // Intentionally unimplemented...
      };

      /** @return the std::string's length
       */
      virtual FLDLEN32 length () {

        return (FLDLEN32)_value.length();
      };

      /** @return the std::string's size
       */
      virtual FLDLEN32 size () {

        return (FLDLEN32)_value.size();
      };

      /** @return a C std::string (with \0) of current std::string's content.
       */
      const char* c_str ( ) const {
        return _value.c_str();
      }

      /** Appends a copy of the argument to the std::string.
       *
       * The new std::string content is the content existing in the std::string object before the call followed by the content of
       * the argument.
       *
       * The append member function provides a similar functionality with additional options.
       *
       * @param str a copy of the content of this object is appended to the object's content.
       * @return *this
       */
      std::string& operator+= ( const std::string& str ){

        _value += str;
        return _value;
      }

      /** Appends a copy of the argument to the std::string.
       *
       * The new std::string content is the content existing in the std::string object before the call followed by the content of
       * the argument.
       *
       * The append member function provides a similar functionality with additional options.
       *
       * @param  s  a pointer to an array containing a null-terminated character sequence (C std::string), which is appended to the object's content.
       * @return *this
       */
      std::string& operator+= ( const char* s ){

        _value += s;
        return _value;
      }

      /** Appends character to the std::string field.
       *
       * @param c   character. This single character is appended to the std::string object's content.
       * @return *this
       */
      std::string& operator+= ( char c ){
        _value += c;
        return _value;
      }

      /**
       * Returns a reference the character at position pos in the std::string.
       *
       * The function actually returns data()[ pos ].
       *
       * The at member function has the same behavior as this operator function, except that at also performs a range
       * check.
       *
       * @param pos position within the std::string of the character to be retrieved. Notice that the first character in the std::string has a position of 0, not 1. size_t is an unsigned integral type.
       * @return The character at the specified position in the std::string.
       */
      const char& operator[] ( size_t pos ) const {

        return _value[pos];
      }

      /**
       * Returns a reference the character at position pos in the std::string.
       *
       * The function actually returns data()[ pos ].
       *
       * The at member function has the same behavior as this operator function, except that at also performs a range
       * check.
       *
       * @param pos position within the std::string of the character to be retrieved. Notice that the first character in the std::string has a position of 0, not 1. size_t is an unsigned integral type.
       * @return The character at the specified position in the std::string.
       */
      char& operator[] ( size_t pos ){

        return _value[pos];
      }

      /**
       * assigns values to the std::string field.
       *
       * @param str   null terminated character array.
       */
      virtual Tfield<std::string> &operator= ( const char* str ) {

        _value = str;
        return *this;
      };

      /**
       * assigns values to the std::string field.
       *
       * @param str std::string to assign
       */
      virtual Tfield<std::string> &operator= ( const std::string &str ) {

        _value = str;

        return *this;
      };

      /**
       * assigns values to the std::string field.
       *
       * @param  c  the content is set to a single character.
       */
      virtual Tfield<std::string> &operator= ( char &c ) {

        _value = c;

        return *this;
      };

      /** casts the field value
       */
      operator std::string(){

        return _value;
      };

    protected:

      virtual int set ( buffer &b) {

        int rc = -1;

        rc = Fchg32 ( b.get_buffer(), id(), occurence(), (char *) _value.c_str(), length() );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FCHG32 Tfield::set failed for field %s (id: %d, occ: %d)", name().c_str(), id(), occurence() );
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
          }

          rc = Fadd32 ( b.get_buffer(), id(), (char *) _value.c_str(), length() );
          if ( rc < 0 ) {
            throw buffer_exception ( Ferror32, "FADD32 Tfield::add failed for field %s (id: %d, occ: %d)", name().c_str(), id(), occurence() );
          } else {
            set_field_occurence ( (b.occurences ( *this ) == 0 ? 0 : b.occurences ( *this )-1));
          }
        } else {
          throw atmi_exception ("field %s's _value is empty !!?? Cannot add an empty field value.", name ().c_str());
        }

        return rc;
      };

      virtual int get ( buffer &b ){

        return get ( b, occurence() );
      };

      virtual int get ( buffer &b, FLDOCC32 occ ){

        int rc = -1;
        FLDLEN32 l = 0;
        char *value = NULL;
        set_field_occurence(occ);

        value = Fgetsa32 ( b.get_buffer(), id(), occurence(), &l );
        if ( value != NULL ) {

          rc=0;
          _value = value;                    // copy v into std::string value
          delete value;

        } else {
          throw buffer_exception (Ferror32, "FGETSA32 Tfield::get failed to get field %s (id: %d, occ: %d).", name().c_str(), id(), occurence() );
        }

        return rc;
      };

    private:

      void check_type_match(){
        // check type matching
        if ( type() !=5 ) {
          throw atmi_exception ( "Tfield [%s] (id: %d) value is of type %s and the FML table decalares a type %s.",
              name().c_str(),
              id(),
              typeid(_value).name(),
              tname());
        }
      }

      std::string _value; //!< std::string field's value
  };

  /** Helper that handles the operator << between output streams and field value */
  std::ostream &operator<< ( std::ostream &o, Tfield<std::string> &f );

/** @} */
} // namespace atmi
#endif

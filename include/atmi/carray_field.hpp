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
#include <stdexcept>
#include <atmi/definitions.hpp>
#include <atmi/template_field.hpp>

#ifndef CPP_ATMI_CARRAY_FIELD_HPP
#define CPP_ATMI_CARRAY_FIELD_HPP

namespace atmi {

/** \addtogroup fml
 *
 * @{
 */

  /** Specialization of template Tfield which handles CARRAY typed fields.
   *
   * This class is using a std::string object to hold and handle char * data
   * \copydoc atmi::field
   */
  template <> class Tfield<char *>: public field {
    public:

      /** Constructs a Tfield for the passed field id
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param fid the fml field id to setup (as defined in the FML tables)
       */
      explicit Tfield ( FLDID32 fid ) : _length (0),_buffer_size(0), _value (NULL), field(fid) {

      }

      /** Constructs a Tfield for the passed name
       *
       * The search is done in the tables identified by FLDTBLDIR32  and FIELDTBLS32
       *
       * @param name the fml field name to setup (as defined in the FML tables)
       */
      explicit Tfield ( const char *field_name ) : _length (0),_buffer_size(0), _value (NULL), field(field_name) {

        // check type matching
        if ( type() != 6 ) {
          throw atmi_exception ( "Tfield value is of type carray and the FML table decalares a type %s for %s.",
              tname(),
              name().c_str());
        }
      }

      virtual ~Tfield() {

        free_ressources();
      };

      /** @return backend allocation
       */
      FLDLEN32 buffer_size() const{
        return _buffer_size;
      }

      /** @return returns the size of the carray
       */
      virtual FLDLEN32 length () {

        return _length;
      };

      /** Assigns a value to the field
       *
       * @param carray character buffer
       * @param size character buffer size
       */
      void set_char_array ( const char * carray, FLDLEN32 size ) {

        if ( size > _buffer_size ){
          // there is not enough space in backend buffer, re-allocate a bigger one.

          free_ressources(); // free previous backend

          _buffer_size = size ;
          _value = new char [_buffer_size]; // allocate new backend
        }

        // data length is equal to size
        _length = size ;

        // allocated and util length are equal
        memcpy ( _value, carray, size );
      };

      /** copy (memcpy) the field's value into carray.
       *
       * if the given size is smaller than the actual field size, then size characters are copied into your buffer. If size is bigger than
       * the actual field size, then field length characters are copied meaning your data will be truncated.
       *
       * @param carray pointer to a previously allocted character buffer
       * @param size   character buffer size
       */
      virtual void get_char_array ( char *carray, FLDLEN32 size ) {

        _length = ( size < _buffer_size ) ? size : _buffer_size ;
        memcpy ( carray, _value, _length );
      };

      /** copy the field's value
       */
      Tfield<char *> &operator= ( Tfield<char*> &carray ){

        set_char_array ( carray._value, carray._length );

        return *this;
      };

      /** de-allocate internal ressources (mainly the caray).
       */
      void free_ressources(){
        if ( _value != NULL ){ // make sure there were allocated ressources
          _buffer_size = 0;
          _length      = 0;

          delete[] _value;
          _value = NULL ;
        }
      }

      /** gives direct access to characters in the carray.
       *
       * this avoids the need to make a copy prior the utilisation of carray's content (shoudl be faster).
       *
       * @param index character in the range of 0 - length
       * @return a reference to the character at position index.
       */
      char &operator[](int index){
        if ((index < 0) || (index >= _length )){
          std::out_of_range("carray index is out of bound.");
        }
        return _value[index];
      }

      /* if you need to change the field's value, get a copy by calling get_char_array.
       *
       * @return address of first character in carray (is NULL if no carray data is available)
       */
//      operator const char * () const {
//        return _value ;
//      }


    protected:

      /** implement a Fchg32 call for carrays
       *
       * @param b buffer to apply the change in
       */
      virtual int set ( buffer &b) {

        int rc = -1;

        rc = Fchg32 ( b.get_buffer(), id(), occurence(), _value, _length );
        if ( rc < 0 ) {
          throw buffer_exception ( Ferror32, "FCHG32 Tfield::set<char *> failed for field %s (id: %d, occ: %d)",
              name().c_str(),
              id(),
              occurence() );
        }

        return rc;
      };

      /** implement a Fadd32 call for carrays
       *
       * @param b buffer to apply the change in
       */
      virtual int add ( buffer &b) {

        int rc = -1;
        if ( _length > 0 ) {
          try {
            if ( (needed () + b.used()) >= b.size()) {
              b.extend ();
            }
          } catch ( buffer_exception &err ) {
            throw  atmi_exception ( "Add failed to estimate needed memory extension. %s", err.what() );
          }

          rc  = Fadd32 ( b.get_buffer(), id(), _value, _length );
          if ( rc < 0 ) {
            throw buffer_exception ( Ferror32, "FADD32 Tfield::add<char *> failed for field %s (id: %d, occ: %d)",
                name().c_str(),
                id(),
                occurence() );
          } else {
            set_field_occurence ( (b.occurences ( *this ) == 0 ? 0 : b.occurences ( *this )-1));
          }

        } else {
          throw atmi_exception ( "field %s's value is empty !!?? Cannot add an empty field _value.", name () );
        }

        return rc;
      };

      /** retreive the current carray content.
       *
       * the content is copied into character array (backend). if the backend is too small a bigger one
       * is allocated and used.
       *
       * @param b buffer to apply the change in
       */
      virtual int get ( buffer &b ){

        return get ( b, occurence() );
      };

      /** \copydoc virtual int get ( buffer &b )
       *
       * @param b   fielded buffer
       * @param occ field occurence to search for
       */
      virtual int get ( buffer &b, FLDOCC32 occ ){

        int rc = -1;
        set_field_occurence(occ);


        // check if be got a carray backend
        if ( _buffer_size == 0 ){
          // we need to allocate a new carray buffer anyway
          _value = Fgetalloc32 ( b.get_buffer(), id(), occurence(), &_buffer_size );
          if ( _value != NULL ) {

            _length = _buffer_size;

            rc = 0;

          } else {
            throw buffer_exception (Ferror32, "FGETALLOC32 Tfield::get<char *> failed to get field %s (id: %d, occ: %d).",
                name().c_str(),
                id(),
                occurence() );
          }
        } else {

          // we already got a carray backend, we try to fill it with the field's content.
          rc = Fget32(b.get_buffer(), id(), occ, _value, &_length);

          // carray could be truncated, if so the returned size is greater then _buffer_size
          if ((rc != -1) && (_length > _buffer_size)){

            // there was not enough memory to store carray's content, re-allocate

            free_ressources();

            _value = Fgetalloc32 ( b.get_buffer(), id(), occurence(), &_buffer_size );
            if ( _value != NULL ) {

              _length = _buffer_size;
              rc = 0;

            } else {
              throw buffer_exception (Ferror32, "FGETALLOC32 Tfield::get<char *> failed to get field %s (id: %d, occ: %d).",
                  name().c_str(),
                  id(),
                  occurence() );
            }

          } else if (( rc == -1) && ( Ferror32 == FNOSPACE )){ // buffer is too small re-allocate a new one

            free_ressources();

            // allocate a new buffer and move the fields value into i
            _value = Fgetalloc32 ( b.get_buffer(), id(), occurence(), &_buffer_size );
            if ( _value != NULL ) {

              _length = _buffer_size;
              rc = 0;

            } else {
              throw buffer_exception (Ferror32, "FGETALLOC32 Tfield::get<char *> failed to get field %s (id: %d, occ: %d).",
                  name().c_str(),
                  id(),
                  occurence() );
            }
          }
        }

        return rc;
      };

      FLDLEN32 _length;   //!< field length (bytes)
      FLDLEN32 _buffer_size;   //!< back end size(bytes)
      char    *_value; //!< chacater buffer
  };

/** @} */
} // namespace atmi
#endif

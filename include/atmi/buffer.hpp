/*
 * author: herbert koelman(herbert.koelman@me.com)
 * creation date: 1/1/2006
 */

#include <fml.h>
#include <fml32.h>
#include <typeinfo>
#include <string>
#include <cstring>
#include <memory>

#include <atmi/exceptions.hpp>
#include <atmi/fields.hpp>
#include <atmi/tuxedo.hpp>

#ifndef CPP_ATMI_BUFFER_HPP
#define CPP_ATMI_BUFFER_HPP

using namespace std;

namespace atmi {

/** \addtogroup fml Field Manipulation Language
 *
 * A set of C++ classes for defining and manipulating storage structures called fielded buffers, that contain attribute-value
 * pairs called fields. The attribute is the field’s identifier, and the associated value represents the field’s data content.
 *
 * Fielded buffers provide an excellent structure for communicating parameterized data between cooperating processes, by providing
 * named access to a set of related fields. Programs that need to communicate with other processes can use the FML software to
 * provide access to fields without concerning themselves with the structures containing them.
 * @{
 */

//----------------------------------------------------------------------------------------------
  class buffer;
  class field;

  typedef char * carray; //!< character array
#if __cplusplus < 201103L
  typedef auto_ptr<carray> Acarray; //!< auto_ptr to a carray.
  typedef auto_ptr<buffer> Abuffer; //!< auto_ptr to a FML buffer.
#else
  typedef unique_ptr<carray> ACarray; //!< unique_ptr to a carray.
  typedef unique_ptr<buffer> Abuffer; //!< unique_ptr to a FML buffer. 
#endif

  /**
   * FML buffer
   *
   */
  class buffer : public tuxedo {

    public:

      friend class field;

      /** Allocates a buffer of 1024bytes */
      buffer ();

      /** Create a new buffer reference.
       *
       * Memory allocation (alloc/free) are not handled by the instance create by this constructor. We assume that
       * the given buffer was allocated elsewhere. And it will be deallocated later.
       *
       * @param b set FML32 buffer reference
       */
      explicit buffer ( FBFR32 *b );

      /** Allocates a buffer of byte size.
       * @param len bytes space of field value in bytres
       */
      explicit buffer ( FLDLEN32 len );

      /** default destructor
       *
       * If memory was allocated by this instance then it will be freed. Otherwise the buffer is not deallocated.
       */
      ~buffer ();

      /**
       * @return true if it's a FMLTYPE32 buffer type
       * @throw atmi_exception if we failed to get buffer type (tptypes)
       */
      static inline bool is_fml32_buffer( char *buffer );

      /** @return the size of the buffer (in bytes) */
      size_t size () const;

      /** @return number of bytes stored into the buffer */
      size_t used () const;

      /** @return number of bytes not yet used into the buffer */
      size_t unused () const;


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
       * @throw buffer_exception upon failure
       */
      void resize ( size_t extent );

      /** Set/change a field's value into the buffer (Fchg32)
       *
       * @param f a pointer to the field for which to change the value
       * @throw buffer_exception upon failure
       *
       * @see add
       */
      field &set    ( field &f );

      /** Adds a field into the buffer (Fadd32).
       *
       * Adds an occurence of a field to the buffer. The first call becomes occurence 0. When successfully added the 
       * occurence property of the field is set.
       *
       * @param f a pointer to the field to add into the buffer
       * @throw buffer_exception upon failure
       *
       * @see set to update the value of a field in a buffer
       */
      field &add    ( field &f );         // FLDID fieldid, char *value, FLDLEN len );

      /** Appends a field to the buffer
       *
       * @param f a pointer to the field to add into the buffer
       * @deprecated not implemented.
       */
      field &append ( field &f );         // FLDID fieldid, char *value, FLDLEN len );

      /** Removes the field from the buffer (Fdel32).
       *
       * @param f a pointer to the field to remove
       * @throw buffer_exception upon failure
       */
      void remove ( field &f );         // FLDID fieldid );

      /** Get the field value set in the buffer (Fget32).
       *
       * The field's occurence property is used to find the fields value.
       *
       * @param f the field to set
       * @return the field when set
       * @throw buffer_exception upon failure
       */
      field &get ( field &f );

      /** Get the field value set in the buffer (Fget32).
       *
       * @param f the field to set
       * @param occ the field occurence to set
       * @return the field when set
       * @throw buffer_exception upon failure
       */
      field &get ( field &f, FLDOCC32 occ);

      /** @return the current check sum of the buffer
       *
       * @throw buffer_exception when calcutaion fails.
       */
      long chksum();

      /** @return the number of occurences of the field into the buffer (Foccur32)*/
      FLDOCC32 occurences ( const field &f );

      /** @return the number of fields into the buffer */
      FLDOCC32 field_count () const;

      /** call this to get an idea of what char * buffer you should allocate to store the
       * result of Fsprint32
       *
       * @return an estimatation of what a buffer size should to call Fsprint32
       */
      size_t print_buffer_size()const ;

      /** fill the given buffer with the a string representation of the buffer's content.
       *
       * @param buffer char buffer to fill
       * @see print_buffer_size()
       */
      void print(char *buffer) const ;

      /** print on stdout the content of the buffer. Mainly used for debugging
       */
      void print () const;

      /** @return return FML32 buffer reference
       */
      FBFR32 * get_buffer ();

      /** Set a new buffer reference. Previous buffer is deallocated before assign the passed value.
       * @param b set FML32 buffer reference
       */
      void set_buffer ( FBFR32 *b );

      /**
       * @return true if this instance willfree allocated memory buffer.
       */
      bool is_handling_memory();

      /** @param b If true then destructor will free referenced buffer.
       */
      void set_handling_memory ( bool b);

      // operators -------------------------------------------------------------------------------

      /** Checks equality of two buffers (based upon chksum)
       * @return true if both checksums are equal.
       */
      bool operator== ( buffer &);

      /** Copies the content of a fielded buffer into another
       */
      buffer &operator= (buffer &);

    private:
      
      FBFR32 *_buffer;     //!< FML buffer reference 
      bool    _allocated;  //!< if true then buffer was allocated by the buffer instance (thus it can be freed when needed).
      long    _extent;     //!< value by which the buffer size will be extended 
  };

  /** @} */
}
#endif


/*
 * tuxedo
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * tuxedo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Logging tuxedo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tuxedo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef CPP_ATMI_TUXEDO_HPP
#define CPP_ATMI_TUXEDO_HPP

#include <typeinfo>
#include <atmi.h>
#include <string>
#include <cstdio>
#include <memory>
#include <nl_types.h>

#include <atmi/config.h>
#include <atmi/exceptions.hpp>

// using namespace std;

namespace atmi {

/** \addtogroup atmi Application-to-Transaction Monitor Interface
 *
 * The main API for the Tuxedo system. It includes transaction management functions (routines, verbs); request/response,
 * conversational, queuing, and publish-and-subscribe message-handling functions; service interface functions; and buffer
 * management functions for distributed application communication.
 * @{
 */

  /**
   * All common used ATMI method are group in this class.
   *
   * @author Herbert Koelman
   */
  class tuxedo {
    public:
      tuxedo ();
      virtual ~tuxedo();

      /**
       * Allocate a new buffer.
       *
       * buffer types provided by tuxedo
       * CARRAY	   Character array (possibly containing NULL characters) that is neither encoded nor decoded during transmission
       * STRING	   NULL-terminated character array
       * FML		   FML fielded buffer
       * VIEW		   C structure or FML view
       * X_OCTET   Equivalent to CARRAY; provided for XATMI compatibility
       * X_C_TYPE  Equivalent to VIEW; provided for XATMI compatibility
       * X_ COMMON Equivalent to VIEW; provided for XATMI compatibility
       * FML32     FML32 fielded buffer, using 32-bit identifiers and offsets
       * VIEW32    C structure or FML32 view, using 32-bit identifiers, counter variables, and size variables
       * XML       buffer for XML documents
       * MBSTRING  Character array for multibyte characters
       *
       * Note that only the first eight bytes of type and the first 16 bytes
       * of subtype are significant.
       *
       * @param type buffer type to allocate (STRING, VIEW, FML, ...)
       * @param subtype buffer subtype. NULL means none
       * @param size memory size to allocate.
       *
       * @return allocated buffer or NULL if failed
       *
       * @throw tuxedo_exception if something goes wrong.
       */
      char *allocate ( const char *type, const char *subtype, long size );

      /**
       * resize buffer.
       *
       * @param buffer buffer to resize (the return value maybe different buffer)
       * @param extent number of bytes to add
       * @return reference to resized buffer
       */
      char *extend ( char *buffer, long extent );

      /**
       * Free a previously allocated tuxedo buffer.
       */
      void free ( char *buffer ); //NOSONAR named intentionally...

      /**
       * Starts XA transaction.
       *
       * @param timeout max duration in seconds for a transaction to complete.
       *
       * @throw transaction_exception is raised upon failure.
       */
      int begin ( int timeout = 0 );

      /**
       * Commit XA transaction.
       *
       * @throw transaction_exception on failure.
       */
      int commit ();

      /**
       * Abort XA transaction.
       *
       * @throw transaction_exception if abort failed.
       */
      int abort (); //NOSONAR named intentionally...

      /**
       * @return the current errno value
       */
      inline long  error () const {
        return _errorno;
      };

      /**
       * @return an error description string of the last errno
       */
      inline const char *error_message () const {
        return tpstrerror(_errorno);
      };

      /**
       * @return the current error detail (if one exist).
       */
      inline int errnodetail() const {
        return _errornodetail;
      };
      /**
       * @return an error detail description string (if one exists).
       */
      inline const char *error_detail () const {
        return tpstrerrordetail ( _errornodetail, 0 );
      };

      /** set the tuxedo context to use when calling tuxedo functions(tpsetctxt()).
       *
       * When set, ATMI++ switches to this context before executing a tuxedo call.
       *
       * @param context context to be used by this instance
       */
      inline void set_context ( TPCONTEXT_T context) {
        _context = context;
        // printf("DEBUG set_context [%d]:\n", _context);
      };

      /** @return the current contex
       */
      inline TPCONTEXT_T context () const {

        // printf("DEBUG context [%d]:\n", _context);

        return _context;
      };

      /**
       * @return tuxedo flags used by this instance.
       */
      inline long flags() const {
        return _flags;
      };

      /**
       * Set tuxedo flags
       */
      inline void set_flags ( long flags ) {
        tuxedo::set (_flags, flags);
      };

      /**
       * Reset flag value to TPNOFLAGS
       */
      inline void reset_flags ( long flags ) {
        _flags = TPNOFLAGS;
        tuxedo::set (_flags, flags);
      };

      /** unset flags
       * @param flags flags to unse
       */
      inline void unset_flags ( long flags ) {
        tuxedo::unset (_flags, flags);
      };

      static const long FAILED = -1; //!< Tuxedo error value

    protected:

      /** If context is set (context != 0 ), then tpsetctxt is called. This method does nothing otherwise.
       */
      void switch_context ();

      /** Unset bits in second argument from first argumen
       *
       * @param bits bits that should be unse
       * @param flags flags to unse
       */
      static long unset ( long bits, long flags);

      /** set bits in second argument in first argumen
       *
       * @param bits bits that should be unse
       * @param flags flags to unse
       */
      static long set ( long bits, long flags);

      /** update current errno and sets what must be set.
       *
       * @deprecated we use exception instead
       */
      void updateErrno ();

      /**
       * Triggers exceptions according to the given tperrno passed. The exception will be initialized with msg.
       *
       * @param _tperrno tperrno to handle
       * @param msg message to setup in thrown exception.
       * @param args message argument (see printf)
       * @return legacy will be removed when prototype will be changed to void handle_transaction_errno()
       */
        template<typename... Args>  int handle_tperrno ( int _tperrno, const char *msg, const Args&... args) {

          switch ( _tperrno ) {
            case TPEINVAL:
            case TPEPERM:
            case TPENOENT:
            case TPEITYPE:
            case TPEOTYPE:
            case TPETRAN:
            case TPEPROTO:
            case TPESYSTEM:
            case TPEOS:
            case TPELIMIT:
            {
              throw tuxedo_exception ( _tperrno, msg, args... );
            }
            break;
            case TPEBLOCK:
            {
              throw blocking_exception ( msg, args... );
            }
            break;
            case TPGOTSIG:
            {
              throw interrupt_exception ( msg, args... );
            }
            break;
            case TPESVCERR:
            {
              throw service_exception ( msg, args... );
            }
            break;
            case TPETIME:
            {
              throw timeout_exception ( msg, args... );
            }
            break;
            case TPESVCFAIL:
              // return application specific error number instead
              // as the application will probably know what to do
              _tperrno = ((tpurcode > 0)? tpurcode : -1 );
              break;

            default:
              throw tuxedo_exception (_tperrno, catgets ( _catd, CATD_ATMI_SET, 33,"Never heard about this tperrno (%d)."), _tperrno );
          }

          return _tperrno;
        };

      long    _flags; //!< Tuxedo flags
      nl_catd _catd;  //!< message catalog refenence

    private:
      long        _errorno;
      long        _errornodetail;
      TPCONTEXT_T _context;

  }; // class tuxedo

  /** @} */
} // namespace atmi

#endif

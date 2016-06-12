/*
 * atmi_exceptions
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * atmi_exceptions is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * atmi_exceptions is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with atmi_exceptions; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef __TUXEDO_EXCEPTION__
#define __TUXEDO_EXCEPTION__

#include <stdarg.h>
#include <exception>
#include <string>

using namespace std;

namespace atmi {

/****************************************************************************
*
* base class
*
****************************************************************************/

/**
 * Base class of ATMI++ exception
 *
 * Can be used to throw any kind of error message in a consitent way.
 */
  class atmi_exception : public std::exception {
    public:
      /** create a new instance
       *
       * @param msg error message
       * @todo use variadic template instead.
       */
      atmi_exception ( const char *msg = NULL, ... ) throw ();
      virtual ~atmi_exception () throw () { };

      /**
       * @return user friendly text message
       */
#if __cplusplus < 201103L
      virtual const char *what() throw () ;
#else
      virtual const char *what() const noexcept override;
#endif

      /** @return explanatory error message. */
#if __cplusplus < 201103L
      virtual const char *message() throw() ;
#else
      virtual const char *message() const noexcept ;
#endif

      /**
       * helper to build string message using va_list.
       *
       * Message length is limited to 1024 characters long.
       *
       * this method is public because it is used by error handlers (queue, transaction). This can surely be re-written
       * using variadic templates, making it possible to pass ... from one constructor to it's base one.
       */
      void setup_message ( const char *, va_list );

    protected:
      std::string _message; //!< error message
      std::string _what;    //!< what message (often the concatenation of message + other infos)
  };

  /** Unix related exceptions.
   *
   * This exception can be used to return a system error.
   */
  class unix_exception : public atmi_exception {
    public:
      /** new unix exception.
       *
       * @param err errno value
       * @param msg error message
       * @param ... error message parameters (variadic).
       */
      unix_exception ( int err, const char *msg = NULL, ... ) throw ();

      /** new unix exception.
       *
       * @param msg error message
       * @param ... error message parameters (variadic).
       */
      unix_exception ( const char *msg, ... ) throw () ;

      virtual ~unix_exception () throw() {};

      /** @return unix errno
       */
      int error () const;
    protected:
      int _error; //!< unix errno
  };

  /** FML buffer related exceptions.
   *
   */
  class buffer_exception : public atmi_exception {
    public:
      /** new buffer error instance.
       *
       * @param err Ferror32 value
       * @param msg explanation message
       * @param ... message parameters
       */
      buffer_exception ( int err, const char *msg = NULL, ... ) throw ();

      /**
       * @return tuxedo FML error number
       */
      int error () const;

      /**
       * @return tuxedo FML error message string
       */
       const char *error_message () const;

      /**
       * @return user friendly text message
       */
#if __cplusplus < 201103L
      virtual const char *what() throw () ;
#else
      virtual const char *what() const noexcept override;
#endif

    private:
      int _error; //!< related Ferror32 error number.
  };

  /**
   *
   * Tuxedo TP related exceptions
   *
   */
  class tuxedo_exception : public atmi_exception {
    public:
      /**
       * Tuxedo exceptions.
       *
       * @param err value of tperrno
       * @param msg error message.
       */
      tuxedo_exception ( int err = 0, const char *msg = NULL, ... ) throw ();

//      /** tuxedo exception
//       *
//       * @param err value of tperrno
//       */
//      tuxedo_exception ( int err ) throw ();

      virtual ~tuxedo_exception () throw () {
      };

      /**
       * @return user friendly text message
       */
#if __cplusplus < 201103L
      virtual const char *what() throw () ;
#else
      virtual const char *what() const noexcept override;
#endif

      /**
       * @return tperr that raise the exception
       */
      virtual inline int error () const {
        return _error;
      };

      /**
       * @return detail error number
       */
      virtual inline int detail () const {
        return _detail;
      };

      /**
       * @return tuxedo error message string
       */
      virtual const char *error_message () const ;

      /**
       * @return tuxedo error detail string
       */
      virtual const char *error_detail () const ;

    private:

      int _error; //!< A Tuxedo error number (tperrno)
      int _detail;//!< Tuxedo detail error number

  };

/**
 * Thrown when a TPESVRERR is returned after a TP call.
 */
  class service_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param err error number
       * @param msg error message format
       * @param ... error message parameters.
       */
      service_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~service_exception () throw () { };
  };

/**
 * Thrown when TPETIME is returned after a TP call.
 */
  class timeout_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param err error number
       * @param msg error message format
       * @param ... error message parameters.
       */
      timeout_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~timeout_exception () throw (){ };
  };

/**
 * Thrown when TPEBLOCK is returned after a TP call and a blocking condition
 * exists.
 */
  class blocking_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param err error number
       * @param msg error message format
       * @param ... error message parameters.
       */
      blocking_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~blocking_exception () throw () {
      };
  };

/**
 * Thrown when TPENOSIG is returned after a signal was received.
 */
  class interrupt_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param err error number
       * @param msg error message format
       * @param ... error message parameters.
       */
      interrupt_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~interrupt_exception () throw () {
      };
  };

  /**
   *
   * /Q related exceptions
   *
   */
  class diagnostic_exception : public tuxedo_exception {
    public:
      /**
       * Constructs a Queue exeption
       *
       * @param err value of tperr
       * @param diagno value of ctl.diagnostic
       * @param msg error message format.
       * @param ... error message parameters
       */
      diagnostic_exception ( int err = 0, long diagno = 0, const char *msg  = NULL, ... ) throw ();
      virtual ~diagnostic_exception ()  throw () {
      };

      /**
       * @return tuxedo diagnostic error number
       */
      inline long diagnostic () const {
        return _diagno;
      };

      /**
       * @return diagnostic error message string
       */
      const char *diagnostic_message () const ;
//
//      /**
//       * @return user friendly text message
//       */
//#if __cplusplus < 201103L
//      virtual const char *what() throw () ;
//#else
//      virtual const char *what() const noexcept override;
//#endif

    private:
      long _diagno;
  };

/**
 * Thrown when QMENOMSG is returned
 */
  class nomsg_exception : public diagnostic_exception {
    public:
      /**
       * Constructs a Queue exeption
       *
       * @param err value of tperr
       * @param diagno value of ctl.diagnostic
       * @param msg error message format.
       * @param ... error message parameters
       */
      nomsg_exception ( int err, int diagno, const char *msg = NULL, ... ) throw ();
      virtual ~nomsg_exception () throw () {
      };
  };

/**
 * Thrown when QMEABORTED is returned
 */
  class aborted_exception : public diagnostic_exception {
    public:

      /**
       * Constructs an aborted  exeption
       *
       * @param err value of tperr
       * @param diagno value of ctl.diagnostic
       * @param msg error message format.
       * @param ... error message parameters
       */
      aborted_exception ( int err, int diagno, const char *msg = NULL, ... ) throw ();
      virtual ~aborted_exception () throw () { };
  };

} // atmi namespace
#endif

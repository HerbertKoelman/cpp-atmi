/*
   $Id: atmi_exceptions.h 79 2007-08-18 17:30:26Z hkoelman $
 */

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
  class atmi_exception : public exception {
    public:
      /** create a new instance
       *
       * @param msg error message
       */
      atmi_exception ( const char *msg = NULL, ... ) throw ();
      virtual ~atmi_exception () throw () { };

      /**
       * @return user friendly text message
       */
      virtual const char *what () throw ();

      /**
       * helper to build string message using va_list.
       *
       * Message length is limited to 1024 characters long.
       */
      void setup_message ( const char *, va_list );

    protected:
      string message;
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
       * @param error message
       * @param ... error message parameters (variadic).
       */
      unix_exception ( int err, const char *msg = NULL, ... ) throw ();

      /** new unix exception.
       *
       * @param error message
       * @param ... error message parameters (variadic).
       */
      unix_exception ( const char *msg, ... ) throw () ;
      virtual ~unix_exception () throw() {};

      /**
       * @return user friendly text message
       */
      virtual const char *what () throw ();

      /** @return unix errno
       */
      int get_errno ();
    protected:
      int error;
  };

  /** FML buffer related exceptions.
   *
   */
  class buffer_exception : public atmi_exception {
    public:
      buffer_exception ( int err, const char *msg = NULL, ... ) throw ();

      /**
       * @return tuxedo FML error number
       */
      int getFerror ();

      /**
       * @return tuxedo FML error message string
       */
      virtual const char *getFmlerrmsg ();

      /**
       * @return error message.
       */
      virtual const char *getMessage();

      /**
       * @return user friendly text message
       */
      virtual const char *what() throw ();

    protected:
      int ferror;
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
       * @param err value of tperr
       * @param msg error message.
       */
      tuxedo_exception ( int err = 0, const char *msg = NULL, ... ) throw ();
      virtual ~tuxedo_exception () throw () {
      };

      /**
       * @return tperr that raise the exception
       */
      virtual inline int getErrno () {
        return tuxerror;
      };

      /**
       * @return detail error number
       */
      virtual inline int getErrdetail () {
        return errdetail;
      };

      /**
       * @return error message.
       */
      virtual const char *getMessage();

      /**
       * @return tuxedo error message string
       */
      virtual const char *getTperrmsg ();

      /**
       * @return tuxedo error detail string
       */
      virtual const char *getTperrdetail ();

      /**
       * @return user friendly text message
       */
      virtual const char *what () throw ();

    protected:

      int tuxerror;
      int errdetail;

  };

/**
 * Thrown when a TPESVRERR is returned after a TP call.
 */
  class service_exception : public tuxedo_exception {
    public:
      service_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~service_exception () throw () {
      };
  };

/**
 * Thrown when TPETIME is returned after a TP call.
 */
  class timeout_exception : public tuxedo_exception {
    public:
      timeout_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~timeout_exception () throw (){
      };
  };

/**
 * Thrown when TPEBLOCK is returned after a TP call and a blocking condition
 * exists.
 */
  class blocking_exception : public tuxedo_exception {
    public:
      blocking_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~blocking_exception () throw () {
      };
  };

/**
 * Thrown when TPEGOSIG is returned after a signal was received.
 */
  class interrupt_exception : public tuxedo_exception {
    public:
      interrupt_exception ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~interrupt_exception () throw () {
      };
  };

/****************************************************************************
 *
 * /Q related exceptions
 *
 ***************************************************************************/

  class diagnostic_exception : public tuxedo_exception {
    public:
      /**
       * Constructs a Queue exeption
       *
       * @param err value of tperr
       * @param diagno value of ctl.diagnostic
       * @param msg error message.
       */
      diagnostic_exception ( int err = 0, long diagno = 0, const char *msg  = NULL, ... ) throw ();
      virtual ~diagnostic_exception ()  throw () {
      };

      /**
       * @return tuxedo diagnostic error number
       */
      inline long getDiagno () {
        return diagno;
      };

      /**
       * @return diagnostic error message string
       */
      const char *getDiagmsg ();

      /**
       * @return user friendly text message
       */
      virtual const char *what () throw ();

    private:
      long diagno;
  };

/**
 * Thrown when QMENOMSG is returned
 */
  class nomsg_exception : public diagnostic_exception {
    public:
      nomsg_exception ( int err, int diagno, const char *msg = NULL, ... ) throw ();
      virtual ~nomsg_exception () throw () {
      };
  };

/**
 * Thrown when QMEABORTED is returned
 */
  class aborted_exception : public diagnostic_exception {
    public:
      aborted_exception ( int err, int diagno, const char *msg = NULL, ... ) throw ();
      virtual ~aborted_exception () throw () {
      };
  };

} // atmi namespace
#endif

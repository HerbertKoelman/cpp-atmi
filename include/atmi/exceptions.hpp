/*
   $Id: Exceptions.h 79 2007-08-18 17:30:26Z hkoelman $
 */

/*
 * Exceptions
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * Exceptions is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Exceptions is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Exceptions; if not, write to the Free Software
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
  class Exception : public exception {
    public:
      /** create a new instance
       *
       * @param msg error message
       */
      Exception ( const char *msg = NULL, ... ) throw ();
      virtual ~Exception () throw () {
      };

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

/****************************************************************************
*
* Unix related exceptions
*
****************************************************************************/
  class UnixException : public Exception {
    public:
      UnixException ( int err, const char *msg = NULL, ... ) throw ();

      /**
       * @return user friendly text message
       */
      virtual const char *what () throw ();

      int get_errno ();
    protected:
      int error;
  };

/****************************************************************************
*
* Tuxedo FML related exceptions
*
****************************************************************************/
  class BufferException : public Exception {
    public:
      BufferException ( int err, const char *msg = NULL, ... ) throw ();

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

/****************************************************************************
*
* Tuxedo TP related exceptions
*
****************************************************************************/

  class TuxedoException : public Exception {
    public:
      /**
       * Base class of Tuxedo exceptions.
       *
       * @param err value of tperr
       * @param msg error message.
       */
      TuxedoException ( int err = 0, const char *msg = NULL, ... ) throw ();
      virtual ~TuxedoException () throw () {
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
  class ServiceException : public TuxedoException {
    public:
      ServiceException ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~ServiceException () throw () {
      };
  };

/**
 * Thrown when TPETIME is returned after a TP call.
 */
  class TimeoutException : public TuxedoException {
    public:
      TimeoutException ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~TimeoutException () throw (){
      };
  };

/**
 * Thrown when TPEBLOCK is returned after a TP call and a blocking condition
 * exists.
 */
  class BlockingException : public TuxedoException {
    public:
      BlockingException ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~BlockingException () throw () {
      };
  };

/**
 * Thrown when TPEGOSIG is returned after a signal was received.
 */
  class InterruptException : public TuxedoException {
    public:
      InterruptException ( int err, const char *msg = NULL, ... ) throw ();
      virtual ~InterruptException () throw () {
      };
  };

/****************************************************************************
 *
 * /Q related exceptions
 *
 ***************************************************************************/

  class DiagnosticException : public TuxedoException {
    public:
      /**
       * Constructs a Queue exeption
       *
       * @param err value of tperr
       * @param diagno value of ctl.diagnostic
       * @param msg error message.
       */
      DiagnosticException ( int err = 0, long diagno = 0, const char *msg  = NULL, ... ) throw ();
      virtual ~DiagnosticException ()  throw () {
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
  class NomsgException : public DiagnosticException {
    public:
      NomsgException ( int err, int diagno, const char *msg = NULL, ... ) throw ();
      virtual ~NomsgException () throw () {
      };
  };

/**
 * Thrown when QMEABORTED is returned
 */
  class AbortedException : public DiagnosticException {
    public:
      AbortedException ( int err, int diagno, const char *msg = NULL, ... ) throw ();
      virtual ~AbortedException () throw () {
      };
  };

}
#endif

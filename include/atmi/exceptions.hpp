/*
 * atmi exceptions
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

#include <atmi/definitions.hpp>
#include <atmi.h>
#include <exception>
#include <string>
#include <cstring>

//using namespace std;

#define ATMI_MESSAGE_LENGTH 1024

namespace atmi {

/** \addtogroup errors Eceptions
 *
 * Errors are handle through these exceptions.
 *
 * @{
 */

/**
 * Base class of ATMI++ exception
 *
 * Can be used to throw any kind of error message in a consitent way.
 */
  class atmi_exception : public std::exception {
    public:
      /** create a new instance
       *
       * @param msg error message (used in snprintf)
       * @param args message parameters
       */
      template<typename... Args> atmi_exception( const char *msg, const Args&... args){
        char buff[ATMI_MESSAGE_LENGTH+1];

        memset ( buff, 0, ATMI_MESSAGE_LENGTH+1 );
        snprintf ( buff, ATMI_MESSAGE_LENGTH, msg, args... );

        _message = buff ;
        // DEBUG printf("Atmi message was set to [%s], what to [%s] (size: %d).\n", _message.c_str(), _what.c_str(), _what.size());
      };

      /** default constructor.
       *
       * set a default message.
       */
      atmi_exception();

      virtual ~atmi_exception () throw () {
        // Intentionally unimplemented...
      };

      /**
       * @return user friendly text message
       */
#if __cplusplus < 201103L
      virtual const char *what() const throw () ;
#else
      virtual const char *what() const noexcept override;
#endif

      /** @return explanatory error message. */
#if __cplusplus < 201103L
      virtual const char *message() throw() ;
#else
      virtual const char *message() const noexcept ;
#endif

    protected:
      std::string _what;    //!< what message (often the concatenation of message + other infos)
      std::string _message; //!< error message
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
       * @param args error message parameters (variadic).
       */
      template<typename... Args> unix_exception( int err, const char *msg, const Args&... args): _error(err), atmi_exception(msg, args...){
        _what = _message + " " + strerror ( _error );
      };

      /** new unix exception.
       *
       * error is defaulted to errno
       *
       * @param msg error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> unix_exception( const char *msg, const Args&... args): unix_exception(errno, msg, args...){
      };

      /** default constructor.
       *
       * set error message to strerror.
       */
      unix_exception();

      virtual ~unix_exception () throw() {
        // Intentionally unimplemented...
      };

      /** @return unix errno
       */
      int error () const;

    private:
      int _error; //!< unix errno
  };

  /** FML buffer related exceptions.
   *
   */
  class buffer_exception : public atmi_exception {
    public:
      /** new buffer exception.
       *
       * @param err  Ferror32 value
       * @param msg  error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> buffer_exception( int err, const char *msg, const Args&... args): _error(err), atmi_exception(msg, args...){
        _what = _message + " " + error_message();
      };

      /** default constructor.
       *
       * set error message to strerror.
       */
      buffer_exception();

      /**
       * @return tuxedo FML error number
       */
      int error () const;

      /**
       * @return tuxedo FML error message string
       */
       const char *error_message () const;

    private:
      int _error; //!< related Ferror32 error number.
  };

  /** Tuxedo TP related exceptions
   *
   */
  class tuxedo_exception : public atmi_exception {
    public:
      /**
       * Tuxedo exceptions.
       *
       * @param err  Ferror32 value
       * @param msg  error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> tuxedo_exception( int err, const char *msg, const Args&... args): _error(err), _detail(0), atmi_exception(msg, args...){
        _what = _message + " " + error_message();
      };

      virtual ~tuxedo_exception () throw () {
        // Intentionally unimplemented...
      };

      /**
       * @return tperr that raise the exception
       */
      virtual inline int error () const {
        return _error;
      };

      /**
       * @return detail error number
       */
      inline int detail () const {
        return _detail;
      };

      /**
       * @return tuxedo error message string
       */
      virtual const char *error_message () const ;

      /**
       * @return tuxedo error detail string
       */
      const char *error_detail () const ;

    private:

      int _error; //!< A Tuxedo error number (tperrno)
      int _detail;//!< Tuxedo detail error number

  };

  /**
   * Thrown when a TPESVCERR s returned after a TP call.
   */
  class service_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param msg  error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> service_exception( const char *msg, const Args&... args): tuxedo_exception(TPESVCERR, msg, args...){
      };
      virtual ~service_exception () throw () {
        // Intentionally unimplemented...
      };
  };

  /**
   * Thrown when TPETIME is returned after a TP call.
   */
  class timeout_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param msg  error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> timeout_exception( const char *msg, const Args&... args): tuxedo_exception(TPETIME, msg, args...){
      };
      virtual ~timeout_exception () throw (){
        // Intentionally unimplemented...
      };
  };

  /**
   * Thrown when a blocking condition is detected (TPEBLOCK .
   */
  class blocking_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param msg  error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> blocking_exception( const char *msg, const Args&... args): tuxedo_exception(TPEBLOCK, msg, args...){
      };
      virtual ~blocking_exception () throw () {
        // Intentionally unimplemented...
      };
  };

  /**
   * Thrown when TPGOTSIG is returned after a signal was received.
   */
  class interrupt_exception : public tuxedo_exception {
    public:
      /** new instance.
       *
       * @param msg  error message
       * @param args error message parameters (variadic).
       */
      template<typename... Args> interrupt_exception( const char *msg, const Args&... args): tuxedo_exception(TPGOTSIG, msg, args...){
      };
      virtual ~interrupt_exception () throw () {
        // Intentionally unimplemented...
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
       * @param args error message parameters (variadic).
       */
      template<typename... Args> diagnostic_exception( int err, long diagno, const char *msg, const Args&... args): _diagno(diagno), tuxedo_exception(err, msg, args...){
        if ( error() == TPEDIAGNOSTIC ) {
          _what = _message + " " + diagnostic_message ();
        } else {
          _what = _message + " " + error_message ();
        }
      };
      virtual ~diagnostic_exception ()  throw () {
        // Intentionally unimplemented...
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
       * @param msg    error message format.
       * @param args   error message parameters
       */
      template<typename... Args> nomsg_exception( const char *msg, const Args&... args): diagnostic_exception(TPEDIAGNOSTIC, QMENOMSG, msg, args...){
      };
      virtual ~nomsg_exception () throw () {
        // Intentionally unimplemented...
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
       * @param msg error message format.
       * @param args error message parameters
       */
      template<typename... Args> aborted_exception( const char *msg, const Args&... args): diagnostic_exception(TPEDIAGNOSTIC, QMEABORTED, msg, args...){
      };
      virtual ~aborted_exception () throw () {
        // Intentionally unimplemented...
      };
  };

/* @} */

} // atmi namespace
#endif

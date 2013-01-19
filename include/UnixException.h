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
#include <stdarg.h>
#include <exception>
#include <string>

#ifndef __UNIX_EXCEPTION__
#define __UNIX_EXCEPTION__

using namespace std;

namespace atmi {

  /**
  *
  * Unix related exceptions
  *
  */
  class UnixException : public exception {
    public:
      /** Add a system message after the error message. The system message is returned by
       * strerror. The last errno value is used to set the system message.
       * @param msg user message.
       */
      UnixException ( const char *msg = NULL, ... ) throw ();

      /** Add a system message after the error message. The system message is returned by
       * strerror.
       * @param err error number (like errno).
       * @param msg user message.
       */
      UnixException ( int err=0, const char *msg = NULL, ... ) throw ();

      virtual ~UnixException () throw () {
      };

      /**
       * @return user friendly text message
       */
      virtual const char *what () throw ();

      int get_errno ();
    protected:
      void setup_message ( const char *msg, va_list args );

      int error;
      string message;
  };
}
#endif

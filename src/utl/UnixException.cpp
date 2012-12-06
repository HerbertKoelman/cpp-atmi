#include <iostream>
using namespace std;

#include <stdarg.h>
#include <stdio.h>
#include <UnixException.h>
#include <cerrno>
#include <string.h>


int UnixException::get_errno () {

	return error;
}

const char *UnixException::what() throw () {

	message = message + ": " + strerror ( error );

	return message.c_str();
}

UnixException::UnixException ( int err, const char *msg, ... ) throw () {

  if ( msg != NULL ) {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
  }

	this->error = ( err == 0 ? errno : err );;
}

UnixException::UnixException ( const char *msg, ... ) throw () {

  if ( msg != NULL ) {
    va_list ap;

    va_start ( ap, msg );
    setup_message ( msg, ap );
    va_end (ap);
  }

	this->error = errno;
}

void UnixException::setup_message ( const char *msg, va_list args ) {

	if ( msg == NULL )
		message = "Unix error occured.";
	else {
		int len = 100;
		char *buff = new char[len];

		// try to fit message into default buffer size
		len = vsnprintf ( buff, len, msg, args );

		if ( len > 100 ) {

			// didn't fit into default buffer size.
			delete[] buff;

			len += 1;
			buff = new char[len];

			vsnprintf ( buff, len, msg, args );
		}

		message = buff;
	}
}


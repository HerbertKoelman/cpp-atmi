/** implementation of Socket class
 */
#include <Network.h>
#include <cerrno>
#include <iostream>
#include <fcntl.h>

/** Create a new socket descriptor (AF_INET, SOCK_STREAM)
 */
Socket::Socket () throw ( UnixException ) {
	int rc = -1, on = 1 ;
	socket_id = -1 ;
	wrapper = false ;

	socket_id = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( socket_id >= 0 ){

	  flags = fcntl(socket_id, F_GETFL, 0); // save default flags
    if (setsockopt(socket_id, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0 ){

      throw UnixException ( errno, "setsockopt() failed");
		}
	} else {
    throw UnixException ( errno, "socket() failed" );
	}
}

Socket::Socket ( int fd ){
  socket_id = fd ;
	wrapper = true ;
  flags = fcntl(socket_id, F_GETFL, 0); // save default flags
  if ( fcntl(socket_id, F_SETFL, flags | O_NONBLOCK) < 0 ) {
    throw UnixException ( errno, "fcntl() failed to set ONONBLOCK");
  }
}

Socket::~Socket() {
	if ( ! wrapper ) { // It's not a wrapper, so we can close the descriptor
		close (socket_id);
  }
}

void Socket::write ( const string &buffer ) throw ( UnixException ){

  write ( buffer.c_str(), buffer.size() );
}

void Socket::write ( const char *buffer, size_t size ) throw ( UnixException ){

  if ( send ( socket_id, buffer, size, 0 ) == -1 ) {

		throw UnixException ( "send on socket %d failed", socket_id );
	}
}

int Socket::read ( string &buffer ) throw ( UnixException ){

	int len ;
	char *rbuff = new char[1024]; // Read buffer
	buffer.clear() ;
	do {
		memset ( rbuff, 0, sizeof (buffer));
		len = read ( rbuff, sizeof (rbuff));
		buffer += rbuff ;
	} while ( len != EWOULDBLOCK );
  
	delete rbuff ;
  return buffer.size();
}

int Socket::read ( char *buffer, size_t size ) throw ( UnixException ){

  int len = 0;

	len = recv ( socket_id, buffer, size, 0 );

	if ( len < 0 ) {
    if ( errno != EWOULDBLOCK ) {
		  throw UnixException ( "read on socket %d failed", socket_id );
    } else {
      len = errno ;
    }
	}

	return len;
}

/** @return current socket descriptor (FD)
*/
int Socket::get_socket_id() {

  return socket_id ;
}

void Socket::set_socket_id( int id ) {

	close ( socket_id );
  socket_id = id ;
}

bool Socket::is_wrapper () {
	return wrapper ;
}

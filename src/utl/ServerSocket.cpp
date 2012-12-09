#include <Network.h>
#include <iostream>
#include <string.h>
#include <UnixException.h>
#include <cerrno>
#include <fcntl.h>


/**
 * Open a socket and bind it to given port number. Bind address is set to INADDR_ANY.
 *
 * @param port port number to bind listenr to
 * @throws UnixException if bind fails
 */
ServerSocket::ServerSocket( int port ) throw ( UnixException ){

  set_timeout ( 30 );
  FD_ZERO ( &server_set );
  max_socket_descriptor = 0;
  memset ( &addr, 0, sizeof ( addr));

  int flags;
  flags = fcntl(get_socket_id(), F_GETFL, 0); // save default flags
  if ( fcntl(get_socket_id(), F_SETFL, flags | O_NONBLOCK) < 0 ) {
    throw UnixException ( errno, "fcntl() failed to set ONONBLOCK");
  }

  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);
  if ( bind(get_socket_id(), (struct sockaddr *)&addr, sizeof(addr)) >= 0 ) {
    if ( listen ( get_socket_id(), 32 ) < 0 ) {
      throw UnixException (errno, "listen on port %d failed", port);
    }

    max_socket_descriptor = get_socket_id();
    FD_SET ( get_socket_id(), &server_set );

  } else {
    //throw UnixException(errno, "bind on port %d failed", port);
    throw UnixException( "bind on port %d failed", port);
  }
}

bool ServerSocket::select_read_sockets ( vector<Socket> &selection ) {

  fd_set working_set;
  struct timeval timeout;
  int desc = 0;       // current working file descriptor
  int flagged_descriptors = 0;        // number of descriptors that where flagged in select call

  memcpy ( &working_set, &server_set, sizeof (server_set));

  timeout.tv_sec = seconds;  // select() may update the timeout argument to indicate how much time was left
  timeout.tv_usec = 0;

  flagged_descriptors = select ( max_socket_descriptor + 1, &working_set, NULL, NULL, &timeout );
  if ( flagged_descriptors >= 0 ) {

    for ( desc = 0; desc <= max_socket_descriptor && flagged_descriptors > 0; ++desc ) {

      if ( FD_ISSET ( desc, &working_set ) ) {

        // we found a flagged descriptor
        flagged_descriptors--;
        if ( desc == get_socket_id() ) {                         // This is the listening socket, meaning we have a new incoming connection
          accept_connections(); // handle all waiting incoming connections
        } else {
          selection.push_back ( Socket ( desc ) );
        }
      }
    }
  }

  return (flagged_descriptors >= 0 );
}

void ServerSocket::accept_connections () {

  int new_desc = 0;

  do {
    new_desc = accept ( get_socket_id(), NULL, NULL );
    if ( new_desc >= 0 ) {
      FD_SET ( new_desc, &server_set );
      if ( max_socket_descriptor < new_desc ) {
        max_socket_descriptor = new_desc;
      }
    }
  } while ( new_desc != -1 );
}

void ServerSocket::set_timeout ( int seconds ) {
  this->seconds =  seconds;
}

int ServerSocket::get_timeout () {
  return seconds;
}

int ServerSocket::get_max_socket_descriptor() {
  return max_socket_descriptor;
}

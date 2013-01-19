#include <Network.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <UnixException.h>
#include <cerrno>
#include <netdb.h>
#include <stdlib.h>

namespace atmi {

  /**
   * Open a socket and bind it to given port number. Bind address is set to INADDR_ANY.
   *
   * @param port port number to bind listenr to
   * @throws UnixException if bind fails
   */
  ClientSocket::ClientSocket( const char *srv, int port ) throw ( UnixException ){

    struct addrinfo *result, *res, hints;
    stringstream s_port;
    int sfd = -1, err = -1;

    s_port << port;

    // Setup hint (alias filter)
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;        //AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;

    set_timeout ( 30 );

    if ( getaddrinfo ( srv, s_port.str().c_str(), &hints, &result ) == 0 ) {

      for ( res = result; res != NULL; res=res->ai_next ) {

        sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sfd != -1) {

          if (connect(sfd, res->ai_addr, res->ai_addrlen) != -1) {
            cout << "connected" << endl;
            break;
          } else {
            err = errno;
          }

          close(sfd);
        } else {
          err = errno;
        }
      }

      freeaddrinfo ( result );

      if ( sfd < 0 ) {
        throw UnixException (errno, "failed to connect to server %s (%d).", srv, port );
      } else {
        set_socket_id ( sfd );
      }
    } else {
      throw UnixException (err, "failed to resolve name %s(%d).", srv, port );
    }
  }

  void ClientSocket::set_timeout ( int seconds ) {
    this->seconds =  seconds;
  }

  long ClientSocket::get_timeout () {
    return seconds;
  }
}

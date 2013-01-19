/**
 * ServerSocket setup a listener socket 
 *
 * @author Herbert Koelman
 */
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <UnixException.h>
#include <vector>
#include <string>

#ifndef __ATMI_NETWORK__
#define __ATMI_NETWORK__

using namespace std ;

namespace atmi {

  /**
   * Within the operating system and the application that created a socket, the socket is referred to by a unique integer number called socket identifier or socket number.
   * The operating system forwards the payload of incoming IP packets to the corresponding application by extracting the socket address information from the IP and
   * transport protocol headers and stripping the headers from the application data.
   *
   * Socket represents a non blocking network socket.
   *
   * @author herbert koelman
   */
  class Socket {
  public:
    /** Creates an instance of an AF_INET SOCK_STREAM
    */
    Socket () throw ( UnixException );

    /** Creates a wrapper instance to a socket
     *
     * @param fd socket's file descriptor.
     */
    Socket ( int fd ) ;

    virtual ~Socket();

    /** Fills the passed buffer with at most size characters.
     *
     * @param buffer buffer to fill
     * @param size buffer size
     * @return number of characters read or EWOULDBLOCK
     */
    int read ( char *buffer, size_t size ) throw ( UnixException );

    /** Write the content of the given buffer
     *
     * @param buffer data buffer to write
     * @param size number of characters to write
     */
    void write ( const char *buffer, size_t size ) throw ( UnixException );

    /** Fills the passed string buffer with at most size characters.
     *
     * @param buffer buffer to fill
     * @return the number of character read
     */
    int read ( string &buffer ) throw (UnixException);

    /** Write the content of the given buffer
     *
     * @param buffer string to write
     */
     void write ( const string &buffer ) throw ( UnixException );

    int get_socket_id () ;

    /** @return true if this instance is a wrapper (meaning it will not close the underlying socket)
     */
    bool is_wrapper();

  protected:
    void set_socket_id ( int id );

  private:
    int socket_id ;
    int flags ;
    bool wrapper ;
  };

  /**
   * Create a client tcp/ip socket.
   */
  class ClientSocket: public Socket {
  public:
    ClientSocket ( const char *server, int port ) throw ( UnixException );
    virtual ~ClientSocket () {};

    void set_timeout ( int seconds ) ;
    long get_timeout ( ) ;

  private:
    struct sockaddr addr ;
    struct timespec timeout ;
    long seconds ;
  };

  /**
   * Create a socket ready to listen on a given tcp/ip address and port.
   */
  class ServerSocket: public Socket {
  public:
    ServerSocket ( int port ) throw ( UnixException );
    virtual ~ServerSocket() {};

    /** wait for a socket to be signled to have some data to read
     *
     * @param selection a vector of sockets that were selected
     * @return true if sockets were selected.
     */
    bool select_read_sockets ( vector<Socket> &selection );


    /** Set the maximun number of seconds the server will wait for network events
     * @param seconds seconds to wait
     */
    void set_timeout ( int seconds ) ;

    /** @return the numbers of seconds the server socket is waiting
    */
    int get_timeout () ;

    int get_max_socket_descriptor() ;

  private:

    void accept_connections () ;

    struct sockaddr_in addr ;
    long seconds ;
    fd_set server_set ;
    int max_socket_descriptor ;

  } ;
}
#endif

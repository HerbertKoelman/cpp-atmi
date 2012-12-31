#include <atmi++.h>
#include <Network.h>
#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
#include <errno.h>
#include <UnixException.h>

using namespace std;
// using namespace atmi ;

bool RUNNING = true;

void signal_handler ( int signal ) {

  RUNNING = false;
  cout << "signaled handled, exiting soon." << endl;
}

int main ( int argc, char **argv ) {

  string str = (string) "salut les copains";
  vector<Socket> sockets;
  char *buffer = NULL;
  int port = 13000;

  signal ( SIGINT, signal_handler );

  try {

    buffer = new char[1024];

    ServerSocket server ( port );
    cout << "server binded on " << port << "." << endl;

    while ( RUNNING ) {

      sockets.clear();
      if ( server.select_read_sockets( sockets ) ) {

        for ( int i = 0; i < sockets.size (); i++ ) {

          int len;

          str.clear();
          do {
            memset ( buffer, 0, sizeof (buffer));
            len = sockets[i].read ( buffer, sizeof ( buffer) );
            if ( len > 0 ) str += buffer;
          } while ( len != EWOULDBLOCK );
          cout << str << endl;
        }
      } else {
				cout << "timed out..." << endl ;
			}
    }

    delete buffer;

    cout << "type enter to end program." << endl;
    getline(cin, str );

  } catch ( UnixException &err ) {
    cerr << "Hello test program failed. " << err.what() << endl;
  } catch ( atmi::Exception &err ) {
    cerr << "Hello test program failed. " << err.what() << endl;
  } catch ( ... ) {
    cerr << "Hello test program failed." << endl;
  }
}

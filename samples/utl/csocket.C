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

  try {

    ClientSocket client( "chatelet", 13000);
    client.write ("hello world." );

  } catch ( UnixException &err ) {
    cerr << err.what() << endl;
  } catch ( ... ) {
    cerr << "program failed." << endl;
  }
}

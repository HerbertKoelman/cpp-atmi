#include <atmi.h>
#include <string>
#include <iostream>

using namespace std;

class base {
public:
  const char *what ();
private:
  string buffer;
};

class super : public base {
public:
};

const char *base::what () {

  buffer = "Hello world";
  return buffer.c_str();
}

int main ( int argc, char **argv ) {

  long flags = TPNOFLAGS;

  cout << "flags: " << flags << endl;

  flags |= TPQWAIT;
  cout << "flags 1: " << flags << endl;

  flags |= TPQWAIT;
  cout << "flags 2: " << flags << endl;

//  try {
//    throw super() ;
//  } catch ( base& err ) {
//    cout << "What: " << err.what()  << endl ;
//  }
}


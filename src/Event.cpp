/*
   $Id$

   Helper for handling tuxedo EVENTS porting calls
 */

#include <unistd.h>
#include <iostream>
#include <atmi/atmi++.h>

#include <string.h>

using namespace std;

namespace atmi {

  Event::Event( const char *eventname ) {
    this->eventname = eventname;
    set_flags ( TPNOTRAN );
  }

  long Event::post ( const char *data, long len ) throw (TuxedoException ){
    int ret = -1;

    if ( tppost ( const_cast<char *> (eventname), const_cast<char *>(data), len, flags ) != -1 ) {
      // get number of events that were dispatched see tpurcode
      ret = tpurcode;
    }else{
      // Throw an EventException
      handleTperrno ( tperrno, "Post event %s failed.", eventname );
    }

    return ret;
  }

}

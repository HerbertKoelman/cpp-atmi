/*
   $Id$

   Helper for handling tuxedo EVENTS porting calls
 */

#include <unistd.h>
#include <iostream>
#include <atmi/event.hpp>

using namespace std;

namespace atmi {

  event::event( const char *eventname ) {
    this->eventname = eventname;
    set_flags ( TPNOTRAN );
  }

  long event::post ( const char *data, long len ) throw (tuxedo_exception ){
    int ret = -1;

    if ( tppost ( const_cast<char *> (eventname), const_cast<char *>(data), len, _flags ) != -1 ) {
      // get number of events that were dispatched see tpurcode
      ret = tpurcode;
    }else{
      // Throw an eventException
      handle_tperrno ( tperrno, "Post event %s failed.", eventname );
    }

    return ret;
  }

}

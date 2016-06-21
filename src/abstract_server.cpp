/*
   $Id: abstract_server.C 79 2007-08-18 17:30:26Z hkoelman $
 */

/*
 * abstract_server
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * abstract_server is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * abstract_server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with abstract_server; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <iostream>
using namespace std;

#include <string>

#include <atmi/atmi++.hpp>

namespace atmi {

  abstract_server::abstract_server () {
    if ( tpopen () == -1 ) throw tuxedo_exception ( tperrno, catgets ( _catd, CATD_ATMI_SET, 1, "Call to tpopen() failed.") );
  };

  abstract_server::~abstract_server () {
    if ( tpclose () == -1 ) ; // throw tuxedo_exception ( tperrno, catgets ( _catd, CATD_ATMI_SET, 2, "Call to tpclose() failed.a") );
  };

  void abstract_server::tpsvrthrdone( void ) {
    if ( tpclose () == -1 ) throw tuxedo_exception ( tperrno, catgets ( _catd, CATD_ATMI_SET, 3, "Thread done call to tpclose() failed.") );
  }

  int abstract_server::tpsvrthrinit( int argc, char **argv ) {
    if ( tpopen () == -1 ) throw tuxedo_exception ( tperrno, catgets ( _catd, CATD_ATMI_SET, 4, "Thread init call to tpopen() failed.") );

    return -1;
  }


}

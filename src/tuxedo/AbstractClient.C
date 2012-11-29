/*
 * AbstractClient
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * AbstractClient is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * AbstractClient is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbstractClient; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <iostream>
#include <string>
#include <atmi++.h>

using namespace std;

namespace atmi {

  AbstractClient::~AbstractClient () {
    if ( context > 0 ) tpsetctxt ( context, 0 );
    tpterm ();
  };

  AbstractClient::AbstractClient ( char *cltname, char *usr, char *passwd, char *group, char *tuxconfig) {

    int rc = -1;


    if ( cltname != NULL || usr != NULL || passwd != NULL || tuxconfig != NULL ) {


      tpinfo = (TPINIT *) tpalloc ( const_cast<char *>("TPINIT"), NULL, TPINITNEED (0)  );

      if ( tuxconfig != NULL ) {
        tpinfo->flags = TPMULTICONTEXTS;
        tuxputenv(tuxconfig);
      }

      if ( usr != NULL ) strncpy ( tpinfo->usrname, usr, MAXTIDENT );
      if ( passwd != NULL ) strncpy ( tpinfo->passwd,  passwd, MAXTIDENT );
      if ( group != NULL ) strncpy ( tpinfo->grpname,  group, MAXTIDENT );
      if ( cltname != NULL ) strncpy ( tpinfo->cltname, cltname, MAXTIDENT );
    }

    rc = tpinit ( tpinfo );

    if ( rc < 0 ) {
      //  handleTperrno ( tperrno, "TPINIT failed. Is application started ? Check ULOG for more." );
      throw TuxedoException  ( tperrno, catgets ( catd, CATD_ATMI_SET, 39, "TPINIT failed. Is application started ? Check TUXCONFIG env var and ULOG for more.") );
    } else {

      if ( tpgetctxt ( &context, 0 ) < 0 ) {
        context = 0;
        throw TuxedoException ( tperrno, catgets ( catd, CATD_ATMI_SET, 34, "Context switch failed while calling tpgetctxt ( target context was %d )."), context );
      }

    }

  }

  ATp AbstractClient::new_tp_class ( char *svc ){

    auto_ptr<Tp> ptr;
    ptr.reset ( new Tp ( svc ));
    ptr->set_context ( context );
    return ptr;
  }

  AQueue AbstractClient::new_queue_class ( char *qspace, char *queue ){

    auto_ptr<Queue> ptr;
    ptr.reset ( new Queue ( qspace, queue ));
    ptr->set_context ( context );
    return ptr;
  }

}

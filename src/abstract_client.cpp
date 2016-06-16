/*
 * abstract_client
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * abstract_client is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * abstract_client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with abstract_client; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <atmi/atmi++.hpp>

using namespace std;

namespace atmi {

  abstract_client::~abstract_client () {
    switch_context() ; // unused remove when possible if ( context() > 0 ) tpsetctxt ( context, 0 );
    tpterm ();
  };

  abstract_client::abstract_client ( bool tuxconfig, const char *cltname, const char *usr, const char *passwd, const char *group) {

    string tc = string ("TUXCONFIG=") + getenv ("TUXCONFIG");
    setup_client ( cltname, usr, passwd, group, tc.c_str() );
  }

  abstract_client::abstract_client ( const char *cltname, const char *usr, const char *passwd, const char *group, const char *tuxconfig) {
    setup_client ( cltname, usr, passwd, group, tuxconfig );
  }

  // TODO protect this code with a mutex
  void abstract_client::setup_client ( const char *cltname, const char *usr, const char *passwd, const char *group, const char *tuxconfig){

    int rc = -1;

    if ( cltname != NULL || usr != NULL || passwd != NULL || tuxconfig != NULL ) {

      tpinfo = (TPINIT *) tpalloc ( const_cast<char *>("TPINIT"), NULL, TPINITNEED (0)  );

      if ( tuxconfig != NULL ) {
        tpinfo->flags = TPMULTICONTEXTS;
        if ( tuxputenv(const_cast<char *>(tuxconfig)) != 0 ) {
          throw atmi_exception ("failed to put env varaible %s", tuxconfig );
        }
      }

      if ( usr != NULL ) {
        strncpy ( tpinfo->usrname, usr, MAXTIDENT );
      } else {
        strncpy ( tpinfo->usrname, (getenv ("LOGNAME") == NULL ? "void" : getenv ("LOGNAME")), MAXTIDENT );
      }

      if ( passwd != NULL ) {
        strncpy ( tpinfo->passwd,  passwd, MAXTIDENT );
      }else {
        if (tpchkauth() == TPSYSAUTH ) {
          char *p = getpass ("enter application password: ");
          strncpy ( tpinfo->passwd,  p, MAXTIDENT );
        }
      }

      if ( group != NULL ) strncpy ( tpinfo->grpname,  group, MAXTIDENT );
      if ( cltname != NULL ) strncpy ( tpinfo->cltname, cltname, MAXTIDENT );
    }

    rc = tpinit ( tpinfo );

    if ( rc < 0 ) {
      //  handletransactionerrno ( tperrno, "TPINIT failed. Is application started ? Check ULOG for more." );
      if (  tperrno == TPEPERM ) {
        throw tuxedo_exception  ( tperrno, "TPINIT access permission to DOMAIN denied. Check user and password." );
      }else{
        throw tuxedo_exception  ( tperrno, catgets ( _catd, CATD_ATMI_SET, 39, "TPINIT failed. Is application started ? Check TUXCONFIG env var and ULOG for more.") );
      }
    } else {
      TPCONTEXT_T ctxt = 0;
      if ( tpgetctxt ( &ctxt, 0 ) < 0 ) {
        set_context(ctxt);
        throw tuxedo_exception ( tperrno, catgets ( _catd, CATD_ATMI_SET, 34, "Context switch failed while calling tpgetctxt ( target context was %d )."), context() );
      }
    }
  }

  tp_auto_ptr abstract_client::new_tp_instance ( const char *svc ){


#if __cplusplus < 201103L
    auto_ptr<transaction> ptr;
#else
    unique_ptr<transaction> ptr;
#endif

    ptr.reset ( new transaction ( svc ));
    ptr->set_context ( context );
    return ptr;
  }

  queue_auto_ptr abstract_client::new_queue_instance ( const char *qspace, const char *queue, const char *reply ){

#if __cplusplus < 201103L
    auto_ptr<atmi::queue> ptr;
#else
    unique_ptr<atmi::queue> ptr;
#endif
    ptr.reset ( new atmi::queue ( qspace, queue, reply ));
    ptr->set_context ( context );
    return ptr;
  }

}

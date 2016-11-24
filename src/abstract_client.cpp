/*
 * abstract_clien
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
#include <sstream>
#include <string>
#include <atmi/exceptions.hpp>
#include <atmi/abstract_client.hpp>

// using namespace std;

namespace atmi {

  abstract_client::~abstract_client () {
    switch_context() ; // unused remove when possible if ( context() > 0 ) tpsetctxt ( context, 0 );
    tpterm ();
  };

  abstract_client::abstract_client (): abstract_client(NULL, NULL, NULL, NULL, NULL, NULL) {// cltname is passed as NULL

  }

  abstract_client::abstract_client ( const char *cltname, const char *usr, const char *sys_passwd, const char *app_passwd, const char *group, const char *tuxconfig){

    int rc = -1;

    // these will store copies of char buffers
    const char *spasswd = NULL; // reference to system passwd
    const char *apasswd = NULL; // reference to application passwd
    size_t      apasswd_size = 0;

    auto authentication_requirement = tpchkauth(); // check if authentification is required

    switch (authentication_requirement){
      case TPAPPAUTH:
        apasswd = ( app_passwd != NULL ? app_passwd : getpass ("enter application password: "));
        apasswd_size = strlen(apasswd) + 1 ;

      case TPSYSAUTH:
        spasswd = ( sys_passwd != NULL ? sys_passwd : getpass ("enter system (DOMAIN) password: "));
        break;
    }

    // allocate INIT structure and set fields
    _tpinfo = (TPINIT *) tpalloc ( const_cast<char *>("TPINIT"), NULL, TPINITNEED (10)  );

    if ( apasswd != NULL ) {
      strcpy ( (char *)&_tpinfo->data, apasswd );
      _tpinfo->datalen = apasswd_size ;
#     ifdef DEBUG
      printf("DEBUG app pass: %s -> %s(datalen: %d, len: %d)\n", apasswd, (char *) &_tpinfo->data, _tpinfo->datalen, apasswd_size);
#     endif
    }

    if ( spasswd != NULL ) {
      strncpy ( _tpinfo->passwd, spasswd, MAXTIDENT );
#     ifdef DEBUG
      printf("DEBUG sys pass: %s -> %s\n", spasswd, _tpinfo->passwd );
#     endif
    }

    // when we receive this parameter we setup things so they can run in a multi context mode.
    if ( tuxconfig != NULL ) {
      _tuxconfig = tuxconfig;
      _tpinfo->flags = TPMULTICONTEXTS;

      // tuxconfig parameter is a path to an UBB file, to make sure that the env var TUXCONFIG is
      // indeed referencing this path, we set this variable through an explicit call to tuxputenv.
      // The function tuxputenv expect a string in form of "key=value".
      std::string tuxconfig_kv = std::string("TUXCONFIG=")+_tuxconfig ; // create env variable entry (key=value)

      if ( tuxputenv(const_cast<char *>(tuxconfig_kv.c_str()) ) != 0 ) {
        throw atmi_exception ("failed to put env varaible %s.", tuxconfig );
      }
    }

    if ( usr != NULL ) {
      strncpy ( _tpinfo->usrname, usr, MAXTIDENT );
    } else {
      strncpy (
          _tpinfo->usrname,
          (getenv ("LOGNAME") == NULL ? "void" : getenv ("LOGNAME")),
          MAXTIDENT );
    }

    if ( group != NULL ) { strncpy ( _tpinfo->grpname,  group, MAXTIDENT ); }
    if ( cltname != NULL ) {strncpy ( _tpinfo->cltname, cltname, MAXTIDENT ); }

    rc = tpinit ( _tpinfo );

    if ( rc < 0 ) {
      if (  tperrno == TPEPERM ) {
        throw tuxedo_exception  ( tperrno, "TPINIT access to DOMAIN denied. Check user and password." );
      }else{
        throw tuxedo_exception  ( tperrno,
            catgets ( _catd, CATD_ATMI_SET, 39, "TPINIT failed. Is application started ? Is either TUXCONFIG or WSNADDR env var Set ? Check ULOG for more.")
            );
      }
    } else {
      TPCONTEXT_T ctxt = 0;
      int ret = 0;
      ret = tpgetctxt ( &ctxt, 0 ) ;
      if ( ret != -1 ) {
        switch(ctxt){
          case TPSINGLECONTEXT:
            set_context(TPSINGLECONTEXT);
            break;
          case TPNULLCONTEXT:
            throw tuxedo_exception ( tperrno, "tpgetctxt returned TPNULLCONTEXT.");
          case TPINVALIDCONTEXT:
            throw tuxedo_exception ( tperrno, "tpgetctxt returned failed.");
          default:
            set_context(ctxt);
        }
      }

      std::stringstream buff;
      buff << _tpinfo->cltname << "-context-" << context() ;
      _name = buff.str();
    }
  }


  transaction_ptr abstract_client::new_transaction_instance ( const char *svc ){

#if __cplusplus < 201103L
    std::auto_ptr<transaction>   ptr;
#else
    std::unique_ptr<transaction> ptr;
#endif

    ptr.reset ( new transaction ( svc ));
    ptr->set_context ( this->context() );
    return ptr;
  }

  queue_ptr abstract_client::new_queue_instance ( const char *qspace, const char *queue, const char *reply ){

#if __cplusplus < 201103L
    std::auto_ptr<atmi::queue> ptr;
#else
    std::unique_ptr<atmi::queue> ptr;
#endif
    ptr.reset ( new atmi::queue ( qspace, queue, reply ));
    ptr->set_context ( this->context());
    return ptr;
  }

}

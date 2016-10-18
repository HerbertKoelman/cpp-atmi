
/*
 * tuxedo
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * tuxedo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Logging tuxedo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tuxedo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef CPP_ATMI_CLIENT_HPP
#define CPP_ATMI_CLIENT_HPP

#include <typeinfo>
#include <atmi.h>
#include <fml32.h>
#include <string>
#include <cstdio>
#include <memory>
#include <nl_types.h>

#include <atmi/config.h>
#include <atmi/exceptions.hpp>
#include <atmi/transaction.hpp>
#include <atmi/queue.hpp>


namespace atmi {


  /** \addtogroup helpers Helper classes
   *
   * @{
   */

  /**
   * Helper class to implement tuxedo application.
   *
   * Extending this class ensures that tpterm  and tpinit is called when client programs are run.
   *
   * abstract_client has two modes of operation: single-context mode and multicontext mode. To run in multicontext mode you'll
   * need to pass a valid TUXCONFIG file when constructing an abstract_client instance. The multiconext mode is available
   * only for native clients.
   *
   * Two factory methods are available to construct transaction and queue class instances (new_tp_instance and new_queue_instance). These methods return tp_auto_ptr and queue_auto_ptr
   * which are auto pointers. which is probaly the best way to avoid memory leaks.
   *
   */
  class abstract_client : public tuxedo {
    public:

      /** End any pending operation and free any alloated ressource (tpterm()).
       *
       * After this call any attempt at using ATMI will fail.
       */
      virtual ~abstract_client ();

      /**
       * Join a BEA tuxedo ATMI system application by calling tpinit.
       *
       * Before a client can use any of the BEA tuxedo ATMI system communication or transaction routines, it mus
       * first join a BEA tuxedo ATMI system application by explicitly using tpinit.
       */
       abstract_client ();

      /**
       * Join a BEA tuxedo ATMI system application by calling tpinit.
       *
       * Before a client can use any of the BEA tuxedo ATMI system communication or transaction routines, it mus
       * first join a BEA tuxedo ATMI system application by explicitly using tpinit.
       *
       * This constructor sets the TPINFO flag TPMULTICONTEXTS if the parameter tuxconfig is passed.
       *
       * In a multi threaded application it is good practice to initiliaze all your clients before starting the threads.
       *
       * If passwd is NULL then the constructor checks if authentication is needed. If so it promps the user for a password.
       *
       * @param cltname client program name (default NULL)
       * @param usr user name (default NULL)
       * @param passwd user's password (default NULL)
       * @param group is used to associate the client with a resource manager group name (default NULL)
       * @param tuxconfig used located the DOMAIN
       */
      explicit abstract_client ( const char *cltname, const char *usr = NULL, const char *passwd = NULL, const char *group = NULL, const char *tuxconfig = NULL );

      /** This method must overriden  to run the client application.
       *
       * @param argc number of command line option received when the program was started
       * @param argv actual value of command line arguments
       * @deprecated
       */
      virtual int run ( int argc, char **argv ) {
        return -1;
      };

      /** Creates an instance of transaction and set the client context to be used.
       *
       * @return  an auto_ptr to a new transaction instance
       */
      transaction_ptr new_transaction_instance ( const char *svc );

      /** Creates an instance of queue and set the client context to be used.
       *
       * @return  an auto_ptr to a new queue instance
       */
      queue_ptr new_queue_instance ( const char *qspace, const char *queue, const char *reply = NULL );

      /** @return tuxedo client name */
      const char *name() const {
        return _name.c_str();
      }

      /** @return associated TUXCONFIG value */
      const char *tuxconfig() const {
        return _tuxconfig.c_str();
      }

      /** @return true if multicontext is active */
      bool multi_context(){
        return context() > 0 ;
      }

    private:

      std::string  _name;
      std::string  _tuxconfig;
      TPINIT      *_tpinfo;

  };

  /** @} */
}

#endif

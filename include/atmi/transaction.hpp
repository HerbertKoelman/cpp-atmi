/*
 * tuxedo /T operations
 *
 * author: herbert koelman (herbert.koelm@me.com)
 *
 */

#ifndef CPP_ATMI_TRANSACTION__HPP
#define CPP_ATMI_TRANSACTION__HPP

#include <atmi.h>
#include <fml32.h>

#include <typeinfo>
#include <string>
#include <cstdio>
#include <memory>
#include <nl_types.h>

#include <atmi/config.h>
#include <atmi/tuxedo.hpp>
#include <atmi/buffer.hpp>

using namespace std;

namespace atmi {

/** \addtogroup atmi 
 *
 * @{
 */

// ---------------------------------------------------------------------------------

  class transaction;

#if __cplusplus < 201103L
  typedef auto_ptr<transaction>   transaction_ptr;//!< @deprecated use unique_ptr instead
#else
  typedef unique_ptr<transaction> transaction_ptr; 
#endif

  /**
   * Implement transaction calls (/T)
   *
   * @author herbert koelman
   */
  class transaction : public tuxedo {
    public:
      /**
       * Call service. 
       *
       * @param idata a data buffer previously allocated with tpalloc() and hols input data
       * @param ilen  idata buffer lenght.
       * @param odata a data buffer previously allocated with tpalloc() and that will hold returned data by the called service.
       * @param olen  reponse buffer length.
       * @param urcode user return code (see tpreturn)
       * @param retries call is attempted at most retries times.
       * @param delay   delay in seconds between each retry.
       *
       * @return  tpurcode if TPESVCFAIL a user returned code (tpurcode) > 0
       * @return  -1 is retuened upon service failure TPFAIL and tpurcode == 0
       * @return  0 TPSUCCESS is returned
       *
       * @throw  service_exception Upon TPESVCERR.
       * @throw  timeout_exception Upon TPETIME.
       * @throw  tuxedo_exception thrown for the other error conditions.
       *
       * @see atmi::Tuxedo
       */
      int call( char *idata, long ilen, char **odata, long *olen, int *urcode = NULL,int retries = 0, int delay = 0 );

      /**
       * Call service.
       *
       * @param buffer a data buffer previously allocated
       * @param urcode user return code (see tpreturn)
       * @param retries call is attempted at most retries times.
       * @param delay   delay in seconds between each retry.
       *
       * @return  tpurcode if TPESVCFAIL a user returned code (tpurcode) > 0
       * @return  -1 is retuened upon service failure TPFAIL and tpurcode == 0
       * @return  0 TPSUCCESS is returned
       *
       * @throw  service_exception Upon TPESVCERR.
       * @throw  timeout_exception Upon TPETIME.
       * @throw  tuxedo_exception thrown for the other error conditions.
       *
       * @see atmi::Tuxedo
       */
      int call( buffer *buffer, int *urcode = NULL,int retries = 0, int delay = 0 );

      /**
       * Call service.
       *
       * Input data buffer is used as output data buffer.
       *
       * @param idata a data buffer previously allocated with tpalloc() and holds input data
       * @param ilen  idata buffer lenght.
       * @param urcode user return code (see tpreturn)
       * @param retries call is attempted at most retries times.
       * @param delay   delay in seconds between each retry.
       *
       * @return tpurcode if TPESVCFAIL a user returned code (tpurcode) > 0
       * @return -1 is retuened upon service failure TPFAIL and tpurcode == 0
       * @return 0 TPSUCCESS is returned
       *
       * @throw service_exception Upon TPESVCERR.
       * @throw timeout_exception Upon TPETIME.
       * @throw tuxedo_exception thrown for the other error conditions.
       *
       * @see transaction
       */
      int call( char **idata = NULL, long *ilen = 0, int *urcode = NULL, int retries = 0, int delay = 0 );

      /**
       * Asynchronious service call.
       *
       * @param idata - a data buffer previously allocated with tpalloc() and hols input data
       * @param ilen  - idata buffer lenght.
       *
       * @return call descriptor if call succeeded else -1
       *
       * @throw An exception is raised upon failure
       *
       * @see reply
       */
      int acall( char *idata = NULL, long ilen = 0 );

      /**
       * Asynchronious service call.
       *
       * @param buffer a fielded buffer previously allocated with tpalloc() and hols input data
       *
       * @return call descriptor if call succeeded else -1
       *
       * @throw An exception is raised upon failure
       */
      int acall( buffer *buffer );

      /**
       * get reply from previous asynchronious service call.
       *
       * @param data    a data buffer previously allocated with tpalloc() and that will hold returned data by the called service.
       * @param urcode  user return code
       * @param len     reponse buffer length.
       * @param cd      a call descriptor. if 0 then last acall descriptor is used.
       */
      int reply ( char **data, long *len,int *urcode = NULL, int *cd = NULL );

      /**
       * get reply from previous acall().
       *
       * @param buffer - a fielded buffer previously allocated with tpalloc() and that will hold returned data by the called service.
       * @param urcode - user return code
       * @param cd    - a call descriptor. if 0 then last acall descriptor is used.
       */
      int reply ( buffer *buffer,int *urcode = NULL, int *cd = NULL );

      /**
       * Cancel asynchronious service call.
       *
       * @param cd call descriptor. if 0 then last acall descriptor is used.
       */
      int cancel ( int cd = 0 );

      /** @return last asynchronous call descriptor. */
      inline int call_descriptor () {
        return _call_descriptor;
      };

      /** @return transaction/service name */
      inline string service () {
        return _service;
      };

      // contrucors/destructors -------------------------------------------------------

      /**
       * @param service service name (< 32 characters long)
       */
      explicit transaction ( const char *service ) ;

    protected:

    private:

      int    _call_descriptor;
      string _service;

  }; // class transaction

  /** @} */
} // namespace atmi

#endif


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

#ifndef __CPP_ATMI_TUXEDO__
#define __CPP_ATMI_TUXEDO__

#include <atmi/config.h>
#include <atmi/logger.hpp>

#include <typeinfo>
#include <atmi.h>
#include <fml32.h>
#include <string>
#include <stdio.h>
#include <memory>
#include <nl_types.h>

//#ifndef HAVE_CPP11_MUTEX
//#include <pthread/pthread.hpp>
//#else
//#include <mutex>
//#endif

/** catalog message set */
#define CATD_ATMI_SET 100

using namespace std;

namespace atmi {

  /** \addtogroup atmi Application-to-Transaction Monitor Interface
   * @{
   */
// ---------------------------------------------------------------------------------

  class buffer;

  class transaction;
  class queue;

#if __cplusplus < 201103L
  typedef auto_ptr<transaction> transaction_ptr;
  typedef auto_ptr<atmi::queue> queue_ptr;
#else
  typedef unique_ptr<transaction> tp_auto_ptr;    //!< @deprecated use unique_ptr instead
  typedef unique_ptr<atmi::queue> queue_auto_ptr; //!< @deprecated use unique_ptr instead
#endif

  /**
   * All common used ATMI method are group in this class.
   *
   * @author Herbert Koelman
   */
  class tuxedo {
    public:
      tuxedo ();
      virtual ~tuxedo();

      /* Method moved into the constructor of abstract_client
       * init() allows a client to join a BEA tuxedo ATMI system application. Before a client can
       * use any of the BEA tuxedo ATMI system communication or transaction routines, it can
       * first join a BEA tuxedo ATMI system application by explicitly using tpinit or implicitly by
       * issuing a service request (or any ATMI function). In the later case, the tpinit() function is
       * called by the BEA tuxedo system on behalf of the client with the tpinfo argument set to NULL.
       *
       * @param cltname client program name
       * @param usr user name
       * @param passwd user's password.
       *  int init ( char *cltname = NULL, char *usr = NULL, char *passwd = NULL) ;
       */

      /* Method moved into the destructor of abstract_client
       * End any pending operation and free any alloated ressource. After this call any attempt at
       * using ATMI will fail.
       * int term () ;
       */

      /**
       * Allocate a new buffer. 
       *
       * buffer types provided by tuxedo
       * CARRAY	   Character array (possibly containing NULL characters) that is neither encoded nor decoded during transmission
       * STRING	   NULL-terminated character array
       * FML		   FML fielded buffer
       * VIEW		   C structure or FML view
       * X_OCTET   Equivalent to CARRAY; provided for XATMI compatibility
       * X_C_TYPE  Equivalent to VIEW; provided for XATMI compatibility
       * X_ COMMON Equivalent to VIEW; provided for XATMI compatibility
       * FML32     FML32 fielded buffer, using 32-bit identifiers and offsets
       * VIEW32    C structure or FML32 view, using 32-bit identifiers, counter variables, and size variables
       * XML       buffer for XML documents
       * MBSTRING  Character array for multibyte characters
       *
       * Note that only the first eight bytes of type and the first 16 bytes
       * of subtype are significant.
       *
       * @param type buffer type to allocate (STRING, VIEW, FML, ...)
       * @param subtype buffer subtype. NULL means none
       * @param size memory size to allocate.
       *
       * @return allocated buffer or NULL if failed
       *
       * @throw tuxedo_exception if something goes wrong.
       */
      char *allocate ( const char *type, const char *subtype, long size );

      /**
       * resize buffer.
       *
       * @param buffer buffer to resize (the return value maybe different buffer)
       * @param extent number of bytes to add
       * @return reference to resized buffer
       */
      char *extend ( char *buffer, long extent );

      /**
       * Free a previously allocated tuxedo buffer.
       */
      void free ( char *buffer );

      /**
       * Starts XA transaction.
       *
       * @param timeout max duration in seconds for a transaction to complete.
       *
       * @throw transaction_exception is raised upon failure.
       */
      int begin ( int timeout = 0 );

      /**
       * Commit XA transaction.
       *
       * @throw transaction_exception on failure.
       */
      int commit ();

      /**
       * Abort XA transaction.
       *
       * @throw transaction_exception if abort failed.
       */
      int abort ();

      /**
       * @return the current errno value
       */
      inline long  error () const {
        return _errorno;
      };

      /**
       * @return an error description string of the last errno
       */
      inline const char *error_message () const {
        return tpstrerror(_errorno);
      };

      /**
       * @return the current error detail (if one exist).
       */
      inline int errnodetail() const {
        return _errornodetail;
      };
      /**
       * @return an error detail description string (if one exists).
       */
      inline const char *error_detail () const {
        return tpstrerrordetail ( _errornodetail, 0 );
      };

      /** set the tuxedo context to use when calling tuxedo functions(tpsetctxt()).
       *
       * When set, ATMI++ switches to this context before executing a tuxedo call.
       *
       * @param context context to be used by this instance
       */
      inline void set_context ( TPCONTEXT_T context) {
        _context = context;
        // printf("DEBUG set_context [%d]:\n", _context);
      };

      /** @return the current context
       */
      inline TPCONTEXT_T context () const {

        // printf("DEBUG context [%d]:\n", _context);

        return _context;
      };

      /**
       * @return tuxedo flags used by this instance.
       */
      inline long flags() const {
        return _flags;
      };

      /**
       * Set tuxedo flags
       */
      inline void set_flags ( long flags ) {
        tuxedo::set (_flags, flags);
      };

      /**
       * Reset flag value to TPNOFLAGS
       */
      inline void reset_flags ( long flags ) {
        _flags = TPNOFLAGS;
        tuxedo::set (_flags, flags);
      };

      /** unset flags
       * @param flags flags to unset
       */
      inline void unset_flags ( long flags ) {
        tuxedo::unset (_flags, flags);
      };

      static const long FAILED = -1; //!< Tuxedo error value

    protected:

      /** If context is set (context != 0 ), then tpsetctxt is called. This method does nothing otherwise.
       */
      void switch_context ();

      /** Unset bits in second argument from first argument
       */
      static long unset ( long, long );

      /** set bits in second argument in first argument
       */
      static long set ( long, long );

      /** update current errno and sets what must be set. 
       *
       * @deprecated we use exception instead
       */
      void updateErrno ();

      /**
       * Triggers exceptions according to the given tperrno passed. The exception will be initialized with msg.
       *
       * @param tpe tperrno to handle
       * @param msg message to setup in thrown exception.
       * @param Args substitution parameters
       * @return legacy will be removed when prototype will be changed to void handle_transaction_errno()
       */
        template<typename... Args>  int handle_tperrno ( int _tperrno, const char *msg, const Args&... args) {

          switch ( _tperrno ) {
            case TPEINVAL:
            case TPEPERM:
            case TPENOENT:
            case TPEITYPE:
            case TPEOTYPE:
            case TPETRAN:
            case TPEPROTO:
            case TPESYSTEM:
            case TPEOS:
            case TPELIMIT:
            {                           
              throw tuxedo_exception ( _tperrno, msg, args... );
            }
            break;
            case TPEBLOCK:
            {
              throw blocking_exception ( msg, args... );
            }
            break;
            case TPGOTSIG:
            {
              throw interrupt_exception ( msg, args... );
            }
            break;
            case TPESVCERR:
            {
              throw service_exception ( msg, args... );
            }
            break;
            case TPETIME:
            {
              throw timeout_exception ( msg, args... );
            }
            break;
            case TPESVCFAIL:
              // return application specific error number instead
              // as the application will probably know what to do
              _tperrno = ( tpurcode > 0 ? tpurcode : -1 );
              break;

            default:
              throw tuxedo_exception (_tperrno, catgets ( _catd, CATD_ATMI_SET, 33,"Never heard about this tperrno (%d)."), _tperrno );
          };

          return _tperrno;
        };

      long    _flags; //!< Tuxedo flags
      nl_catd _catd;  //!< message catalog refenence

    private:
      long        _errorno;
      long        _errornodetail;
      TPCONTEXT_T _context;

  };

// ---------------------------------------------------------------------------------

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

      /** Method moved into the destructor of abstract_client
       * End any pending operation and free any alloated ressource. After this call any attempt at
       * using ATMI will fail.
       * int term () ;
       */
      virtual ~abstract_client ();

      /**
       * Join a BEA tuxedo ATMI system application by calling tpinit. 
       *
       * Before a client can use any of the BEA tuxedo ATMI system communication or transaction routines, it must
       * first join a BEA tuxedo ATMI system application by explicitly using tpinit.
       */
       abstract_client ();

      /**
       * Join a BEA tuxedo ATMI system application by calling tpinit. 
       *
       * This constructor sets the TPINFO flag TPMULTICONTEXTS.
       *
       * In a multi threaded application it is good practice to initiliaze all your clients before starting the threads or to use a factory.
       *
       * Before a client can use any of the BEA tuxedo ATMI system communication or transaction routines, it must
       * first join a BEA tuxedo ATMI system application by explicitly using tpinit.
       *
       * If passwd is NULL then the constructor checks if authentication is needed. If so it promps the user for a password.
       *
       * @param cltname client program name (default NULL)
       * @param usr user name (default NULL)
       * @param passwd user's password (default NULL)
       * @param group is used to associate the client with a resource manager group name (default NULL)
       * @param tuxconfig used located the DOMAIN
       */
      abstract_client ( const char *cltname, const char *usr = NULL, const char *passwd = NULL, const char *group = NULL, const char *tuxconfig = NULL );

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
      inline const char *name() const {
        return _name.c_str();
      }

      /** @return associated TUXCONFIG value */
      inline const char *tuxconfig() const {
        return _tuxconfig.c_str();
      }

      /** @return true if multicontext is active */
      inline bool multi_context(){
        return context() > 0 ;
      }

    private:

      std::string  _name;
      std::string  _tuxconfig;
      TPINIT      *_tpinfo;

#ifndef HAVE_CPP11_MUTEX
//      static pthread::mutex _mutex;
#else
//      static std::mutex     _mutex;
#endif
  };

// ---------------------------------------------------------------------------------

/**
 * Helper class to implement tuxedo AsbtractServers.
 *
 * @deprecated clutters server code.
 */
  class abstract_server : public tuxedo {
    public:

      /**
       * Setup minimal requirements (mainly tpopen)
       *
       * @throw tuxedo_exception if tpopen failed.
       */
      abstract_server ();

      /**
       * minimal cleanup
       */
      ~abstract_server ();

      /**
       * Terminates a BEA tuxedo ATMI server thread
       */
      virtual void tpsvrthrdone();

      /**
       * Initializes a BEA tuxedo ATMI server thread.
       *
       * @param argc CLOPT argument count.
       * @param argv CLOPT user arguments
       *
       * @return A negative return value will cause the server dispatch thread to exit gracefully.
       */
      virtual int tpsvrthrinit( int argc, char **argv );

      /**
       * Handles SIGHUP signals. It normaly expected that the server takes actions
       * to re-initialize it's internal state.
       *
       * This method should be overloaded
       */
      virtual inline void handle_sighup ()  {                   /* not implemented */
      };

      /**
       * Handles SIGTERM signals. A server should cleanup and prepare it's self to
       * terminated.
       *
       * This method should be overloaded
       */
      virtual inline void handle_sigterm () {                   /* not implemented */
      };

  };

// ---------------------------------------------------------------------------------

/**
 * Implement TP calls
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
      transaction ( const char *service );

    protected:

    private:

      int    _call_descriptor;
      string _service;

  };

// ---------------------------------------------------------------------------------

/**
 * Handles /Q operations
 */
  class queue : public tuxedo {
    public:

      /** Creates an instance of queue
       *
       * @param qspace qspace that handles the queue
       * @param queue the queue that will be manipulated
       * @param reply name of a reply
       */
      queue ( const char *qspace = NULL, const char *queue = NULL, const char *reply = NULL );

      /** enqueue a message
       *
       * @param data tuxedo buffer (allocated with tuxedo::allocate)
       * @param len length of the tuxedo buffer
       */
      int enqueue ( char *data, long len );

      /** dequeue a message
       *
       * @param data tuxedo buffer (allocated with tuxedo::allocate)
       * @param len length of the tuxedo buffer
       */
      int dequeue ( char **data, long *len );

      /** enqueue a message
       *
       * @param data tuxedo FML32 buffer (allocated with tuxedo::allocate)
       */
      int enqueue ( buffer *data );

      /** dequeue a message
       *
       * @param data tuxedo FML32 buffer (allocated with tuxedo::allocate)
       */
      int dequeue ( buffer *data );

      /** dequeue a reply message
       *
       * Reply must have been set when queue instance was created. If the flag TP TPQCORRID is set, then the correlation id value is used to
       * retreive the message from the reply queue.
       *
       * @param data tuxedo buffer (allocated with tuxedo::allocate)
       * @param len length of the tuxedo buffer
       */
      int dequeueReply ( char **data, long *len );

      /** dequeue a reply message
       *
       * Reply must have been set when queue instance was created.
       *
       * @param data tuxedo buffer (allocated with tuxedo::allocate)
       */
      int dequeueReply ( buffer *data );

      /** enqueue a reply message
       *
       * The reply queue is identified through the Qctl structure. It is the responability
       * of the caller to set the reply queue value to use.
       *
       * @param data tuxedo buffer (allocated with tuxedo::allocate)
       * @param len length of the tuxedo buffer
       */
      int enqueueReply ( char *data, long len );

      /** enqueue a reply message
       *
       * The reply queue is identified through the Qctl structure. It is the responability
       * of the caller to set the reply queue value to use.
       *
       * @param data tuxedo buffer (allocated with tuxedo::allocate)
       */
      int enqueueReply ( buffer *data );

      /**
       * indicate in wich queue the replies should be posted in.
       *
       * @param q reply queue, if NULL then current reply queue is unset.
       */
      void set_reply_queue ( const char *q );

      /** reply message are expected to be posted onto this /Q queue.
       *
       * @return reply queue name
       */
      const char *reply_queue ();

      /**
       * Set the queuespace name to wich this queue associated.
       *
       * @param qs queue space name (32 characters max)
       */
      inline void set_queue_space ( const char *qs) {
        _qspace = const_cast<char *>(qs);
      };

      /** @return queue's queque space name */
      inline const char *queue_space () {
        return _qspace;
      };

      /** set the queue the instance is wrapping.
       *
       * @param q queue name 
       * @see set_queue_space make sure the queue name exists in the current queue space.
       */
      inline void set_queue_name ( char *q) {
        _queue = q;
      };

      /** @return the currently wrapped queue name */
      inline const char *queue_name () {
        return _queue;
      };

      /**
       * @return the current value of QCTL flags
       */
      inline long qctl_flags () {
        return _qctl.flags;
      };

      /** Wait until a message is enqueued.
       *
       * @param wait if true queue waits for a message
       */
      void set_message_wait ( bool wait );

      /** @deprecated use set_message_wait instead
       */
      inline void setQWait ( bool wait ) {
        set_message_wait (wait);
      };

      /** Check if QWait flag is set
       *
       * @return true if wait flag is set
       * @deprecated use is_message_waiting instead
       */
      inline bool isQWaiting () {
        return is_message_waiting ();
      };

      /** @return ture if TPQWAIT flag is set. */
      inline bool is_message_waiting () {
        return ((_qctl.flags & TPQWAIT) == TPQWAIT);
      };

      /** Set the desired persistence mode (memory, disk, default)
       * Accepted values are :
       * TPQQOSDEFAULTPERSIST: uses the persistence mode defined with queue
       * TPQQOSPERSISTENT: message are written onto persistent storage
       * TPQQOSNONPERSISTENT: message are written into memory only (message are lost if the queue manager is interrupted)
       *
       * @param qos desired flags
       */
      void set_quality_of_service ( long qos);

      /** @return the diagnostic code returned by last call.
       */
      inline int diagno () {
        return _diagno;
      };

      /** Calculate and set qctl corid
       *
       * Correlation ID is made of pid+threadid+current_time (in milliseconds).
       * When the correlation is set the flags TPQGETBYCORRID and TPQCORRID are also set.
       * This means that subsequent calls to dequeue methods will get only message with the correleation id that was last set.
       */
      void set_new_corrid();

      /** Reset flags and value related to correlation id to off.
       *
       */
      void unset_corrid();

    protected:
      /** handle queue diagnostic errors
       *
       * @param tux_tperrno tperrno to handle
       * @param tux_diagno diagnostic number
       * @param msg related explanatory message.
       */
      template<typename... Args> int handle_diagnostics ( int tux_tperrno, int tux_diagno, const char *msg, const Args&... args) {

        _diagno = tux_diagno;

        switch ( _diagno ) {
          case QMEINVAL:
          case QMEBADRMID:
          case QMENOTOPEN:
          case QMETRAN:
          case QMEBADMSGID:
          case QMESYSTEM:
          case QMEOS:
          case QMEPROTO:
          case QMEBADQUEUE:
          case QMENOSPACE:
          case QMERELEASE:
          case QMESHARE:
          {
            throw diagnostic_exception ( tux_tperrno, _diagno, msg, args... );
          }
          break;
          case QMEABORTED:
          {
            throw aborted_exception ( msg, args... );
          }
          break;
          /* these are not errors */
          case QMENOMSG:
            // not an error - throw nomsg_exception ( _tperrno, _diagno, msg, ap );
            break;
          default:
            throw diagnostic_exception ( tux_tperrno, _diagno, catgets ( _catd, CATD_ATMI_SET, 33, "Never heard about this diagno %d (tperrno: %d) !!??"), _diagno, tux_tperrno);
        }

        return _diagno;
      }

    private:

      /** Utiliy method used to simplify the implementation of enqueue messages
       */
      int enqueue ( char *queue,  char *data, long len );

      /** Utiliy method used to simplify the implementation of dequeue messages
       */
      int dequeue ( char *queue,  char **data, long *len );

      char   *_qspace;
      char   *_queue;

      int    _diagno;

      TPQCTL _qctl;

      void setQCTL();
  };

// ---------------------------------------------------------------------------------

  /**
   * Use this class to post an event and any accompanying data. The event is named by eventname and data, if
   * not NULL, points to the data. The posted event and its data are dispatched by the Oracle tuxedo ATMI eventBroker
   * to all subscribers whose subscriptions successfully evaluate against eventname and whose optional filter rules
   * successfully evaluate against data.
   *
   * eventname is a NULL-terminated string of at most 31 characters. eventname’s first character cannot be a dot (“.”)
   * as this character is reserved as the starting character for all events defined by the Oracle tuxedo ATMI system
   * itself.
   *
   * If data is non-NULL, it must point to a buffer previously allocated by tpalloc() and len should specify the
   * amount of data in the buffer that should be posted with the event. Note that if data points to a buffer of a type
   * that does not require a length to be specified (for example, an FML fielded buffer), then len is ignored. If data
   * is NULL, len is ignored and the event is posted with no data.
   *
   * When event is used within a transaction, the transaction boundary can be extended to include those servers
   * and/or stable-storage message queues notified by the eventBroker. When a transactional posting is made, some of
   * the recipients of the event posting are notified on behalf of the poster’s transaction (for example, servers and
   * queues), while some are not (for example, clients).
   *
   * If the poster is within a transaction and the TPNOTRAN flag is not set, the posted event goes to the eventBroker
   * in transaction mode such that it dispatches the event as part of the poster’s transaction. The broker dispatches
   * transactional event notifications only to those service routine and stable-storage queue subscriptions that used
   * the TPEVTRAN bit setting in the ctlflags parameter passed to subscribe(). Client notifications, and those
   * service routine and stable-storage queue subscriptions that did not use the TPEVTRAN bit setting in the ctlflags
   * parameter passed to subscribe(), are also dispatched by the eventBroker but not as part of the posting
   * process’s transaction.
   *
   * If the poster is outside a transaction, post() is a one-way post with no acknowledgement when the service
   * associated with the event fails. This occurs even when TPEVTRAN is set for that event (using the ctlflags
   * parameter passed to subscribe()). If the poster is in a transaction, then post() returns TPESVCFAIL when the
   * associated service fails in the event.
   */
  class event : public tuxedo {
    public:
      /**
       * Use this class to post an event and any accompanying data. The event is named by eventname and data, if
       * not NULL, points to the data. The posted event and its data are dispatched by the Oracle tuxedo ATMI eventBroker
       * to all subscribers whose subscriptions successfully evaluate against eventname and whose optional filter rules
       * successfully evaluate against data.
       *
       * @param evtname is a NULL-terminated string of at most 31 characters. eventname’s first character cannot be a dot (“.”) as this character is reserved by the Oracle tuxedo ATMI system itself.
       */
      event( const char *evtname = NULL);

      /**
       * The caller uses post() to post an event and any accompanying data. The event is named by eventname and data, if
       * not NULL, points to the data. The posted event and its data are dispatched by the Oracle tuxedo ATMI eventBroker
       * to all subscribers whose subscriptions successfully evaluate against eventname and whose optional filter rules
       * successfully evaluate against data.
       *
       * @param data it must point to a buffer previously allocated by allocate()
       * @param len len should specify the amount of data in the buffer that should be posted with the event.
       *
       * @return number of notifications number of event notifications dispatched by the eventBroker on behalf of eventname.
       */
      long post ( const char *data = NULL, long len = 0 ) throw ( tuxedo_exception );

    private:
      const char *eventname;

  };

// ---------------------------------------------------------------------------------

  /** Handles in/out operation on a queue
   *
   */
  class queue_stream : public tuxedo {
    public:

      friend ostream& operator<<(ostream& out, queue_stream& qs);
      friend istream& operator>>(istream& in, queue_stream& qs);

      /** setup a queue stream.
       *
       * @param q a queue
       */
      queue_stream ( queue *q );

      /** setup a queue stream.
       *
       * @param q a queue
       * @param bs stream buffer size.
       */
      queue_stream ( queue *q, long bs );

      /** @return the number of messages handle by last IO operation
       */
      inline long count () {
        return _count;
      };

      /** set the stream buffer size.
       * @param s buffer size (bytes)
       */
      inline void set_buffer_size ( long s ) {
        _buffer_size = s;
      };

      /** @return current buffer size */
      inline long buffer_size () {
        return _buffer_size;
      };

      /** use base64 encoding 
       * @param b if true, the encode using base64
       */
      void encode_base64(bool b);

    protected:

    private:
      /**
       * Number of handled messages during last in/out operation.
       */
      long   _count;
      long   _buffer_size;

      long   _flags;

      queue *_queue;
  };

/** Global utility to stream out the content of a queue
 *
 * @param out output stream
 * @param qs queue stream that will handle the reading of messages
 */
  extern ostream& operator<<(ostream& out, queue_stream& qs);

/** Global utility to stream in a queue
 *
 * @param in input stream
 * @param qs queue stream that handles the writing of messages to
 */
  extern istream& operator>>(istream& in, queue_stream& qs);

  /**
   * @}
   */
}

/** fake C function that can be used with autotool AC_CHECK_LIB macro
 *
 * @return the current build version
 */
extern "C" const char *atmicpp_is_present(void);

#endif

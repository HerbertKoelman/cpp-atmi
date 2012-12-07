/*
   $Id$

   Helper class for dealing with tuxedo.
 */

/*
 * Tuxedo
 *
 * Copyright (C) 2006 - herbert koelman
 *
 * Tuxedo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Logging Tuxedo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tuxedo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef __TUXEDO__
#define __TUXEDO__

#include <typeinfo>
#include <atmi.h>
#include <fml32.h>
#include <string>
#include <stdio.h>
#include <memory>
#include <nl_types.h>

// #define ATMI_VERSION "v1.3 Aix 5.3TL12 SMP O5 Tuxedo 9.1 32bits"

/** catalog message set */
#define CATD_ATMI_SET 100

using namespace std;

namespace atmi {

// ---------------------------------------------------------------------------------

        #ifndef __BUFFERS__
  class Buffer;
        #endif


  class Tp;
  class Queue;

  typedef auto_ptr<Tp> ATp;
  typedef auto_ptr<Queue> AQueue;

/**
 * All common used ATMI method are group in this class.
 *
 * @author Herbert Koelman
 */
  class Tuxedo {
public:
    Tuxedo ();
    virtual ~Tuxedo();

    /* Method moved into the constructor of AbstractClient
     * init() allows a client to join a BEA Tuxedo ATMI system application. Before a client can
     * use any of the BEA Tuxedo ATMI system communication or transaction routines, it can
     * first join a BEA Tuxedo ATMI system application by explicitly using tpinit or implicitly by
     * issuing a service request (or any ATMI function). In the later case, the tpinit() function is
     * called by the BEA Tuxedo system on behalf of the client with the tpinfo argument set to NULL.
     *
     * @param cltname client program name
     * @param usr user name
     * @param passwd user's password.
     *  int init ( char *cltname = NULL, char *usr = NULL, char *passwd = NULL) ;
     */

    /* Method moved into the destructor of AbstractClient
     * End any pending operation and free any alloated ressource. After this call any attempt at
     * using ATMI will fail.
     * int term () ;
     */

    /**
     * Allocate a new buffer of type. Depending on the type both subtype and
     * len are optional.
     *
     *
     * Buffer types provided by Tuxedo
     * CARRAY	   Character array (possibly containing NULL characters) that is neither encoded nor decoded during transmission
     * STRING	   NULL-terminated character array
     * FML		   FML fielded buffer
     * VIEW		   C structure or FML view
     * X_OCTET   Equivalent to CARRAY; provided for XATMI compatibility
     * X_C_TYPE  Equivalent to VIEW; provided for XATMI compatibility
     * X_ COMMON Equivalent to VIEW; provided for XATMI compatibility
     * FML32     FML32 fielded buffer, using 32-bit identifiers and offsets
     * VIEW32    C structure or FML32 view, using 32-bit identifiers, counter variables, and size variables
     * XML       Buffer for XML documents
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
     * @throw TuxedotException if something goes wrong.
     */
    char *allocate ( char *type, char *subtype, long size );

    /**
     * Routine to change the size of a typed buffer.
     *
     * @param buffer the buffer to resize (the return value maybe different)
     * @param extent number of bytes to add
     * @return reference to resized buffer
     */
    char *extend ( char *buffer, long extent );

    /**
     * Free a preveously allocated tuxedo buffer.
     */
    void free ( char *buffer );

    /**
     * Starts an XA transaction.
     *
     * @param timeout max duration in seconds for a transaction to complete.
     *
     * @throw TransactionException is raised upon failure.
     */
    int begin ( int timeout = 0 );

    /**
     * Commit XA transaction
     *
     * @throw TransactionException on failure.
     */
    int commit ();

    /**
     * Abort XA transaction.
     *
     * Return:
     * Upon failure an TransactionException is raised.
     */
    int abort ();

    inline long  getErrno () {
      return errorno;
    };
    inline char *getErrmsg () {
      return tpstrerror(errorno);
    };

    inline int   getErrnodetail() {
      return errornodetail;
    };
    inline char *getErrdetail () {
      return tpstrerrordetail ( errornodetail, 0 );
    };

    /** set the Tuxedo context to use when calling Tuxedo functions.
     *
     * When set, ATMI++ switches to this context (@tpsetctxt()@ ) before executing a Tuxedo call.
     *
     * @param c context to be used by this instance
     */
    inline void set_context ( TPCONTEXT_T c ) {

      context = c;
    };

    /** @return the current context
     */
    inline TPCONTEXT_T get_context () {

      return context;
    };

    inline long getFlags() {
      return flags;
    };

    inline void setFlags ( long flags ) {
      Tuxedo::set (this->flags, flags);
    };
    inline void resetFlags ( long flags ) {
      this->flags = TPNOFLAGS; Tuxedo::set (this->flags, flags);
    };
    inline void unsetFlags ( long flags ) {
      Tuxedo::unset (this->flags, flags);
    };

    /** @return ATMI++ version number */
    static inline const char *version () {
      return ATMI_VERSION;
    };

    static const long FAILED = -1;

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

    void updateErrno ();
    virtual int handleTperrno ( int, const char * = NULL, ... );

    long flags;
    nl_catd catd;

private:
    long errorno;
    long errornodetail;

    /* Moved into AbstractClient TPINIT *tpinfo ; */
    TPCONTEXT_T context;

  };

// ---------------------------------------------------------------------------------

/**
 * Helper class to implement Tuxedo application.
 *
 * Extending this class ensures that tpterm  and tpinitis called when client program are run
 */
  class AbstractClient : public Tuxedo {
public:

    /** Method moved into the destructor of AbstractClient
     * End any pending operation and free any alloated ressource. After this call any attempt at
     * using ATMI will fail.
     * int term () ;
     */
    virtual ~AbstractClient ();

    /**
     * The constructor allows a client to join a BEA Tuxedo ATMI system application by calling tpinit. Before a client can
     * use any of the BEA Tuxedo ATMI system communication or transaction routines, it can
     * first join a BEA Tuxedo ATMI system application by explicitly using tpinit or implicitly by
     * issuing a service request (or any ATMI function). In the later case, the tpinit() function is
     * called by the BEA Tuxedo system on behalf of the client with the tpinfo argument set to NULL.
     *
     * If tuxconfig is passed then the MULTICONTEXT flag is set and the newly created context is saved. Multi context applications
     * should use factory methods  to build Queue and Tp objects.
     *
     * @param cltname client program name (default NULL)
     * @param usr user name (default NULL)
     * @param passwd user's password (default NULL)
     * @param group is used to associate the client with a resource manager group name (default NULL)
     * @param tuxconf path to Tuxedo config file (same as TUXCONFIG en variable default NULL)
     */
    AbstractClient ( char *cltname = NULL, char *usr = NULL, char *passwd = NULL, char *group = NULL, char *tuxconfig = NULL);

    /** This method must overriden  to run the client application.
     *
     * @param argc number of command line option received when the program was started
     * @param argv actual value of command line arguments
     */
    virtual int run ( int argc, char **argv ) = 0;

    /** Creates an instance of Tp and set the client context to be used.
     *
     * @return  an auto_ptr to a new Tp instance
     */
    ATp new_tp_class ( char *svc );

    /** Creates an instance of Queue and set the client context to be used.
     *
     * @return  an auto_ptr to a new Queue instance
     */
    AQueue new_queue_class ( char *qspace, char *queue );

    TPCONTEXT_T get_context () {
      return context;
    };

protected:

private:

    TPINIT *tpinfo;
    TPCONTEXT_T context;

  };

// ---------------------------------------------------------------------------------

/**
 * Helper class to implement Tuxedo AsbtractServers.
 *
 */
  class AbstractServer : public Tuxedo {
public:

    /**
     * Setup minimal requirements (mainly tpopen)
     *
     * @throw TuxedoException if tpopen failed.
     */
    AbstractServer ();

    /**
     * minimal cleanup
     */
    ~AbstractServer ();

    /**
     * Terminates a BEA Tuxedo ATMI server thread
     */
    virtual void tpsvrthrdone();

    /**
     * Initializes a BEA Tuxedo ATMI server thread.
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
    virtual inline void handle_sighup ()  {                     /* not implemented */
    };

    /**
     * Handles SIGTERM signals. A server should cleanup and prepare it's self to
     * terminated.
     *
     * This method should be overloaded
     */
    virtual inline void handle_sigterm () {                     /* not implemented */
    };

  };

// ---------------------------------------------------------------------------------

/**
 * Implement TP calls
 */
  class Tp : public Tuxedo {
public:
    /**
     * call() sends a request and synchronously awaits its reply. A call to call () returns
     * 0 if call succeed, -1 when no urcode was found else urcode is returned and
     * should be > 0. All other error conditions are handled through Exceptions.
     *
     * @param idata a data buffer preveously allocated with tpalloc() and hols input data
     * @param ilen  idata buffer lenght.
     * @param odata a data buffer preveously allocated with tpalloc() and that will hold returned data by the called service.
     * @param olen  reponse buffer length.
     * @param urcode user return code (see tpreturn)
     * @param retries call is attempted at most retries times.
     * @param delay   delay in seconds between each retry.
     *
     * @return  tpurcode if TPESVCFAIL a user returned code (tpurcode) > 0
     * @return  -1 is retuened upon service failure TPFAIL and tpurcode == 0
     * @return  0 TPSUCCESS is returned
     *
     * @throw  ServiceException Upon TPESVCERR.
     * @throw  TimeoutException Upon TPETIME.
     * @throw  TuxedoException thrown for the other error conditions.
     */
    int call( char *idata, long ilen, char **odata, long *olen, int *urcode = NULL,int retries = 0, int delay = 0 );

    /**
     * call() sends a request and synchronously awaits its reply. A call to call () returns
     * 0 if call succeed, -1 when no urcode was found else urcode is returned and
     * should be > 0. All other error conditions are handled through Exceptions.
     *
     * @param buffer a data buffer preveously allocated
     * @param urcode user return code (see tpreturn)
     * @param retries call is attempted at most retries times.
     * @param delay   delay in seconds between each retry.
     *
     * @return  tpurcode if TPESVCFAIL a user returned code (tpurcode) > 0
     * @return  -1 is retuened upon service failure TPFAIL and tpurcode == 0
     * @return  0 TPSUCCESS is returned
     *
     * @throw  ServiceException Upon TPESVCERR.
     * @throw  TimeoutException Upon TPETIME.
     * @throw  TuxedoException thrown for the other error conditions.
     */
    int call( Buffer *buffer, int *urcode = NULL,int retries = 0, int delay = 0 );

    /**
     * call() sends a request and synchronously awaits its reply. A call to call () returns
     * 0 if call succeed, -1 when no urcode was found else urcode is returned and
     * should be > 0. All other error conditions are handled through Exceptions.
     *
     * Input data buffer is used to as output data buffer.
     *
     * @param idata a data buffer preveously allocated with tpalloc() and hols input data
     * @param ilen  idata buffer lenght.
     * @param urcode user return code (see tpreturn)
     * @param retries call is attempted at most retries times.
     * @param delay   delay in seconds between each retry.
     *
     * @return tpurcode if TPESVCFAIL a user returned code (tpurcode) > 0
     * @return -1 is retuened upon service failure TPFAIL and tpurcode == 0
     * @return 0 TPSUCCESS is returned
     *
     * @throw ServiceException Upon TPESVCERR.
     * @throw TimeoutException Upon TPETIME.
     * @throw TuxedoException thrown for the other error conditions.
     */
    int call( char **idata = NULL, long *ilen = 0, int *urcode = NULL, int retries = 0, int delay = 0 );

    /**
     * acall() sends a request message to the service named by svc.
     *
     * Parameters:
     * idata - a data buffer preveously allocated with tpalloc() and hols input data
     * ilen  - idata buffer lenght.
     *
     * @return call descriptor if call succeeded else -1
     *
     * @throw An exception is raised upon failure
     */
    int acall( char *idata = NULL, long ilen = 0 );

    /**
     * get reply from preveous acall().
     *
     * @param data - a data buffer preveously allocated with tpalloc() and that will hold returned data by the called service.
     * @param urcode - user return code
     * @param len  - reponse buffer length.
     * @param cd    - a call descriptor. if 0 then last acall descriptor is used.
     */
    int reply ( char **data, long *len,int *urcode = NULL, int *cd = NULL );

    /**
     * acall() sends a request message to the service named by svc.
     *
     * Parameters:
     * buffer - a fielded buffer preveously allocated with tpalloc() and hols input data
     *
     * @return call descriptor if call succeeded else -1
     *
     * @throw An exception is raised upon failure
     */
    int acall( Buffer *buffer );

    /**
     * get reply from preveous acall().
     *
     * @param buffer - a fielded buffer preveously allocated with tpalloc() and that will hold returned data by the called service.
     * @param urcode - user return code
     * @param cd    - a call descriptor. if 0 then last acall descriptor is used.
     */
    int reply ( Buffer *buffer,int *urcode = NULL, int *cd = NULL );

    /**
     * Cancel acall.
     *
     * @param cd    - a call descriptor. if 0 then last acall descriptor is used.
     */
    int cancel ( int cd = 0 );

    inline int getCallDesc () {
      return calldesc;
    };
    inline string getService () {
      return service;
    };

    // contrucors/destructors -------------------------------------------------------

    /**
     * @param service service name (< 32 characters long)
     */
    Tp ( char *service );

protected:

private:

    int calldesc;
    string service;

  };

// ---------------------------------------------------------------------------------

/**
 * Handles /Q operations
 */
  class Queue : public Tuxedo {
public:


    /** Creates an instance of Queue
     *
     * @param qspace qspace that handles the queue
     * @param queue the queue that will be manipulated
     * @param reply name of a reply
     */
    Queue ( char *qspace = NULL, char *queue = NULL, char *reply = NULL );

    /** enqueue a message
     *
     * @param data Tuxedo buffer (allocated with Tuxedo::allocate)
     * @param len length of the Tuxedo buffer
     */
    int enqueue ( char *data, long len );

    /** dequeue a message
     *
     * @param data Tuxedo buffer (allocated with Tuxedo::allocate)
     * @param len length of the Tuxedo buffer
     */
    int dequeue ( char **data, long *len );

    /** enqueue a message
     *
     * @param data Tuxedo FML32 buffer (allocated with Tuxedo::allocate)
     */
    int enqueue ( Buffer *data );

    /** dequeue a message
     *
     * @param data Tuxedo FML32 buffer (allocated with Tuxedo::allocate)
     */
    int dequeue ( Buffer *data );

    /** dequeue a reply message
     *
     * Reply must have been set when Queue instance was created.
     *
     * @param data Tuxedo buffer (allocated with Tuxedo::allocate)
     * @param len length of the Tuxedo buffer
     */
    int dequeueReply ( char **data, long *len );

    /** enqueue a reply message
     *
     * The reply queue is identified through the Qctl structure. It is responability
     * of the caller to set the reply queue value to use.
     *
     * @param data Tuxedo buffer (allocated with Tuxedo::allocate)
     * @param len length of the Tuxedo buffer
     */
    int enqueueReply ( char *data, long len );

    void set_reply_queue ( char * );
    const char *get_reply_queue ();

    inline void set_queue_space ( char *qs) {
      this->qspace = qs;
    };
    inline const char *get_queue_space () {
      return qspace;
    };

    inline void set_queue ( char *q) {
      this->queue = q;
    };
    inline const char *get_queue () {
      return queue;
    };

    /**
     * @return the current value of QCTL flags
     */
    inline long get_qctl_flags () {
      return qctl.flags;
    };

    /** Wait until a message is enqueued.
     *
     * @param wait if true Queue waits for a message
     */
    void set_message_wait ( bool wait );
    inline void setQWait ( bool wait ) {
      set_message_wait (wait);
    };

    /** Check if QWait flag is set
     *
     * @return true if wait flag is set
     */
    inline bool isQWaiting () {
      return is_message_waiting ();
    };

    inline bool is_message_waiting () {
      return ((qctl.flags & TPQWAIT) == TPQWAIT);
    };

    /** Set the desired persistence mode (memory, disk, default)
     * Accepted values are :
     * TPQQOSDEFAULTPERSIST: uses the persistence mode defined with queue
     * TPQQOSPERSISTENT: message are written onto persistent storage
     * TPQQOSNONPERSISTENT: message are written into memory only (message are lost if the queue manager is interrupted)
     *
     * @param qos desired flags
     */
    void setQoS ( long qos);

    /** @return the diagnostic code returned by last call.
     */
    inline int get_diagno () {
      return diagno;
    };

protected:
    int handleDiagnostics ( int _tperrno, int _diagno, const char *msg, ... );

private:

    /** Utiliy method used to simplify the implementation of enqueue messages
     */
    int enqueue ( char *queue,  char *data, long len );

    /** Utiliy method used to simplify the implementation of dequeue messages
     */
    int dequeue ( char *queue,  char **data, long *len );

    char *qspace;
    char *queue;

    int diagno;

    TPQCTL qctl;

    void setQCTL();
  };

// ---------------------------------------------------------------------------------

/** Handles in/out operation on a queue
 *
 *
 */
  class QueueStream : public Tuxedo {
public:

    friend ostream& operator<<(ostream& out, QueueStream& qs);
    friend istream& operator>>(istream& in, QueueStream& qs);

    QueueStream ( Queue *q );
    QueueStream ( Queue *q, long bs );

    /** @return the number of messages handle by last IO operation
     */
    inline long getCount () {
      return count;
    };

    inline void setBuffSize ( long s ) {
      buff_size = s;
    };
    inline long getBuffSize () {
      return buff_size;
    };

    void encodeBase64( bool b);

protected:

private:
    /**
     * Number of handled messages during last in/out operation.
     */
    long count;
    long buff_size;

    long flags;

    Queue *queue;
  };

/** Global utility to stream out the content of a Queue
 *
 * @param out output stream
 * @param qs queue stream that will handle the reading of messages
 */
  extern ostream& operator<<(ostream& out, QueueStream& qs);

/** Global utility to stream in a Queue
 *
 * @param in input stream
 * @param qs queue stream that handles the writing of messages to
 */
  extern istream& operator>>(istream& in, QueueStream& qs);
}
#endif

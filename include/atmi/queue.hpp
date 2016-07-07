/* /Q (queuing) related classes.
 *
 */

#ifndef CPP_ATMI_QUEUE_HPP
#define CPP_ATMI_QUEUE_HPP

#include <typeinfo>
#include <atmi.h>
#include <fml32.h>
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

  class queue;

#if __cplusplus < 201103L
  typedef std::auto_ptr<atmi::queue>   queue_ptr; //!< @deprecated use unique_ptr instead
#else
  typedef std::unique_ptr<atmi::queue> queue_ptr; //<! helper type
#endif

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
      int enqueue ( buffer &data );

      /** dequeue a message
       *
       * @param data tuxedo FML32 buffer (allocated with tuxedo::allocate)
       */
      int dequeue ( buffer &data );

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
      int dequeueReply ( buffer &data );

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
      int enqueueReply ( buffer &data );

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
       * @param args message arguments
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
      explicit queue_stream ( queue *q ) ;

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
 
  /** @} */
}

#endif

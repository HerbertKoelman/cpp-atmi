/*
   $Id: queue.C 79 2007-08-18 17:30:26Z hkoelman $

   Tuxedo queue manipulation helper class.
 */
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>
#include <atmi.h>
#include <fml32.h>
#include <sys/timeb.h>

// #include <atmi/Concurrency.h>
#include <atmi/atmi++.hpp>

using namespace std;

namespace atmi {

  queue::queue ( const char *qspace, const char *queue, const char *reply ) {

    /* Too restrictive
       if ( qspace == NULL || queue == NULL ) {
            throw atmi_exception ( "Invalid values for qspace or queue parameter" );
       }
     */

    _qspace = const_cast<char *>(qspace);
    _queue = const_cast<char *>(queue);

    this->flags = TPNOFLAGS;
    _qctl.flags = TPNOFLAGS;

    set_message_wait ( true );
    set_quality_of_service( TPQQOSDEFAULTPERSIST );

    set_reply_queue ( reply );
  }

/* operations ------------------------------------------------*/

  int queue::enqueue (buffer *data ) {

    return enqueue ( (char *)data->get_buffer(), 0 );
  }

  int queue::dequeue (buffer *data ) {

    int rc = -1;
    long len = 0;
    FBFR32 *b = data->get_buffer();

    if ( (rc = dequeue ( (char **) &b, &len )) != -1 ) {

      data->set_buffer ( b );
    }

    return rc;
  }

  int queue::enqueue ( char *queue,  char *data, long len ){

    int rc = -1;

    if ( _qspace == NULL || queue == NULL ) {

      throw atmi_exception ( catgets ( catd, CATD_ATMI_SET, 19, "Enqueue failed, qspace and queue properties have not been set !!??") );
    }

    // check if we need to switch context
    switch_context ();

    rc = tpenqueue ( _qspace, queue, (TPQCTL *) &_qctl, data, len, flags );

    updateErrno ();

    if ( rc < 0 ) {
      if ( tperrno == TPEDIAGNOSTIC ) {
        handle_diagnostics ( tperrno, _qctl.diagnostic, "Enqueue in %s:%s failed.", _qspace, queue );
      } else {
        handleTperrno ( tperrno, "Enqueue on %s:%s failed.", _qspace, queue );
      }
    }

    return rc;
  }

  int queue::enqueue ( char *data, long len ){

    int rc = -1;

    rc = enqueue ( _queue, data, len );

    return rc;
  }

  int queue::dequeueReply (buffer *data ) {

    return dequeueReply ( (char **) data->get_buffer(), 0 );
  }

  int queue::dequeueReply ( char **data, long *len ) {

    int rc = -1;

    if ( _qctl.replyqueue == NULL ) {
      throw atmi_exception ( catgets ( catd, CATD_ATMI_SET, 40, "Denqueue reply failed, reply queue properties have not been set !!??") );
    }

    rc = dequeue ( _qctl.replyqueue, data, len );

    return rc;
  }

  int queue::enqueueReply (buffer *data ) {

    return enqueueReply ( (char *)data->get_buffer(), 0 );
  }

  int queue::enqueueReply ( char *data, long len ) {

    int rc = -1;

    if ( _qctl.replyqueue == NULL ) {
      throw atmi_exception ( catgets ( catd, CATD_ATMI_SET, 40, "Enqueue reply failed, reply queue properties have not been set !!??") );
    }

    rc = enqueue ( _qctl.replyqueue, data, len );

    return rc;
  }

  int queue::dequeue ( char *queue, char **data, long *len ) {

    int rc = -1;

    if ( _qspace == NULL || queue == NULL ) {

      throw atmi_exception ( catgets ( catd, CATD_ATMI_SET, 20, "Dequeue failed, qspace and queue properties have not been set !!??") );
    }

    // check if we need to switch context
    switch_context ();

    rc = tpdequeue ( _qspace, queue, (TPQCTL *) &_qctl, data, len, flags );

    updateErrno ();

    if ( rc < 0 ) {
      if ( tperrno == TPEDIAGNOSTIC ) {
        if ( _qctl.diagnostic == QMENOMSG ) {
          rc = QMENOMSG;
        } else {
          handle_diagnostics ( tperrno, _qctl.diagnostic, "Dequeue from %s:%s failed.", _qspace, queue );
        }
      } else {
        handleTperrno ( tperrno, "Dequeue from %s:%s failed.", _qspace, queue );
      }
    }

    return rc;
  }

  int queue::dequeue ( char **data, long *len ) {

    int rc = -1;

    rc = dequeue ( _queue, data, len );

    return rc;
  }

  int queue::handle_diagnostics ( int tux_tperrno, int tux_diagno, const char *msg, ... ) {

    _diagno = tux_diagno;

    va_list ap;
    va_start ( ap, msg );

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
        diagnostic_exception err ( tux_tperrno, _diagno );
        err.setup_message ( msg, ap );
        throw err;
      }
      break;
      case QMEABORTED:
      {
        aborted_exception err ( tux_tperrno, _diagno );
        err.setup_message (msg, ap );
        throw err;
      }
      break;
      /* these are not errors */
      case QMENOMSG:
        // not an error - throw nomsg_exception ( _tperrno, _diagno, msg, ap );
        break;
      default:
        throw diagnostic_exception ( tux_tperrno, _diagno, catgets ( catd, CATD_ATMI_SET, 33, "Never heard about this diagno %d (tperrno: %d) !!??"), _diagno, tux_tperrno);
    }

    va_end ( ap );

    return _diagno;
  }

/* properties ------------------------------------------------*/


  void queue::set_reply_queue( const char *reply ) {
    if ( reply != NULL ) {
      _qctl.flags = set ( _qctl.flags, TPQREPLYQ );
      strncpy(_qctl.replyqueue, reply, TMQNAMELEN);
    } else {
      _qctl.flags = unset ( _qctl.flags,  TPQREPLYQ );
    }
  }

  const char *queue::reply_queue() {

    return _qctl.replyqueue;
  }

  void queue::set_message_wait ( bool qwait ) {

    if ( qwait ) {
      _qctl.flags = set ( _qctl.flags, TPQWAIT);                    // Set QWait bits
    } else {
      _qctl.flags = unset ( _qctl.flags, TPQWAIT );                    // Unset QWait bits
    }
  }

  void queue::set_quality_of_service ( long qos ) {

    _qctl.flags = unset( _qctl.flags, TPQDELIVERYQOS | TPQREPLYQOS );


    switch ( qos ) {

      case TPQQOSDEFAULTPERSIST:
        _qctl.flags        = set ( _qctl.flags, TPQDELIVERYQOS | TPQREPLYQOS);
        _qctl.delivery_qos = TPQQOSDEFAULTPERSIST;
        _qctl.reply_qos    = TPQQOSDEFAULTPERSIST;
        break;

      case TPQQOSPERSISTENT:
        _qctl.flags        = set ( _qctl.flags, TPQDELIVERYQOS | TPQREPLYQOS);
        _qctl.delivery_qos = TPQQOSPERSISTENT;
        _qctl.reply_qos    = TPQQOSPERSISTENT;
        break;

      case TPQQOSNONPERSISTENT:
        _qctl.flags        = set ( _qctl.flags, TPQDELIVERYQOS | TPQREPLYQOS );
        _qctl.delivery_qos = TPQQOSNONPERSISTENT;
        _qctl.reply_qos    = TPQQOSNONPERSISTENT;
        break;

      default:
        throw atmi_exception (catgets ( catd, CATD_ATMI_SET, 23, "Unsupported QoS (%d) for /Q operations."), qos);
    }
  }

  void queue::set_new_corrid() {
    // set correlation ID
    ostringstream corrid;
    timeb tb;
    ftime(&tb);
    corrid << getpid() << pthread_self() << (tb.millitm + (tb.time & 0xfffff) * 1000);

    memset ( _qctl.corrid, 0, TMCORRIDLEN);
    strncpy ( _qctl.corrid, corrid.str().c_str(), (corrid.str().length() > TMCORRIDLEN ? TMCORRIDLEN : corrid.str().length()));
    set ( _qctl.flags, TPQCORRID | TPQGETBYCORRID);
  }

  void queue::unset_corrid() {
    unset  ( _qctl.flags, TPQCORRID | TPQGETBYCORRID);
    memset ( _qctl.corrid, 0, TMCORRIDLEN);
  }


} // namespace-end

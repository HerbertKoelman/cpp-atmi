/*
   $Id: Queue.C 79 2007-08-18 17:30:26Z hkoelman $

   Tuxedo queue manipulation helper class.
 */
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <atmi.h>
#include <fml32.h>
#include <atmi/atmi++.hpp>

using namespace std;

namespace atmi {

  QueueStream::QueueStream ( Queue *queue ) {
    QueueStream ( queue, 1024 );
  }

  QueueStream::QueueStream ( Queue *queue, long bs ) {

    buff_size = bs;
    encodeBase64( false );
    if ( queue == NULL ) {
      throw atmi_exception ( "QueueStream needs a queue to by initialized. Passed queue was NULL." );
    }

    this->queue = queue;

    count = 0;
  }

  void QueueStream::encodeBase64( bool b ){

    flags = b ? TPNOFLAGS : TPEX_STRING;
  };

/* operations ------------------------------------------------*/


/* properties ------------------------------------------------*/


/* operators -------------------------------------------------*/

  ostream& operator<<(ostream& out, QueueStream& qs){

    int rc = -1;
    char *message = NULL;
    long len = qs.buff_size,size = qs.buff_size, s = 0;
    char *buffer = new char[size];

    try{
      qs.count = 0;

      // the use of const_cast avoids the warnings on GCC/Linux
      message = qs.allocate ( const_cast<char *>("CARRAY"), NULL, len );
      // message = qs.allocate ( "CARRAY", NULL, len );

      while ( rc !=  QMENOMSG ) {

        try {

          qs.queue->begin ();
          rc = qs.queue->dequeue ( &message, &len );

          // Make sure we have data
          if ( rc != QMENOMSG && len > 0 ) {

            s = size;

            rc = tpexport ( message, len, buffer, &s, qs.flags);

            if ( rc < 0 ) {

              // Give it chance to be corrected
              if ( tperrno == TPELIMIT ) {
                s += s;

                delete buffer;
                buffer = new char[s];
                memset ( buffer, 0, s);
                rc = tpexport ( message, len, buffer, &s, qs.flags );
              }

              // If we still don't succeed to export buffer we throw an exception and exit the program.
              if ( rc < 0 ) {
                throw tuxedo_exception ( tperrno, "TPEXPORT failed. Check buffer size (last buffer size was %d bytes).", s );
              }
            }

            out.write ( buffer, s ) << endl;
            qs.count++;

          }
          qs.queue->commit ();

        } catch ( timeout_exception &timeout) {
          qs.queue->abort();
        }
      }

      qs.free ( message );
      delete  buffer;

      /* REMOVE - Handled in one and unique exception handler
         }catch ( diagnostic_exception &diagErr ) {
         qs.queue->abort();
         qs.free ( message );
         delete  buffer;
         throw;
         }catch ( tuxedo_exception &tuxErr ) {
         qs.queue->abort();
         qs.free ( message );
         delete buffer;
         throw;
         REMOVE */

    } catch (... ) {
      qs.queue->abort();
      qs.free ( message );
      delete buffer;
      throw;
    }

    return out;
  }

  istream& operator>>(istream& in, QueueStream& qs){

    int rc = -1;
    long len = qs.getBuffSize ();
    string line;
    char *message = NULL;

    try {
      qs.count = 0;

      message = (char *)qs.allocate (const_cast<char *>("CARRAY"), NULL, len );

      while (getline ( in, line)) {

        rc = tpimport ( (char *)line.c_str(), line.size(), (char **) &message, &len, qs.flags );

        if ( rc < 0 ) {
          throw tuxedo_exception ( tperrno, "TPIMPORT failed. Was this message exported using tpexport ?" );
        }

        try{ // to en dequeue message.

          qs.queue->begin();
          rc = qs.queue->enqueue ( (char *)message, len );
          qs.queue->commit();

          qs.count++;

        } catch ( diagnostic_exception &diagErr ) {
          qs.queue->abort ();
          throw;
        } catch (tuxedo_exception &tuxErr ) {
          qs.queue->abort ();
          throw;
        }

      }                   // while-end

      qs.free ( message );

    } catch (tuxedo_exception &terr ) {
      qs.free ( message );
      throw;
    }             // try-end

    return in;
  }

} // namespace-end

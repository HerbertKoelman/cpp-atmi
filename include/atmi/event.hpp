/*
 * Tuxedo event handfling helpers.
 *
 */

#ifndef CPP_ATMI_EVENT_HPP
#define CPP_ATMI_EVENT_HPP

#include <typeinfo>
#include <atmi.h>
#include <fml32.h>
#include <string>
#include <cstdio>
#include <memory>
#include <nl_types.h>

#include <atmi/definitions.hpp>
#include <atmi/tuxedo.hpp>

//using namespace std;

namespace atmi {

/** \addtogroup atmi
 *
 * @{
 */

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
      explicit event( const char *evtname = NULL);

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

  /** @} */
} // namespace atmi

#endif

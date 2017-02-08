/*
 * author: herbert koelman(herbert.koelman@me.com)
 * creation date:  dim. août  7 14:12:00 CEST 2016
 */

#include <atmi/call_info.hpp>
#include "atmi/buffer.hpp"

namespace atmi {

  call_info &call_info::operator=(atmi::call_info &callinfo) {
    _buffer = (FBFR32 *) callinfo;

    return *this;
  }

  call_info::operator FBFR32 *() {
    return _buffer.get_buffer();
  }

  void call_info::print() const {
    _buffer.print();
  }

  const std::string call_info::ecid() {
    _buffer.get(_ecid);
    return _ecid;
  }

  void call_info::set_ecid( const char *value){
    // if (value.size() <= MAXECIDLEN ){
    if (strlen(value) <= MAXECIDLEN  ){
      _ecid = value ;
      _buffer.set(_ecid);
    } else {
      throw atmi_exception("ECID  %s is too long (max ECID len is : %d)", value, MAXECIDLEN);
    }
  }

  call_info::call_info(): _buffer(2048), _ecid("TA_ECID"){
  };
}

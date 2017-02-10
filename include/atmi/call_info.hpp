/*
 * author: herbert koelman(herbert.koelman@me.com)
 * creation date:  dim. août  7 14:12:00 CEST 2016
 */

#include <string>
#include <cstring>
#include <tpadm.h>

#include <atmi/definitions.hpp>
#include <atmi/string_field.hpp>

#ifndef CPP_ATMI_CALL_INFO_HPP
#define CPP_ATMI_CALL_INFO_HPP

namespace atmi {

/** \addtogroup fml
 *
 * @{
 */

//----------------------------------------------------------------------------------------------

  /**
   * Specialized atmi::buffer that handles CALL DATA INFOs.
   *
   * @author herbert koelman(herbert.koelman@me.com)
   * @since v4.3.0
   * @example buffer_test.bcl
   */
  class call_info {
    public:

      /** give private access to this trusted friend :-)
       *
       * @param callinfo Tuxedo call metadata
       */
      friend void buffer::get_call_info( call_info &callinfo);

      /** @return current ECID value */
      const std::string ecid() ;

      /** update/set ECID.
       *
       * @param value ecid
       * @see atmi::buffer::set_cal_info
       */
      //void set_ecid( const std::string &value);
      void set_ecid( const char *value);

      /** print call infos */
      void print() const;

      /** copy (Fcopy32) a call info buffer
       *
       * @param callinfo call info content to copy.
       * @return call_info reference
       */
      call_info &operator=( call_info &callinfo );

      /** @return  a FBFR32 * buffer that contains the call infos */
      explicit operator FBFR32 *() ;

      /** create a FBFR32 buffer to hold call infos
       */
      call_info();

    private:

      buffer              _buffer;
      Tfield<std::string> _ecid;
  };

  /** @} */
}
#endif

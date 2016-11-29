/*
 * ATMI++
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



#ifndef CPP_ATMI_PLUS_PLUS_HPP
#define CPP_ATMI_PLUS_PLUS_HPP

#include <atmi/ulog.hpp>
#include <atmi/exceptions.hpp>

#include <atmi/buffer.hpp>
#include <atmi/fields.hpp>
#include <atmi/call_info.hpp>
#include <atmi/carray_field.hpp>

#include <atmi/tuxedo.hpp>
#include <atmi/transaction.hpp>
#include <atmi/queue.hpp>
#include <atmi/event.hpp>
#include <atmi/abstract_client.hpp>

//! Application to Transaction Monitor Interface
namespace atmi {

  /** \addtogroup atmi
   *  \addtogroup fml
   * @{
   */

  /** @return the library's current version */
  extern "C" const char *cpp_atmi_version();

  /** @} */

}
#endif

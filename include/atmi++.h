/*
  $Id$

  Helper class for dealing with tuxedo.
*/

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

/** @mainpage C++ implementation for ATMI v2.1 - Tuxedo 9.1
 *
 * @author herbert koelman, eric belloni
 *
 * @section what_it_does What it does
 *
 * simple C++ wrappers for the ATMI C library, making it easier to write fast and rock solid Tuxedo software. The targeted \
 * implementation should be thread safe and offer multi-context capabilities.
 *
 * @section what_it_implements What it implements
 *
 * The ATMI++ comes as the following list of libraries:
 *  - atmi++.a : set of classes to handle ATMI calls in C++ programs
 *     - atmi::Tuxedo class : base class offers access to common function (like memory allocation).
 *     - atmi::Tp class: extends Tuxedo class with calls dedicated to transaction processing
 *     - atmi::Queue class : extends Tuxedo class with calls dedicated to queuing operations.
 *
 *  - fml++.a : set of classes to handle Fielded Buffers (FML) calls in C++ programs
 *     - atmi::Buffer class : Handle field buffer memory.
 *     - atmi::Field class: base class to manipulate Fields. This is an virtual pure class.
 *     - atmi::template < >TField class templates : 
 *
 *  - atmiutl++.a : set of helper classes to handle concurrency and multithreading
 *     - Thread class: wrapper of the POSIX Threading library
 *     - Mutex class: wrapper to POSIX Mutex handling functions.
 *     - Condition class: wrapper to POSIX Condition handling functions.
 *
 * The library comes with sample code that are gathered into the bin directory :
 * - btest.C: FML buffer sample
 * - qtest.C: /Q access demo
 * - tptest.C: transactional calls samples
 * - mctest.C: multicontext sample code
 */

#ifndef _ATMI_PLUS_PLUS_H_
#define _ATMI_PLUS_PLUS_H_

/* #define ATMI_VERSION "v3.0 Aix 5.3TL12 SMP O5 Tuxedo 9.1 32bits" */
#include <config.h>
#include <Exceptions.h>
#include <Tuxedo.h>
#include <Buffers.h>

#endif


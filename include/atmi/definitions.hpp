/*
 * globel definitions.
 */

#include <fml32.h>

#ifndef CPP_ATMI_DEFINITIONS_HPP
#define CPP_ATMI_DEFINITIONS_HPP

/** catalog message set */
#define CATD_ATMI_SET 100

namespace atmi {

  /** FML field possible types
   */
  enum field_types {
   fld_short   =FLD_SHORT,    //!< 0       short in
   fld_long    =FLD_LONG,     //!< 1       long in
   fld_char    =FLD_CHAR,     //!< 2       character
   fld_float   =FLD_FLOAT,    //!< 3       single-precision floa
   fld_double  =FLD_DOUBLE,   //!< 4       double-precision floa
   fld_string  =FLD_STRING,   //!< 5       std::string - null terminated
   fld_carray  =FLD_CARRAY,   //!< 6       character array
   fld_ptr     =FLD_PTR,      //!< 9       pointer to a buffer
   fld_buffer_ptr=FLD_FML32,  //!< 10       embedded FML32 buffer
   fld_view      =FLD_VIEW32, //!< 11       embedded VIEW32 buffer
   fld_mbstring  =FLD_MBSTRING  //!< 12       multibyte character array
  };

  /** alias
   */
  typedef field_types field_type;

} // namespace atmi
#endif

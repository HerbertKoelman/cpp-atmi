//
//  cpp_atmi.hpp
//  cpp_atmi
//
//  Created by herbert koelman on 18/03/2016.
//
//

#ifndef atmi_atmi_hpp
#define atmi_atmi_hpp

#include <atmi/atmi++.hpp>
#include "config.h"

namespace atmi {

  extern "C" const char *cpp_atmi_version(){
#ifndef CPP_ATMI_VERSION
    return "missing CPP_ATMI_VERSION define. Re-run configure" ;
#else
    return CPP_ATMI_VERSION ;
#endif
  }

}
#endif /* atmi_atmi_hpp */

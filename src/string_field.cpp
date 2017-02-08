/* author: herbert koelman (herbert.koelman@me.com)
 */

#include <iostream>

#include <atmi.h>
#include <fml.h>
#include <memory>
#include <string>
#include <sstream>

#include <atmi/string_field.hpp>

namespace atmi {

// operators --------------------------------------------------------------------------------

  std::ostream &operator<< ( std::ostream &out, Tfield<std::string> &f ){

    return out << (std::string)f;
  };

}

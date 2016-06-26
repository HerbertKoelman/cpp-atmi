#include <stdarg.h>
#include <string>
#include <atmi/ulog.hpp>

namespace atmi {


  ulog::ulog(log_level level):_level(level){
    setup_level_names();
  }

  /** Set a new logging level
   *  @param level level from which the ulog starts to write messages
   */
  void ulog::set_log_level ( log_levels level ){
    _level = level;
  }

  /** Setup the logging level name array
   * */
  void ulog::setup_level_names (){

    LEVELS[0]="DEBUG";
    LEVELS[1]="FINER";
    LEVELS[2]="INFO";
    LEVELS[3]="WARNING";
    LEVELS[4]="ERROR";
  }
}

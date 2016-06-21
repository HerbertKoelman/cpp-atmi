#include <stdarg.h>
#include <string>
#include <atmi/logger.hpp>

namespace atmi {


  /** @return current logging level
   */
  int logger::get_log_level () {
    return level;
  }

  /** Set a new logging level
   *  @param level level from which the logger starts to write messages
   */
  void logger::set_log_level ( LoggingLevel level ){
    this->level = level;
  }

  /** Set a new logging level
   *  @param level level from which the logger starts to write messages
   */
  void logger::set_log_level ( int level ){
    this->level = level;
  }

  /** Setup the logging level name array
   * */
  void logger::setup_level_names (){

    LEVELS[0]="DEBUG";
    LEVELS[1]="FINER";
    LEVELS[2]="INFO";
    LEVELS[3]="WARNING";
    LEVELS[4]="ERROR";
  }

  /** Write an error message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void logger::error ( const char *msg, ...){

    va_list args;
    va_start (args, msg);
    log ( ERROR, msg, args);
    va_end (args);
  }

  /** Write a warning message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void logger::warning ( const char *msg, ...){
    va_list args;
    va_start (args, msg);
    log (WARNING,  msg, args);
    va_end (args);
  }

  /** Write an info message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void logger::info ( const char *msg, ...){
    va_list args;
    va_start (args, msg);
    log ( INFO, msg, args);
    va_end (args);
  }

  /** Write a finer message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void logger::finer ( const char *msg, ...){
    va_list args;
    va_start (args, msg);
    log ( FINER, msg, args);
    va_end (args);
  }

  /** Write a debug message
   *
   * @param indent indentation level
   * @param msg message string
   * @param ... insertion values
   */
  void logger::debug ( int indent, const char *msg, ...){
    va_list args;
    va_start (args, msg);
    log ( DEBUG, msg, args);
    va_end (args);
  }

}
#include <stdarg.h>
#include <string>
#include <Logger.h>

namespace atmi {


  /** @return current logging level
   */
  int Logger::get_log_level () {
    return level;
  }

  /** Set a new logging level
   */
  void Logger::set_log_level ( LoggingLevel level ){
    this->level = level;
  }

  /** Setup the logging level name array
   * */
  void Logger::setup_level_names (){

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
  void Logger::error ( const char *msg, ...){

    va_list args;
    va_start (args, msg);
    print ( ERROR, msg, args);
    va_end (args);
  }

  /** Write a warning message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void Logger::warning ( const char *msg, ...){
    va_list args;
    va_start (args, msg);
    print (WARNING,  msg, args);
    va_end (args);
  }

  /** Write an info message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void Logger::info ( const char *msg, ...){
    va_list args;
    va_start (args, msg);
    print ( INFO, msg, args);
    va_end (args);
  }

  /** Write a finer message
   *
   * @param msg message string
   * @param ... insertion values
   */
  void Logger::finer ( const char *msg, ...){
    va_list args;
    va_start (args, msg);
    print ( FINER, msg, args);
    va_end (args);
  }

  /** Write a debug message
   *
   * @param indent indentation level
   * @param msg message string
   * @param ... insertion values
   */
  void Logger::debug ( int indent, const char *msg, ...){
    va_list args;
    va_start (args, msg);
    print ( DEBUG, msg, args);
    va_end (args);
  }

}

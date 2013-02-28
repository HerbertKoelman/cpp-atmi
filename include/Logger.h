/*
 * Logger
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
#include <stdarg.h>
#include <string>

#ifndef _LOGGER_
#define _LOGGER_

using namespace std;

namespace atmi {

  enum LoggingLevel {
    ERROR=4,
    WARNING=3,
    INFO=2,
    FINER=1,
    DEBUG=0
  };

  /** Interface og the logging facility 
   *
   * Write messages that matches the current level value and above.
   */
  class Logger {
    public:
    /** Write an error message
     *
     * @param msg message string
     * @param ... insertion values
     */
    void error ( const char *msg, ...);

    /** Write a warning message
     *
     * @param msg message string
     * @param ... insertion values
     */
    void warning ( const char *msg, ...);

    /** Write an info message
     *
     * @param msg message string
     * @param ... insertion values
     */
    void info ( const char *msg, ...);

    /** Write a finer message
     *
     * @param msg message string
     * @param ... insertion values
     */
    void finer ( const char *msg, ...);

    /** Write a debug message
     *
     * @param indent indentation level
     * @param msg message string
     * @param ... insertion values
     */
    void debug ( int indent, const char *msg, ...);

    void set_log_level ( LoggingLevel level );

    int get_log_level () ;

    protected:

    /** Overide this method to implement output handling.
     *
     * @param at the level to which this message correspond's
     * @param msg message to log
     * @param args substitution arguments of the message.
     */
    virtual void print ( LoggingLevel at, const char *msg, va_list args){};


    string LEVELS[5];

    void setup_level_names ();

    int level;
  };

  /**
   * This class uses stdout to log messages.
   *
   * @see Logger
   */
  class StandardLogger : public Logger {
    public:

      /** Create a logger that pprint's message form level up to highest level defined.
       * @param level lowest level 
       * @param id an identification char array (i.e. your program name)
       */
      StandardLogger ( const char *id = NULL, LoggingLevel level = INFO );

    protected:
      
      /** writes message on stdout.
       * example output: 2013-01-23T01:53:07-LOG.26828.3926517568-ERROR: Hello herbert. What's up...
       */
      virtual void print ( LoggingLevel at, const char *msg, va_list args);

    private:
      const char *id;
      pid_t pid;
  };

  /** This logger writes messages in the user log (ULOG).
   *
   * @see Logger
   */
  class ULOGLogger : public Logger {
    public:
      /** Create a logger that print's message form level up to highest level defined.
       * @param level lowest level 
       */
      ULOGLogger ( LoggingLevel level = INFO );

    protected:
      
      virtual void print ( LoggingLevel at, const char *msg, va_list args);
  };

}
#endif

#include <stdarg.h>
#include <memory>
#include <string>

#ifndef _LOGGER_
#define _LOGGER_

using namespace std;

namespace atmi {

  class logger;

#if __cplusplus < 201103L
  typedef auto_ptr<logger> Alogger ;
#else
  typedef unique_ptr<logger> Alogger ;
#endif

  enum LoggingLevel {
    ERROR=4,
    WARNING=3,
    INFO=2,
    FINER=1,
    DEBUG=0
  };

  /** Interface og the logging facility 
   * 
   * Verbose is the DEBUG log level (level value 0) and least verbose is ERROR (level value 4).
   * Write messages that matches the current level value and above.
   */
  class logger {
    public:

      ~virtual ~log() {};

      /** Write an error message
       *
       * @param msg message string
       * @param ... insertion values
       */
      virtual void error ( const char *msg, ...);

      /** Write a warning message
       *
       * @param msg message string
       * @param ... insertion values
       */
      virtual void warning ( const char *msg, ...);

      /** Write an info message
       *
       * @param msg message string
       * @param ... insertion values
       */
      virtual void info ( const char *msg, ...);

      /** Write a finer message
       *
       * @param msg message string
       * @param ... insertion values
       */
      virtual void finer ( const char *msg, ...);

      /** Write a debug message
       *
       * @param indent indentation level
       * @param msg message string
       * @param ... insertion values
       */
      virtual void debug ( int indent, const char *msg, ...);

      void set_log_level ( LoggingLevel level );

      void set_log_level ( int level );

      int get_log_level () ;

    protected:

      /** Overide this method to implement output handling.
       *
       * @param at the level to which this message correspond's
       * @param msg message to log
       * @param args substitution arguments of the message.
       */
      virtual void log ( LoggingLevel at, const char *msg, va_list args) {};


      string LEVELS[5]; //!< logging level names

      /** initialize level name array */
      void setup_level_names ();

      int level; //!< current logging level
  };

  /**
   * This class uses stdout to log messages.
   *
   * @see logger
   */
  class standard_logger : public logger {
    public:

      /** Create a logger that pprint's message form level up to highest level defined.
       * @param level lowest level 
       * @param id an identification char array (i.e. your program name)
       */
      standard_logger ( const char *id = NULL, LoggingLevel level = INFO );

      virtual ~standard_logger() {};

    protected:
      
      /** writes message on stdout.
       * example output: 2013-01-23T01:53:07-LOG.26828.3926517568-ERROR: Hello herbert. What's up...
       */
      void log ( LoggingLevel at, const char *msg, va_list args);

    private:
      const char *id;
      pid_t pid;
  };

  /** This logger writes messages in the user log (ulog_).
   *
   * @see logger
   */
  class ulog_logger : public logger {
    public:
      /** Create a logger that print's message form level up to highest level defined.
       * @param level lowest level 
       */
      ulog_logger ( LoggingLevel level = INFO );

      virtual ~ulog_logger() {};

    protected:

      void log ( LoggingLevel at, const char *msg, va_list args);

    private:
      
      char buffer[1024];
  };

}
#endif

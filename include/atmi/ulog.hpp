/* user access handler
 *
 * author: herbert koelman (herbert.koelman@me.com)
 * creation date: dim. juin 26 17:40:27 CEST 2016
 */

#include <userlog.h>
#include <memory>
#include <string>

#ifndef CPP_ATMI_ULOG_HPP
#define CPP_ATMI_ULOG_HPP

namespace atmi {

  /** \adtogroup logging ULOG writer
   *
   * ULOG helpers
   *
   * @{
   */
  class ulog;

#if __cplusplus < 201103L
  typedef std::auto_ptr<ulog>   ulog_ptr ;
#else
  typedef std::unique_ptr<ulog> ulog_ptr ;
#endif

  enum log_levels {
    error=4,
    warning=3,
    info=2,
    finer=1,
    debug=0
  };

  typedef log_levels log_level;

  /** Interface og the logging facility 
   * 
   * Verbose is the DEBUG log level (level value 0) and least verbose is ERROR (level value 4).
   * Write messages that matches the current level value and above.
   */
  class ulog {
    public:

      /** create a ULOG instance.
       *
       * Default is log_atmi::levels::info
       * 
       * @param level wanted log level
       */
      explicit ulog( log_level level = log_level::info);

      virtual ~ulog() {};

      /** error message
       * 
       * @param fmt  message format( see stdd:pritnf())
       * @param args format arguments 
       */
      template<typename... Args> void error( const std::string &fmt, const Args&... args){
          log(log_levels::error, fmt, args...);
      };

      /** warning message
       * 
       * @param fmt  message format( see stdd:pritnf())
       * @param args format arguments 
       */
      template<typename... Args> void warning( const std::string &fmt, const Args&... args){
          log(log_levels::warning, fmt, args...);
      };

      /** info message
       * 
       * @param fmt  message format( see stdd:pritnf())
       * @param args format arguments 
       */
      template<typename... Args> void info( const std::string &fmt, const Args&... args){
          log(log_levels::info, fmt, args...);
      };

      /** finer message
       * 
       * @param fmt  message format( see stdd:pritnf())
       * @param args format arguments 
       */
      template<typename... Args> void finer( const std::string &fmt, const Args&... args){
          log(log_levels::finer, fmt, args...);
      };

      /** debug message
       * 
       * @param fmt  message format( see stdd:pritnf())
       * @param args format arguments 
       */
      template<typename... Args> void debug( const std::string &fmt, const Args&... args){
          log(log_levels::debug, fmt, args...);
      };

      void set_log_level ( log_levels level );

      /** @return current log level */
      int level () const {
        return _level;
      }

      /** Actual log writer
       *
       * @param at the level to which this message correspond's
       * @param fmt message to log
       * @param args substitution arguments of the message.
       */
      template<typename... Args> void log ( log_levels at, const std::string &fmt, const Args&... args){

        if ( _level <= at) {

            std::string format = LEVELS[at]+ ": " + fmt;
            userlog (const_cast<char *>(format.c_str()), args... );
        }
      }

    private:
      std::string LEVELS[5]; //!< logging level names

      /** initialize level name array */
      void setup_level_names ();

      log_level _level; //!< current logging level
  };

  /** @} */
} // namespace atmi
#endif

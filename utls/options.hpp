//
//  Created by herbert koelman on Wed Oct 12 12:39:08 CEST 2016
//

#include <cstdlib>
#include <libgen.h>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <atmi/atmi++.hpp>

#ifndef UTLS_OPTIONS_HPP
#define UTLS_OPTIONS_HPP


void version(){
  std::cout << "comes with: " << atmi::cpp_atmi_version() << std::endl << std::endl;
}

/** parses command line
 */
class options{
  public:

    /** create an instance and parse the given argument lis
     *
     */
    options():
      user(NULL),
      sys_passwd(NULL),
      app_passwd(NULL),
      group(NULL),
      buffer_size(1024),
      failed(false),
      _arguments("u:p:s:g:b:v"),
      _arguments_usage("[-b buffer size] [-v] [-u user] [-p password] [-s system password] [-g group]")
    {
      // intentional
    }

    virtual ~options(){
      // intentional
    }

    void usage( const char *pname, const char *details = NULL){
      std::cout << "usage : " << pname << " " << _arguments_usage << std::endl ;
      if ( details != NULL ){
        std::cout << std::endl ;
        std::cout << details << std::endl ;
      }
      options_usage();
    }

    /**
     * @param argc argument counter
     * @param argv argument array
     */
    void parse(int argc, char *argv[]){

      int                 opt = 0;
      extern char        *optarg;
      extern int          optind, optopt, opterr;// used by command line parser

#     ifdef DEBUG
      printf("DEBUG %s: parsing command line for [%s] argument.\n", __FUNCTION__, _arguments.c_str());
#     endif

      while ((opt = getopt(argc, argv, _arguments.c_str())) != -1) { // NOSONAR
        set(opt);
      }
    }

    bool       failed;
    char      *user;
    char      *sys_passwd;
    char      *app_passwd;
    char      *group;
    size_t     buffer_size;

  protected:
    std::string _arguments;
    std::string _arguments_usage;

    virtual void options_usage(){

      std::cout << "-u Tuxedo user"  << std::endl;
      std::cout << "-p Tuxedo user's application password"  << std::endl;
      std::cout << "-s Tuxedo system (DOMAIN) password"  << std::endl;
      std::cout << "-g group to use"  << std::endl;
      std::cout << "-b buffer size (in bytes defaults to 1024)"  << std::endl;
      std::cout << "-v print version info and exit"  << std::endl;
    }

    /** handles common command line options.
     *
     * @param opt command line option
     */
    virtual void set( int opt){
      switch(opt) {
        case 'v':
          version();
          failed = true ; // this will let programs a chance to interrupt execution
          break;
        case 'u': // user
          user = optarg;
          break;
        case 's': // system passwd
          sys_passwd = optarg;
          break;
        case 'p': // passwd
          app_passwd = optarg;
          break;
        case 'g': // group
          group = optarg;
          break;
        case 'b': // buffer size
          buffer_size = atoi(optarg);
          break;
        case ':':       /* -f or -o without operand */
          fprintf(stderr, "option -%c requires an operand\n. ", optopt);
          failed = true;
          break;
        case '?':
          fprintf(stderr, "unrecognized option -%c.\n", optopt);
          failed = true;
          break;
        default:
          // intentional
          break;
      }
    }
};

class queue_options: public options{
  public:
    queue_options(): queue(NULL), qspace(NULL){
      _arguments = ":q:" + _arguments ;
      _arguments_usage = "-q <qspace:queue> " + _arguments_usage;
    }

    virtual ~queue_options(){
      // intentional
    }

    char      *queue;
    char      *qspace;

  protected:

    virtual void options_usage(){
      std::cout << "-q queue and queue space" << std::endl;
      options::options_usage();
    }

    void set( int opt){
      switch(opt) {
        case 'q':
          qspace = strtok(optarg, ":");
          queue  = strtok(NULL, ":");
          break;
        default:
          options::set(opt);
      }
    }
};


class export_options: public queue_options{
  public:
    export_options(): daemon(false), output_file(NULL){
      _arguments += "o:d" ;
      _arguments_usage += " [-d] [-o output file]" ;
    }

    virtual ~export_options(){
      // intentional
    }

    bool  daemon ;
    char *output_file ;

  protected:

    virtual void options_usage(){
      options::options_usage();
      std::cout << "-o dump messages in this file" << std::endl;
      std::cout << "-d start program in daemon mode" << std::endl;
    }

    void set( int opt){
      switch(opt) {
        case 'd':
          daemon = true;
          break;
        case 'o':
          output_file = optarg;
          break;
        default:
          queue_options::set(opt);
      }
    }
};

class import_options: public queue_options{
  public:
    import_options(){
      _arguments_usage += " file..." ;
    }

    virtual ~import_options(){
      // intentional
    }

    bool daemon ;

  protected:

    virtual void options_usage(){
      options::options_usage();
    }

};
#endif

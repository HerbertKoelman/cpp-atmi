//
//  Created by herbert koelman on Wed Oct 12 12:39:08 CEST 2016
//

#include <cstdlib>
#include <libgen.h>
#include <iostream>
#include <unistd.h>
#include <csignal>

#ifndef UTLS_OPTIONS_HPP
#define UTLS_OPTIONS_HPP

void usage( const char *pname, const char *details = NULL){
  std::cout << "usage : " << pname << " -q <qspace:queue> [-u user] [-p password] [-g group]" << std::endl ;
  if ( details != NULL ){
    std::cout << std::endl ;
    std::cout << details << std::endl ;
  }
}

/** parses command line
 */
class options{
  public:

    options() = delete;

    /** create an instance and parse the given argument list
     *
     * @param argc argument counter
     * @param argv argument array
     */
    options(int argc, char *argv[]): 
      queue(NULL),
      qspace(NULL),
      user(NULL),
      passwd(NULL),
      group(NULL),
      failed(false)
  {
      int                 opt = 0;
      extern char        *optarg;
      extern int          optind, optopt, opterr;// used by command line parser
      
      while ((opt = getopt(argc, argv, ":q:u:p:g:v")) != -1) {
        switch(opt) {
          case 'v':
            failed= true; // this will cause program to exit
            break;
          case 'q':
            qspace = strtok(optarg, ":");
            queue  = strtok(NULL, ":");
            break;
          case 'u': // user
            user = optarg;
            break;
          case 'p': // passwd
            passwd = optarg;
            break;
          case 'g': // group
            group = optarg;
            break;
          case ':':       /* -f or -o without operand */
            fprintf(stderr, "option -%c requires an operand\n. ", optopt);
            failed = true;
            break;
          case '?':
            fprintf(stderr, "unrecognized option -%c.\n", optopt);
            failed = true;
        }
      }
    }

    bool       failed;
    char      *queue;
    char      *qspace;
    char      *user;
    char      *passwd;
    char      *group;
};

#endif

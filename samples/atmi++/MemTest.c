#include <stdlib.h>

void main () {

  char *buf ;

  while ( 1 ) {
    buf = malloc ( 4096 );
    free ( buf );
  }
}

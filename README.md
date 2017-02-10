## C++ ATMI wrapper

### What it does

ATMI, for Application-to-Transaction Monitor Interface, is the main API for the Tuxedo system. It includes transaction 
management functions (routines, verbs); request/response, conversational, queuing, and publish-and-subscribe message-handling
functions; service interface functions; and buffer management functions for distributed application communication.

ATMI++ is a C++ wrapping of this API,  adding strong typing of FML buffers and adding the use of exceptions to detect and handle error conditions. 

This should help making it easier to write fast and rock solid Tuxedo software.

This project is currently hosted <a href="http://herbertkoelman.github.com/cpp-atmi">here</a>.

### Setting things up

    $ configure
    $ make

Documentation is obtained with this command:

    $ make doxygen

The library is documented [here](http://herbertkoelman.github.io/cpp-atmi/doc/html/).

### Create an rpm

An RPM spec file is generated when configure is executed. You can then copy the tar.gz into the SOURCES directory and run `rpm -bb cpp-atmi.spec`.

The following commands prepare and builds an RPM for the release v4.6.1:

    $ ./configure --enable-release && make pkg
    $ cp distrib/cpp-atmi-v4.6.1.tar.gz ~/rpmbuild/SOURCES
    $ rpm -bb cpp-atmi.spec

 The makefile comes with a `rpm` target which will setup and produce a RPM in the dirctory `rpmbuild`.

    $ ./configure --enable-release && make rpm

### Useful links

- Tuxedo online documentation can be accessed [here](http://docs.oracle.com/cd/E35855_01/tuxedo/docs12c/index.html)
- ATMI C library is docuemented [here](http://docs.oracle.com/cd/E35855_01/tuxedo/docs12c/rf3c/rf3c.html)

###  How it's used

This code creates a FML field and passes it to a service called `XATOUPPER`. If anything goes wrong, we roll back the transaction and pass the exception (`throw`) so that the diagnostic messages can be displayed.

        ...
        transaction tp ( "XATOUPPER" );
        int urcode = 0;

        try {

          buffer buffer;

          Tfield<string> name ( "EMPNAME" );
          name = "Herbert.Koelman@me.com";

          buffer.add ( name );

          printf ("Calling service TOUPPER (%s), context: %d.\n", name.c_str() , name().c_str());

          try {

            tp.begin();

            int ret = tp.call ( buffer );
            switch (ret) {
              case 0:
                break;
              case -1:
                printf ("TOUPPER returned TPFAIL.\n" );
                break;
              default:
                printf ( "TOUPPER returned: %d (error: %d/%d).\n", ret, tp.error());
                buffer.print();
            }

            buffer.get ( name );
            printf ( "Call returned: %s.\n", name.c_str() );

            tp.commit();
          } catch ( ... ){
            tp.abort();
            throw;
          }

        } catch ( tuxedo_exception& err ) {
          printf ( "Tuxedo Exception catched. tpcall failed. %s\n", err.what() ); 
        };
        ...

### License

-------------------------------------------------------------------

 ATMI++ is free software, you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 ATMI++ is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with ATMI++; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor,
 Boston, MA  02110-1301  USA

-------------------------------------------------------------------
Copyright (C) 2016 - herbert koelman


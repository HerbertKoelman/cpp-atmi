Author: Herbert Koelman  
Date: 29/11/2012  
Current stable version: v2.1 (see branch v2.1)
 
Tuxedo ATMI C++ wrapper
============

Tuxedo (Transactions for Unix, Extended for Distributed Operations) is a middleware platform used to manage distributed transaction processing in distributed computing environments. Tuxedo is a transaction processing system or transaction-oriented middleware, or enterprise application server for a variety of systems and programming languages. Developed by AT&T in the 1980s, it became a software product of Oracle Corporation in 2008.

ATMI, for Application-to-Transaction Monitor Interface, is the main API for the Tuxedo system. It includes transaction management functions (routines, verbs); request/response, conversational, queuing, and publish-and-subscribe message-handling functions; service interface functions; and buffer management functions for distributed application communication.

ATMI++ is a C++ wrapping of this API, adding strong typing of FML buffers and adding the use of exceptions to detect and handle error conditions. This should help making it easier to write fast and rock solid Tuxedo software.

Project content
============

ATMI++ comes as :
* ```src``` directory which contains the source code. It is diveided into two subdirectories. One (```src/tuxedo```) whith the actual ATMI wrapper C++ code and a second with the usual utility classes.
* ```samples``` directory which contains a directory ```samples/atmi++``` with sample client and server code. A directory ```samples/utl``` with sample code to illustrate how the utility classes can be used. Finally an ```samples/app``` directory that contains a sample Tuxedo DOMAIN setup.
* ```include``` directory that contains everything you need to compile your code.
* Makefile, autoconf and automake files are included (and soon an RPM build file)

The building of the libraries is based upon automake and autoconf functionalities. So to setup ATMI++ you can follow the next steps:
```
$ configure
$ make
$ make demos
```

This should get you started. At least on ```Fedora FC17``` and AIX 5.3 or later. When build was successfully you should end up with a ```locale``` directory which contains the message catalogues (currently French and English). If needed you can build the usage documentation this way:
```
$ make doxygen
```

This creates an ```html``` and ```man``` directory.

It is also possible to deploy the library onto your system using de install/uninstall make targets.

Samples
===========

The samples directory contains a set of sample programs to illustrate how this is meant to be used. The directory samples/apphelps you setup a Tuxedo server (DOMAIN) to run the sample server implementations. To get things setup and running:
```
$ make demos
$ cd samples/app
$ . ./setup_app
$ . ./deploy_app
```

Should be running :-)

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
Copyright (C) 2006 - herbert koelman


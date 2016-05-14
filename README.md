@section what_it_does What it does

ATMI, for Application-to-Transaction Monitor Interface, is the main API for the Tuxedo system. It includes transaction 
management functions (routines, verbs); request/response, conversational, queuing, and publish-and-subscribe message-handling
functions; service interface functions; and buffer management functions for distributed application communication.

ATMI++ is a C++ wrapping of this API,  adding strong typing of FML buffers and adding the use of exceptions to detect and handle error conditions. 
This should help making it easier to write fast and rock solid Tuxedo software.

This project is currently hosted <a href="http://herbertkoelman.github.com/atmiplusplus">here</a>.

@section what_it_implements What it implements

The ATMI++ comes as the following list of libraries:
 - atmi++.a : set of classes to handle ATMI calls in C++ programs
    - atmi::Tuxedo class : base class offers access to common function (like memory allocation).
    - atmi::Tp class: extends Tuxedo class with calls dedicated to transaction processing
    - atmi::Queue class : extends Tuxedo class with calls dedicated to queuing operations.
    - atmi::Event class : extends Tuxedo class with calls dedicated to notifying and broadcasting operations.
    - atmi::AbstractClient class : utility class that helps writting Tuxedo client programs.
    - atmi::TuxedoException class : extends Tuxedo class with calls dedicated to notifying and broadcasting operations.
    - atmi::Logger class : implements a simple mechanism to write messages at a given logging level (see also atmi::StandardLogger and atmi::ULOGLogger).

 - fml++.a : set of classes to handle Fielded Buffers (FML) calls in C++ programs
    - atmi::Buffer class : Handle field buffer memory.
    - atmi::Field class: base class to manipulate Fields. This is an virtual pure class.
    - atmi::TField < > class templates : 

 - atmiutl++.a : set of helper classes to handle concurrency and multithreading
    - atmi::Thread class: wrapper of the POSIX Threading library
    - atmi::ThreadPool class: pre-allocated Threads
    - atmi::Pipe class: coordonated way to pass messages among Threads
    - atmi::Barrier class: handles a point of synchronization (rendez-vous) among Threads
    - atmi::Mutex class: wrapper to POSIX Mutex handling functions.
    - atmi::Condition class: wrapper to POSIX Condition handling functions.
    - atmi::YamlConfig: helps to parse YAML formatted conf file.

@section setting_things_up Setting things up

    $ configure
    $ make

The library comes with sample code that are gathered into the samples directory. You can build them using 

    $ make demos

This documentation is obtained :

    $ make doxygen

Suggestions or bug reporting can be done <a href="mailto:herbert.koelman@me.com">here</a>

@section depends_on Dependencies

The project depends on :
- libyaml available <a href="http://pyyaml.org/wiki/LibYAML">here</a> 



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


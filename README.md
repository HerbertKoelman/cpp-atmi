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

This documentation is obtained :

    $ make doxygen

Suggestions or bug reporting can be done <a href="mailto:herbert.koelman@me.com">here</a>

###  Dependencies

none

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


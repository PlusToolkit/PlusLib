=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: README.TXT,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  Text
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.4 $

==========================================================================

This package provides a portable C library that provides a straightforward
interface to AURORA and POLARIS systems manufactured by Northern Digital Inc.
This library is provided by Atamai Inc., and is not supported by Northern
Digital Inc.

The contents of this package have been built successfully under
Linux-gcc, Mac-OSX10.3-gcc, IRIX-cc, and Windows-MSVC.  A configure
script is supplied for use with Linux, OSX, and IRIX.  It is used
as follows:

  ./configure --prefix=/usr/local  (or another appropriate directory)
  make
  make install 

To build a Python (http://www.python.org) version of this library, do
  python setup.py build
  python setup.py install
  >>> import polaris

-----
The main contents of this package are as follows:

1) A C library (libndicapi.a) that provides a set of C functions for
   communicating with an NDI device via the NDI Combined API.  The
   documentation for this library is provided in the ndicapi_html
   directory.

2) Two C++ header files (polaris.h and polaris_math.h) that provide
   and interface, via libndicapi.a, to an NDI device via the  
   POLARIS Serial Communications API that predated the Combined API.
   Documentation is provided in the polaris_html directory.

3) The files CMakeLists.txt and vtkndicapi.def provide support for
   building the ndicapi library within the VTK directory tree.
   The vtkPOLARISTracker class supports the use of the POLARIS
   and AURORA systems within VTK.

4) A python interface to the ndicapi library.  However, only the original
   POLARIS API is supported through python.  The full ndicapi interface is
   not yet supported.

---
END OF FILE

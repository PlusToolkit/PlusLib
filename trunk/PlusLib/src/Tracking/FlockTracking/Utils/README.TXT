-------------------------------------------
This archive provides a C API for the Ascension Flock of Birds.

flock.h   - header file which describes the API
flock.c   - C file which can be compiled into a library to support the API

flocktest.c  - file for testing the flock of birds

flock_html/index.html  - main documentation page

-------------------------------------------
There is currently no text file which describes the API.  
Refer to the flock.h and to flocktest.c, and to the Flock of Birds 
specifications provided by Ascension at 
ftp://ftp.ascension-tech.com/flock_of_birds/fob_man21.wp

The file "flock.c" should compile and work under Linux, IRIX, and 
Windows NT.  Other operating systems have not been tested.

To compile flocktest.c into an executable, use the following:

gcc flocktest.c flock.c -lm -lpthread -o flocktest

The '-lpthread' is only necessary if your UNIX machine supports
POSIX threads -- almost every modern (1998 or later) UNIX does.
On some UNIXes, you should specify '-lpthreads' instead.

--------------------------------------------
This API provides the following features:

- a better way of using the flock than banging the serial port

- automatic recovery from phase errors (i.e. serial communication
  errors), usually only a single record is lost

- multithreaded streaming - the extra thread is there to catch
  the data records as they stream in.  This guards against 
  serial buffer overflows, reduces lag, and increases overall 
  performance.

- units are millimetres and radians - real scientists don't use
  inches and degrees

- a high- and low-level interface to the flock

- portability: Windows NT/2000/95/98/Millenium/XP
               Linux, IRIX
               porting to other UNIX variants is very straightforward

--------------------------------------------
This API does not (yet) support:

- extended-range transmitters (anyone want to lend me one?)

---------------------------------------------
Notes on the included VTK classes:

These classes should work under any version of VTK more recent
than 2.0, but have only been tested recently with VTK 4.0.

To simplify the building of these classes, you should also copy
flock.c and flock.h into the vtk/local directory.


-----------------
David Gobbi, MSc
Imaging Research Laboratories
The John P. Robarts Research Institute
dgobbi@irus.rri.on.ca


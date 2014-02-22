# Find the StealthLink2.0 Libraries
# This module defines
# Stealthlink2_INCLUDE_DIRS - StealthLink2 header files 
# Stealthlink2_StealthLink_STATIC_LIBRARY - StealthLink2 static library
# Stealthlink2_StealthLinkD_STATIC_LIBRARY - Stealthlink2 static library with debug info
# Stealthlink2_StealthLink_SHARED_LIBRARY - StealthLink2 shared library
# Stealthlink2_StealthLinkD_SHARED_LIBRARY - StealthLink2 shared library with debug info
#



SET( Stealthlink2_PATH_HINTS 
    ../StealthLink-2.0.1
    ../PLTools/StealthLink-2.0.1
    ../../PLTools/StealthLink-2.0.1
    ../trunk/PLTools/StealthLink-2.0.1
    ${CMAKE_CURRENT_BINARY_DIR}/StealthLink-2.0.1
    )
	
IF ( NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio 10" )
		MESSAGE(FATAL_ERROR "error: StealthLink can only be built using Visual Studio 2010")
ENDIF ( NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio 10" )

SET (PLATFORM_SUFFIX "Win32")
IF (CMAKE_HOST_WIN32 AND CMAKE_CL_64)
	SET( PLATFORM_SUFFIX "x64")
ENDIF (CMAKE_HOST_WIN32 AND CMAKE_CL_64)

find_path (Stealthlink2_INCLUDE_DIRS
           NAMES "StealthLink/Stealthlink.h"
		   PATHS ${Stealthlink2_PATH_HINTS} 
           DOC "Include directory, i.e. parent directory of directory \"StealthLink\"")

FIND_LIBRARY (Stealthlink2_StealthLink_STATIC_LIBRARY
			  NAMES StealthLink
			  PATH_SUFFIXES /windows/${PLATFORM_SUFFIX}/Release 
			  PATHS ${Stealthlink2_PATH_HINTS}
			  )
FIND_LIBRARY (Stealthlink2_StealthLinkD_STATIC_LIBRARY
			  NAMES StealthLink
			  PATH_SUFFIXES /windows/${PLATFORM_SUFFIX}/Debug 
			  PATHS ${Stealthlink2_PATH_HINTS}
			  )
FIND_FILE (Stealthlink2_StealthLink_SHARED_LIBRARY
    	   NAMES StealthLink${CMAKE_SHARED_LIBRARY_SUFFIX}
		   PATH_SUFFIXES /windows/${PLATFORM_SUFFIX}/Release 
		   PATHS ${Stealthlink2_PATH_HINTS}
		   )
FIND_FILE (Stealthlink2_StealthLinkD_SHARED_LIBRARY
		   NAMES StealthLink${CMAKE_SHARED_LIBRARY_SUFFIX}
		   PATH_SUFFIXES /windows/${PLATFORM_SUFFIX}/Debug
		   PATHS ${Stealthlink2_PATH_HINTS}
		   )				  

MESSAGE ("STATIC Stealthlink2_StealthLink_STATIC_LIBRARY:" ${Stealthlink2_StealthLink_STATIC_LIBRARY})
MESSAGE ("SHARED Stealthlink2_StealthLinkD_STATIC_LIBRARY:" ${Stealthlink2_StealthLinkD_STATIC_LIBRARY})
MESSAGE ("STATIC Stealthlink2_StealthLink_SHARED_LIBRARY:" ${Stealthlink2_StealthLink_SHARED_LIBRARY})
MESSAGE ("SHARED Stealthlink2_StealthLinkD_SHARED_LIBRARY:" ${Stealthlink2_StealthLinkD_SHARED_LIBRARY})

# make sure we have everything we need
set (Stealthlink2_FOUND FALSE)


if (Stealthlink2_STATIC_LIBRARY)
    set (Stealthlink2_LIBRARIES ${Stealthlink2_STATIC_LIBRARY})
endif (Stealthlink2_STATIC_LIBRARY)

# set to true if one library found along with include directory
if (Stealthlink2_INCLUDE_DIRS AND Stealthlink2_StealthLink_STATIC_LIBRARY)
   set (Stealthlink2_FOUND TRUE)
   #if (WIN32)
   #    message (SEND_ERROR "Windows is not currently supported")
   #endif (WIN32)
   mark_as_advanced (Stealthlink2_INCLUDE_DIRS
		     Stealthlink2_STATIC_LIBRARY
                     Stealthlink2_StealthLink_STATIC_LIBRARY)
endif (Stealthlink2_INCLUDE_DIRS AND Stealthlink2_StealthLink_STATIC_LIBRARY)


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(StealthLink2 DEFAULT_MSG  
  Stealthlink2_INCLUDE_DIRS  
  Stealthlink2_StealthLink_STATIC_LIBRARY
  Stealthlink2_StealthLinkD_STATIC_LIBRARY
  Stealthlink2_StealthLink_SHARED_LIBRARY
  Stealthlink2_StealthLinkD_SHARED_LIBRARY
  )
MESSAGE ( "FindPackageHandleStandardArgs:" ${FindPackageHandleStandardArgs})


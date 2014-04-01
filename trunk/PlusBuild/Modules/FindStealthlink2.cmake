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
get_filename_component(FileName
                         ${Stealthlink2_StealthLink_STATIC_LIBRARY}
                         NAME)
						 
# SET ( External_Libraries_Install
#	${Stealthlink2_StealthLink_SHARED_LIBRARY}
#	"C:/devel/PLTools/ImagingControl/ImagingControl-3.2/win32/release/TIS_DShowLib09.dll"
#	"C:/devel/PLTools/ImagingControl/ImagingControl-3.2/win32/release/TIS_UDSHL09_vc10.dll"
#	)
#SET(External_Libraries_Debug
#    ${Stealthlink2_StealthLinkD_STATIC_LIBRARY}
#	${Stealthlink2_StealthLinkD_SHARED_LIBRARY}
#	"C:/devel/PLTools/ImagingControl/ImagingControl-3.2/win32/debug/TIS_DShowLib09d.dll"
#	"C:/devel/PLTools/ImagingControl/ImagingControl-3.2/win32/debug/TIS_UDSHL09_vc10d.dll"
#	)
#SET(External_Libraries_Debug_FileName)
#FOREACH (DEBUG ${External_Libraries_Debug})
#	get_filename_component(FileName ${DEBUG} NAME)
#	SET(External_Libraries_Debug_FileName ${External_Libraries_Debug_FileName} ${FileName})
#ENDFOREACH(DEBUG)	

#FOREACH (RELEASE ${External_Libraries_Install})
#	get_filename_component(FileName ${RELEASE} NAME)
#	list(FIND External_Libraries_Debug_FileName ${FileName} HasTheFileName)
#	IF(HasTheFileName EQUAL -1)
#		SET(External_Libraries_Install_Minus_Debug ${External_Libraries_Install_Minus_Debug} ${RELEASE})
#	ENDIF()
#ENDFOREACH(RELEASE)
MESSAGE ("Stealthlink2_StealthLink_STATIC_LIBRARY " ${Stealthlink2_StealthLink_STATIC_LIBRARY})
MESSAGE ("Stealthlink2_StealthLink_SHARED_LIBRARY " ${Stealthlink2_StealthLink_SHARED_LIBRARY})
MESSAGE ("Stealthlink2_StealthLinkD_STATIC_LIBRARY " ${Stealthlink2_StealthLinkD_STATIC_LIBRARY})
MESSAGE ("Stealthlink2_StealthLinkD_SHARED_LIBRARY " ${Stealthlink2_StealthLinkD_SHARED_LIBRARY})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Stealthlink2 DEFAULT_MSG  
  Stealthlink2_INCLUDE_DIRS  
  Stealthlink2_StealthLink_STATIC_LIBRARY
  Stealthlink2_StealthLink_SHARED_LIBRARY
  Stealthlink2_StealthLinkD_STATIC_LIBRARY
  Stealthlink2_StealthLinkD_SHARED_LIBRARY
  )


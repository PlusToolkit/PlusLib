# --------------------------------------------------------------------------
# Sources

SET ( PlusCommon_SRCS 
		vtkAccurateTimer.cxx 
		PlusLogger.cxx
		vtkHTMLGenerator.cxx 
		vtkGnuplotExecuter.cxx
        UsImageConverterCommon.cxx
		vtkFileFinder.cxx
        PlusMath.cxx 
		)

SET (PlusCommon_HDRS)
if (WIN32)	
	SET (PlusCommon_HDRS
		PlusCommon.h
		vtkAccurateTimer.h
		WindowsAccurateTimer.h
		PlusLogger.h
		vtkHTMLGenerator.h
		vtkGnuplotExecuter.h
        vtkTimestampedCircularBuffer.h 
        vtkTimestampedCircularBuffer.txx
		vtkFileFinder.h
        UsImageConverterCommon.h
        PlusMath.h
		)
endif (WIN32)	

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

SET (External_Libraries_Install)

# --------------------------------------------------------------------------
# Build the library

SET (PlusCommon_LIBS ITKIO vtkCommon vtkHybrid vtkFiltering ${PlusCommon_LIBS} )

if (WIN32)	
	SET (PlusCommon_LIBS ${PlusCommon_LIBS} Winmm )
endif (WIN32)

ADD_LIBRARY(vtkPlusCommon ${PlusCommon_SRCS} ${PlusCommon_HDRS})

TARGET_LINK_LIBRARIES(	vtkPlusCommon ${PlusCommon_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusCommon CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusCommon vtkPlusCommon ${PlusLib_BINARY_DIR}/src/PlusCommon/vtkPlusCommon.vcproj CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

INSTALL(FILES 
	${External_Libraries_Install}
	DESTINATION bin
    CONFIGURATIONS Release
	COMPONENT RuntimeLibraries
	)
	
INSTALL(TARGETS vtkPlusCommon
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)

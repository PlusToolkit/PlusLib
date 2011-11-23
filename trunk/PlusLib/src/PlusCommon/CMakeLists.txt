# --------------------------------------------------------------------------
# Sources

SET ( PlusCommon_SRCS 
		vtkAccurateTimer.cxx 
		vtkPlusLogger.cxx
		vtkHTMLGenerator.cxx 
		vtkGnuplotExecuter.cxx
		vtkPlusConfig.cxx
    PlusMath.cxx 
    vtkTransformCombiner.cxx
		)

SET (PlusCommon_HDRS)
if (WIN32)	
	SET (PlusCommon_HDRS
		PlusCommon.h
		vtkAccurateTimer.h
		WindowsAccurateTimer.h
		vtkPlusLogger.h
		vtkHTMLGenerator.h
		vtkGnuplotExecuter.h
    vtkTimestampedCircularBuffer.h 
    vtkTimestampedCircularBuffer.txx
		vtkPlusConfig.h
    PlusMath.h
    vtkTransformCombiner.h
		)
endif (WIN32)	

SET (PlusCommon_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (PlusCommon_LIBS ITKIO vtkCommon vtkHybrid vtkFiltering ${PlusCommon_LIBS} )

if (WIN32)	
	SET (PlusCommon_LIBS ${PlusCommon_LIBS} Winmm )
endif (WIN32)

INCLUDE_DIRECTORIES( ${PlusCommon_INCLUDE_DIRS} )
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
	
INSTALL(TARGETS vtkPlusCommon
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries
    )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

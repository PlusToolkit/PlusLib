# Sources
SET (UsSimulator_SRCS
    vtkUsSimulatorAlgo.cxx
    )

SET (UsSimulator_HDRS)
if (WIN32)    
    SET (UsSimulator_HDRS
      vtkUsSimulatorAlgo.h

    )
endif (WIN32)

SET (UsSimulator_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (UsSimulator_LIBS
    
    winmm
    vtkPlusCommon # An example of a pre-built library
	vtkCommon 
	vtkInfovis 
	itkvnl 
	itkvnl_algo 
	PatternLocAlgo 
	vtkTracking
	vtkFiltering
	vtkImageAcquisition
	vtkSignaling
	vtkHybrid 

  )     

SET (UsSimulator_INCLUDE_DIRS ${UsSimulator_INCLUDE_DIRS} ${UsSimulator_INCLUDE_DIRS} CACHE INTERNAL "" FORCE    )
INCLUDE_DIRECTORIES( ${UsSimulator_INCLUDE_DIRS} )
ADD_LIBRARY(UsSimulator ${UsSimulator_SRCS} ${UsSimulator_HDRS})
TARGET_LINK_LIBRARIES(UsSimulator ${UsSimulator_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} UsSimulator CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_UsSimulator UsSimulator ${PlusLib_BINARY_DIR}/src/UsSimulator/UsSimulator.vcproj vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
	ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS UsSimulator
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)
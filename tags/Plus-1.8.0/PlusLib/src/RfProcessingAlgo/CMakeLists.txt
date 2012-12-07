# Sources
SET (RfProcessingAlgo_SRCS
    vtkRfToBrightnessConvert.cxx
    vtkUsScanConvert.cxx 
    vtkUsScanConvertLinear.cxx 
    vtkUsScanConvertCurvilinear.cxx 
    vtkRfProcessor.cxx
    )

SET (RfProcessingAlgo_HDRS)
if (WIN32)    
  SET (RfProcessingAlgo_HDRS
    vtkRfToBrightnessConvert.h 
    vtkUsScanConvert.h
    vtkUsScanConvertLinear.h 
    vtkUsScanConvertCurvilinear.h 
    vtkRfProcessor.h
  )
endif (WIN32)

SET (RfProcessingAlgo_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (RfProcessingAlgo_LIBS
  vtkPlusCommon
  )     

INCLUDE_DIRECTORIES( ${RfProcessingAlgo_INCLUDE_DIRS} )
ADD_LIBRARY(vtkRfProcessingAlgo ${RfProcessingAlgo_SRCS} ${RfProcessingAlgo_HDRS})
TARGET_LINK_LIBRARIES(vtkRfProcessingAlgo ${RfProcessingAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkRfProcessingAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkRfProcessingAlgo vtkRfProcessingAlgo;"${PlusLib_BINARY_DIR}/src/RfProcessingAlgo/vtkRfProcessingAlgo.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS vtkRfProcessingAlgo
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )

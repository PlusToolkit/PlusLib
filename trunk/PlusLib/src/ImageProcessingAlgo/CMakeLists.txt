# Sources
SET (ImageProcessingAlgo_SRCS
    vtkTrackedFrameProcessor.cxx
    vtkBoneEnhancer.cxx
    vtkRfToBrightnessConvert.cxx
    vtkUsScanConvert.cxx 
    vtkUsScanConvertLinear.cxx 
    vtkUsScanConvertCurvilinear.cxx 
    vtkRfProcessor.cxx
    )

SET (ImageProcessingAlgo_HDRS)
if (WIN32)    
  SET (ImageProcessingAlgo_HDRS
    vtkTrackedFrameProcessor.h
    vtkBoneEnhancer.h
    vtkRfToBrightnessConvert.h 
    vtkUsScanConvert.h
    vtkUsScanConvertLinear.h 
    vtkUsScanConvertCurvilinear.h 
    vtkRfProcessor.h
    )
endif (WIN32)

SET (ImageProcessingAlgo_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (ImageProcessingAlgo_LIBS
  vtkPlusCommon
  )

INCLUDE_DIRECTORIES( ${ImageProcessingAlgo_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(vtkImageProcessingAlgo)
ADD_LIBRARY(vtkImageProcessingAlgo ${ImageProcessingAlgo_SRCS} ${ImageProcessingAlgo_HDRS})
TARGET_LINK_LIBRARIES(vtkImageProcessingAlgo ${ImageProcessingAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkImageProcessingAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkImageProcessingAlgo vtkImageProcessingAlgo;"${PlusLib_BINARY_DIR}/src/ImageProcessingAlgo/vtkImageProcessingAlgo.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS vtkImageProcessingAlgo
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION bin CONFIGURATIONS Release COMPONENT Development
  )

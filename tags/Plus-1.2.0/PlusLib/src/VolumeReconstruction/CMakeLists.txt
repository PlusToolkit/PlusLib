# --------------------------------------------------------------------------
# Options

# --------------------------------------------------------------------------
# Sources
SET (VolumeReconstruction_SRCS
	vtkFreehandUltrasound2.cxx 
	vtkFreehandUltrasound2Dynamic.cxx 
	vtkVolumeReconstructor.cxx 
	)

SET (VolumeReconstruction_HDRS)
if (WIN32)	
	SET (VolumeReconstruction_HDRS
		fixed.h
		vtkFreehandUltrasound2.h 
		vtkFreehandUltrasound2Dynamic.h 
		vtkFreehandUltrasound2Helper.h 
		vtkVolumeReconstructor.h 
		)
endif (WIN32)	

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

# --------------------------------------------------------------------------
# Build the library

SET (VolumeReconstruction_LIBS vtkCommon vtkHybrid vtkFiltering vtkPlusCommon vtkSignaling vtkTracking vtkImageAcquisition )

ADD_LIBRARY(vtkVolumeReconstruction ${VolumeReconstruction_SRCS} ${VolumeReconstruction_HDRS})

TARGET_LINK_LIBRARIES(	vtkVolumeReconstruction  ${VolumeReconstruction_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkVolumeReconstruction CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkVolumeReconstruction vtkVolumeReconstruction "${PlusLib_BINARY_DIR}/src/VolumeReconstruction/vtkVolumeReconstruction.vcproj" vtkPlusCommon vtkSignaling vtkTracking vtkImageAcquisition CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS vtkVolumeReconstruction
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)



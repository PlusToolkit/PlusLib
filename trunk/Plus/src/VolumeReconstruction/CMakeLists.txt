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

SET (VolumeReconstruction_LIBS ${VolumeReconstruction_LIBS} )

ADD_LIBRARY(vtkVolumeReconstruction ${VolumeReconstruction_SRCS} ${VolumeReconstruction_HDRS})

TARGET_LINK_LIBRARIES(	vtkVolumeReconstruction vtkCommon vtkHybrid vtkFiltering 
						vtkPlusCommon
						vtkSignaling
						vtkTracking
						vtkImageAcquisition
						${VolumeReconstruction_LIBS}
						)

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS vtkVolumeReconstruction
  DESTINATION bin
  COMPONENT Runtime)



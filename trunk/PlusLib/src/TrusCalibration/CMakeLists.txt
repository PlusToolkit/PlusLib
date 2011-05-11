# --------------------------------------------------------------------------
# Sources

SET (TrusCalibration_SRCS
	BrachyTRUSCalibrator.cxx
	Phantom.cxx
	vtkTemplateToStepperCalibrator.cxx
	vtkCalibratorVisualizationComponent.cxx 
	vtkCalibrationController.cxx 
	vtkProbeCalibrationController.cxx 
	vtkProbeCalibrationControllerIO.cxx 
	vtkStepperCalibrationController.cxx 
	vtkFreehandController.cxx
	StylusCalibrationController.cxx
	PhantomRegistrationController.cxx
	FreehandCalibrationController.cxx
	VolumeReconstructionController.cxx
    vtkMeanShiftClustering.cxx 
	)

SET (TrusCalibration_HDRS)	
IF (WIN32)	
	SET (TrusCalibration_HDRS
		BrachyTRUSCalibrator.h 
		Phantom.h 
		vtkTemplateToStepperCalibrator.h
		vtkCalibratorVisualizationComponent.h
		vtkCalibrationController.h
		vtkProbeCalibrationController.h
		vtkProbeCalibrationControllerIO.h
		vtkStepperCalibrationController.h 
		vtkFreehandController.h
		AbstractToolbox.h
		AbstractToolboxController.h
        vtkMeanShiftClustering.h
		)
ENDIF (WIN32)

ADD_SUBDIRECTORY (TrusCalibrationCommon)

IF (BUILD_TESTING)
	ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
		
# --------------------------------------------------------------------------
# Build the library

SET (TrusCalibration_LIBS ${TrusCalibration_LIBS} UsFidSegAlgo vtkTrusCalibrationCommon vtkPlusCommon)

ADD_LIBRARY( vtkTrusCalibration STATIC ${TrusCalibration_SRCS} ${TrusCalibration_HDRS} )

TARGET_LINK_LIBRARIES(vtkTrusCalibration 
					vtkCommon 
                    vtkInfovis
					itkvnl 
					itkvnl_algo 
					${TrusCalibration_LIBS})

# --------------------------------------------------------------------------
# Install
#

# INSTALL(TARGETS vtkTrusCalibration
  # DESTINATION bin
  # CONFIGURATIONS Release
  # COMPONENT RuntimeLibraries)
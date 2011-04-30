
ADD_EXECUTABLE(VolumeReconstructor
  VolumeReconstructor.cxx
  )

SET (VolumeReconstructor_LIBS 
    vtkCommon
    vtkFiltering
    vtkHybrid
    vtkImageAcquisition
    ITKIO
    vtkTracking
	vtkVolumeReconstruction
    vtkPlusCommon
    )

TARGET_LINK_LIBRARIES(
	VolumeReconstructor
	${VolumeReconstructor_LIBS}
)

# --------------------------------------------------------------------------
# Install
#

IF(PLUSAPP_INSTALL_BIN_DIR)

INSTALL(FILES 
	${PLUS_DATA_DIR}/ConfigFiles/VolumeReconstructionConfig.xml  
    ${PLUS_DATA_DIR}/ConfigFiles/VolRec_SonixRP_Linear14-5_D45mm.xml  
	DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}
	COMPONENT RuntimeLibraries
  )

INSTALL(TARGETS VolumeReconstructor
	RUNTIME DESTINATION ${PLUSAPP_INSTALL_BIN_DIR} COMPONENT Applications
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

PROJECT(SequenceMetaFileConverter )

SET (SequenceMetaFileConverter_LIBS 
    ${SequenceMetaFileConverter_LIBS} 
    itkvnl 
    itkvnl_algo 
    vtkCommon
    vtkFiltering
    vtkHybrid
    vtkImageAcquisition
    ITKIO
    vtkPlusCommon
    )

ADD_EXECUTABLE(SequenceMetaFileConverter SequenceMetaFileConverter.cxx )

TARGET_LINK_LIBRARIES(SequenceMetaFileConverter ${SequenceMetaFileConverter_LIBS} )

# --------------------------------------------------------------------------
# Install
#

IF (PLUSAPP_INSTALL_BIN_DIR)

INSTALL(TARGETS SequenceMetaFileConverter
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
	 
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

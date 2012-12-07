SET (PSEXT_LIBS
#     vtkCommon
#     vtkFiltering
#     vtkHybrid
#     vtkDataCollection 
#     vtkImageAcquisition
#     vtkTracking
     vtkPlusCommon
    )

# ---------------------- DiagDataCollection ---------------------
#
ADD_EXECUTABLE(PointSetExtractor PointSetExtractor.cxx)
TARGET_LINK_LIBRARIES(PointSetExtractor ${PSEXT_LIBS} )

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
INSTALL(TARGETS PointSetExtractor
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)


SET (DIAGDATA_LIBS
     vtkCommon
     vtkFiltering
     vtkHybrid
     vtkDataCollection 
     vtkImageAcquisition
     vtkTracking
     vtkPlusCommon
    )

IF (PLUS_USE_OpenIGTLink)
    SET (DIAGDATA_LIBS ${DIAGDATA_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)    
# ---------------------- DiagDataCollection ---------------------
#
ADD_EXECUTABLE(DiagDataCollection DiagDataCollection.cxx)
TARGET_LINK_LIBRARIES(DiagDataCollection ${DIAGDATA_LIBS} )

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)

INSTALL(TARGETS DiagDataCollection
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)


# ---------------------- DiagDataSync ---------------------
#
ADD_EXECUTABLE(DiagDataSync DiagDataSync.cxx)
TARGET_LINK_LIBRARIES(DiagDataSync ${DIAGDATA_LIBS} )

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)

INSTALL(TARGETS DiagDataSync
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

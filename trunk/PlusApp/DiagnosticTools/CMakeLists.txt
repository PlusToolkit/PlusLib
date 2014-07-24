
SET (DIAGDATA_LIBS
  ${VTK_LIBRARIES}
  vtkDataCollection 
  vtkPlusCommon
  )

IF (PLUS_USE_OpenIGTLink)
  SET (DIAGDATA_LIBS ${DIAGDATA_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)    
# ---------------------- DiagDataCollection ---------------------
#
ADD_EXECUTABLE(DiagDataCollection DiagDataCollection.cxx)
TARGET_LINK_LIBRARIES(DiagDataCollection ${DIAGDATA_LIBS} )
GENERATE_HELP_DOC(DiagDataCollection)

ADD_EXECUTABLE( TrackingDataServer TrackingDataServer.cxx )
TARGET_LINK_LIBRARIES( TrackingDataServer OpenIGTLink )
GENERATE_HELP_DOC(TrackingDataServer)

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS DiagDataCollection
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

SET (DIAGDATA_LIBS
  ${VTK_LIBRARIES}
  vtkDataCollection 
  vtkPlusCommon
  )
IF (PLUS_USE_OpenIGTLink)
  SET (DIAGDATA_LIBS ${DIAGDATA_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)

ADD_EXECUTABLE(DiagDataCollection DiagDataCollection.cxx)
TARGET_LINK_LIBRARIES(DiagDataCollection ${DIAGDATA_LIBS})
GENERATE_HELP_DOC(DiagDataCollection)

#-------------------------------------------------------------------------------------------- 

SET (TRACKINGDATASERVER_LIBS
  ${VTK_LIBRARIES}
  vtkPlusCommon
  )
IF (PLUS_USE_OpenIGTLink)
  SET (TRACKINGDATASERVER_LIBS ${TRACKINGDATASERVER_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)

ADD_EXECUTABLE(TrackingDataServer TrackingDataServer.cxx)
TARGET_LINK_LIBRARIES(TrackingDataServer ${TRACKINGDATASERVER_LIBS})
GENERATE_HELP_DOC(TrackingDataServer)

#-------------------------------------------------------------------------------------------- 
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)

INSTALL(TARGETS DiagDataCollection
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

SET (DIAGDATA_LIBS
  ${VTK_LIBRARIES}
  vtkPlusDataCollection 
  vtkPlusCommon
  )
IF (PLUS_USE_OpenIGTLink)
  SET (DIAGDATA_LIBS ${DIAGDATA_LIBS} OpenIGTLink)
ENDIF(PLUS_USE_OpenIGTLink)

ADD_EXECUTABLE(DiagPlusDataCollection DiagDataCollection.cxx)
TARGET_LINK_LIBRARIES(DiagPlusDataCollection ${DIAGDATA_LIBS})
GENERATE_HELP_DOC(DiagPlusDataCollection)

#-------------------------------------------------------------------------------------------- 

IF (PLUS_USE_OpenIGTLink)
  SET (TRACKINGDATASERVER_LIBS
    ${VTK_LIBRARIES}
    vtkPlusCommon
    vtkPlusServer
    OpenIGTLink
    )
  ADD_EXECUTABLE(TrackingDataServer TrackingDataServer.cxx)
  TARGET_LINK_LIBRARIES(TrackingDataServer ${TRACKINGDATASERVER_LIBS})
  GENERATE_HELP_DOC(TrackingDataServer)
ENDIF(PLUS_USE_OpenIGTLink)

#-------------------------------------------------------------------------------------------- 
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)

INSTALL(TARGETS DiagPlusDataCollection
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

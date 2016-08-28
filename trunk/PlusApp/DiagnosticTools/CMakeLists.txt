IF (PLUS_USE_OpenIGTLink)
  SET(_IGT_LIB OpenIGTLink)
ENDIF()

ADD_EXECUTABLE(DiagPlusDataCollection DiagDataCollection.cxx)
TARGET_LINK_LIBRARIES(DiagPlusDataCollection PUBLIC vtkPlusDataCollection vtkPlusCommon ${_IGT_LIB})
GENERATE_HELP_DOC(DiagPlusDataCollection)

#-------------------------------------------------------------------------------------------- 

IF (PLUS_USE_OpenIGTLink)
  ADD_EXECUTABLE(TrackingDataServer TrackingDataServer.cxx)
  TARGET_LINK_LIBRARIES(TrackingDataServer PUBLIC vtkPlusCommon vtkPlusServer OpenIGTLink)
  GENERATE_HELP_DOC(TrackingDataServer)
ENDIF()

#-------------------------------------------------------------------------------------------- 
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS DiagPlusDataCollection
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()

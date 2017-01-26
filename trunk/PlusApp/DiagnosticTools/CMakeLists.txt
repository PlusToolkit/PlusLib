IF (PLUS_USE_OpenIGTLink)
  SET(_IGT_LIB OpenIGTLink)
ENDIF()

ADD_EXECUTABLE(DiagDataCollection DiagDataCollection.cxx)
TARGET_LINK_LIBRARIES(DiagDataCollection PUBLIC vtkPlusDataCollection vtkPlusCommon ${_IGT_LIB})
GENERATE_HELP_DOC(DiagDataCollection)

#-------------------------------------------------------------------------------------------- 

IF (PLUS_USE_OpenIGTLink)
  ADD_EXECUTABLE(TrackingDataServer TrackingDataServer.cxx)
  TARGET_LINK_LIBRARIES(TrackingDataServer PUBLIC vtkPlusCommon vtkPlusServer OpenIGTLink)
  GENERATE_HELP_DOC(TrackingDataServer)
ENDIF()

#-------------------------------------------------------------------------------------------- 
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS DiagDataCollection
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()

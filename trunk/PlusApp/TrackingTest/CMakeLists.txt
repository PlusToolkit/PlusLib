#--------------------------------------------------------------------------------------------

#***************************  TrackingTest ***************************
ADD_EXECUTABLE(TrackingTest TrackingTest.cxx )
TARGET_LINK_LIBRARIES(TrackingTest vtkDataCollection vtkPlusCommon )
 
 # Install
IF(PLUSAPP_INSTALL_BIN_DIR)
INSTALL(TARGETS TrackingTest
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

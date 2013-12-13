SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------

#***************************  PlusServer ***************************
IF (PLUS_USE_OpenIGTLink )

  ADD_EXECUTABLE( PlusServer PlusServer.cxx )
  TARGET_LINK_LIBRARIES( PlusServer vtkPlusServer vtkDataCollection vtkCommon )
  
  ADD_TEST( PlusServer
    ${EXECUTABLE_OUTPUT_PATH}/PlusServer
    --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_OpenIGTLinkTestServer.xml
    --running-time=5
    --testing-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_OpenIGTLinkTestClient.xml
    )
  SET_TESTS_PROPERTIES( PlusServer PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  
  ADD_EXECUTABLE( PlusServerRemoteControl PlusServerRemoteControl.cxx )
  TARGET_LINK_LIBRARIES( PlusServerRemoteControl vtkDataCollection vtkCommon vtkPlusServer )
  
ENDIF (PLUS_USE_OpenIGTLink )
 
# --------------------------------------------------------------------------
# Install
#

IF (PLUS_USE_OpenIGTLink )
  INSTALL(
  TARGETS
    PlusServer
    PlusServerRemoteControl
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
ENDIF()

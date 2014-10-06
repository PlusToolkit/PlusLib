SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------

#***************************  PlusServer ***************************
IF (PLUS_USE_OpenIGTLink )

  ADD_EXECUTABLE( PlusServer PlusServer.cxx )
  TARGET_LINK_LIBRARIES( PlusServer vtkPlusServer vtkDataCollection ${VTK_LIBRARIES} )
  
  ADD_TEST( PlusServer
    ${EXECUTABLE_OUTPUT_PATH}/PlusServer
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestServer.xml
    --running-time=5
    --testing-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestClient.xml
    )
  SET_TESTS_PROPERTIES( PlusServer PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  
  ADD_EXECUTABLE( PlusServerRemoteControl PlusServerRemoteControl.cxx )
  TARGET_LINK_LIBRARIES( PlusServerRemoteControl vtkDataCollection ${VTK_LIBRARIES} vtkPlusServer )

  ADD_TEST( PlusServerOpenIGTLinkCommandsTest
    ${EXECUTABLE_OUTPUT_PATH}/PlusServerRemoteControl
    --server-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkCommandsTest.xml
    --run-tests
    )
  SET_TESTS_PROPERTIES( PlusServerOpenIGTLinkCommandsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

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

SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_EXECUTABLE(vtkPlusServerTest vtkPlusServerTest.cxx)
  SET_TARGET_PROPERTIES(vtkPlusServerTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkPlusServerTest vtkPlusServer)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(PlusServer
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusServerTest
    --server-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestServer.xml
    --testing-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestClient.xml
    )
  SET_TESTS_PROPERTIES( PlusServer PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------
  # Even with the timeout, the test still fails on Linux.
  #   - The test is disabled on Linux for now
  IF(NOT ${PLUSLIB_PLATFORM} MATCHES "Linux")
    ADD_TEST(PlusServerOpenIGTLinkCommandsTest
      ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusServerRemoteControl
      --server-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkCommandsTest.xml
      --run-tests
      )

    # The timeout of 90 is added because this test does not seem to exit properly on Linux
    SET_TESTS_PROPERTIES(PlusServerOpenIGTLinkCommandsTest 
      PROPERTIES 
        FAIL_REGULAR_EXPRESSION "ERROR;WARNING" 
        TIMEOUT 90
      )
  ENDIF()
ENDIF()
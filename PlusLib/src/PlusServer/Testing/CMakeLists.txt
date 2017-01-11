SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_TEST( PlusServer
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusServer
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestServer.xml
    --running-time=5
    --testing-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestClient.xml
    )
  SET_TESTS_PROPERTIES( PlusServer PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------
  ADD_TEST( PlusServerOpenIGTLinkCommandsTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusServerRemoteControl
    --server-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkCommandsTest.xml
    --run-tests
    )
  # The timeout of 90 is added because this test does not seem to exit properly on Linux
  SET_TESTS_PROPERTIES( PlusServerOpenIGTLinkCommandsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" TIMEOUT 90)
ENDIF()
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------

#***************************  TrackingTest ***************************
ADD_EXECUTABLE(TrackingTest TrackingTest.cxx )
TARGET_LINK_LIBRARIES(TrackingTest vtkDataCollection vtkPlusCommon )

#***************************  PlusServer ***************************
IF (PLUS_USE_OpenIGTLink )
  ADD_EXECUTABLE( PlusServer PlusServer.cxx )
  TARGET_LINK_LIBRARIES( PlusServer vtkDataCollection vtkCommon )

  ADD_TEST( PlusServer
    ${EXECUTABLE_OUTPUT_PATH}/PlusServer
    --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_OpenIGTLinkTest.xml
    --running-time=5
    --testing
    )
  SET_TESTS_PROPERTIES( PlusServer PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_OpenIGTLink )
 
# --------------------------------------------------------------------------
# Install
#

IF (PLUS_USE_OpenIGTLink )
  INSTALL(
  TARGETS
    TrackingTest
    PlusServer
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
ENDIF()

SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# SegmentationParameterDialogTest
# Qt specific commands
IF( ${QT_VERSION_MAJOR} EQUAL 5 )
  FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)

  SET(QT_LIBRARIES ${Qt5Widgets_LIBRARIES})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
ELSE(${QT_VERSION_MAJOR} EQUAL 5)
  FIND_PACKAGE(Qt4 REQUIRED)
  INCLUDE(${QT_USE_FILE})

  INCLUDE_DIRECTORIES(${QT_INCLUDES})
  ADD_DEFINITIONS(${QT_DEFINITIONS})

ENDIF(${QT_VERSION_MAJOR} EQUAL 5)

INCLUDE_DIRECTORIES(${PlusApp_BINARY_DIR}/PlusCommonWidgets)
INCLUDE_DIRECTORIES(${PlusApp_SOURCE_DIR}/PlusCommonWidgets)

#--------------------------------------------------------------------------------------------
  
SET (SegmentationParameterDialogTest_LIBS 
    ${SegmentationParameterDialogTest_LIBS} 
    ${QT_LIBRARIES}
    PlusCommonWidgets
    vtkPlusCommon
    vtkPlusCalibration
    vtkPlusDataCollection
  )
  
ADD_EXECUTABLE(
  SegmentationParameterDialogTest
  SegmentationParameterDialogTestMain.cxx
  SegmentationParameterDialogTest.cxx
  SegmentationParameterDialogTest.h
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/PlusCommonWidgets.qrc
  )
  
TARGET_LINK_LIBRARIES(SegmentationParameterDialogTest ${SegmentationParameterDialogTest_LIBS})

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)

  INSTALL(TARGETS SegmentationParameterDialogTest 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )

ENDIF(PLUSAPP_INSTALL_BIN_DIR)

# --------------------------------------------------------------------------
# ToolStateDisplayWidgetTest
# --------------------------------------------------------------------------
# Qt specific commands
IF( ${QT_VERSION_MAJOR} EQUAL 5 )
  FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)

  SET(QT_LIBRARIES ${Qt5Widgets_LIBRARIES})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
ELSE(${QT_VERSION_MAJOR} EQUAL 5)
  FIND_PACKAGE(Qt4 REQUIRED)
  INCLUDE(${QT_USE_FILE})

  INCLUDE_DIRECTORIES(${QT_INCLUDES})
  ADD_DEFINITIONS(${QT_DEFINITIONS})

ENDIF(${QT_VERSION_MAJOR} EQUAL 5)

INCLUDE_DIRECTORIES(${PlusApp_BINARY_DIR}/PlusCommonWidgets)
INCLUDE_DIRECTORIES(${PlusApp_SOURCE_DIR}/PlusCommonWidgets)

#--------------------------------------------------------------------------------------------
  
SET (ToolStateDisplayWidgetTest_LIBS 
    ${ToolStateDisplayWidgetTest_LIBS} 
    ${QT_LIBRARIES}
    PlusCommonWidgets
    vtkPlusCommon
    vtkPlusDataCollection
  )
  
ADD_EXECUTABLE(
  ToolStateDisplayWidgetTest
  ToolStateDisplayWidgetTestMain.cxx
  ToolStateDisplayWidgetTest.cxx
  ToolStateDisplayWidgetTest.h
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/PlusCommonWidgets.qrc
  )
  
TARGET_LINK_LIBRARIES(ToolStateDisplayWidgetTest ${ToolStateDisplayWidgetTest_LIBS})

#--------------------------------------------------------------------------------------------

IF (PLUSAPP_TEST_GUI)

  ADD_TEST(DeviceSetSelectorWidgetTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/Testing/DeviceSetSelectorWidgetTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-dir=${ConfigFilesDir}/Testing
          --image-dir=${TestDataDir}
      )
  SET_TESTS_PROPERTIES( DeviceSetSelectorWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(StatusIconTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/Testing/StatusIconTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-dir=${ConfigFilesDir}/Testing
          --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_fCal_SonixTouch_L14-5_Ascension3DG_1.0.xml
          --image-dir=${TestDataDir}
      )
  SET_TESTS_PROPERTIES( StatusIconTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(SegmentationParameterDialogTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/Testing/SegmentationParameterDialogTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-dir=${ConfigFilesDir}/Testing
          --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SegmentationParameterDialog_SavedDataset_1.0.xml
          --image-dir=${TestDataDir}
      )
  SET_TESTS_PROPERTIES( SegmentationParameterDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(ConfigFileSaverDialogTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/Testing/ConfigFileSaverDialogTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-dir=${ConfigFilesDir}/Testing
          --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SegmentationParameterDialog_SavedDataset_1.0.xml
          --image-dir=${TestDataDir}
          --verify-saved-configuration-file
      )
  SET_TESTS_PROPERTIES( ConfigFileSaverDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(ToolStateDisplayWidgetTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/Testing/ToolStateDisplayWidgetTest.sikuli
        --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/ToolStateDisplayWidgetTest
          --device-set-configuration-dir=${ConfigFilesDir}/Testing
          --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_FakeTracker_ToolState.xml
      )
  SET_TESTS_PROPERTIES( ToolStateDisplayWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ENDIF (PLUSAPP_TEST_GUI)

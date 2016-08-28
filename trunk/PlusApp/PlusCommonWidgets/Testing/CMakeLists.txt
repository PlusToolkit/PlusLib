SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# SegmentationParameterDialogTest
SET (SegmentationParameterDialogTest_LIBS 
  Qt5::Widgets
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
  )

SET_TARGET_PROPERTIES(SegmentationParameterDialogTest PROPERTIES 
  COMPILE_DEFINTIIONS 
    ${Qt5Widgets_COMPILE_DEFINITIONS}
  )
target_include_directories(SegmentationParameterDialogTest PRIVATE 
  ${PlusApp_BINARY_DIR}/PlusCommonWidgets
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets
  )
TARGET_LINK_LIBRARIES(SegmentationParameterDialogTest PRIVATE ${SegmentationParameterDialogTest_LIBS})

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS SegmentationParameterDialogTest 
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()

# --------------------------------------------------------------------------
# ToolStateDisplayWidgetTest
SET (ToolStateDisplayWidgetTest_LIBS 
  Qt5::Widgets
  Qt5::Core
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
SET_TARGET_PROPERTIES(ToolStateDisplayWidgetTest PROPERTIES 
  COMPILE_DEFINTIIONS 
    ${Qt5Widgets_COMPILE_DEFINITIONS}
  )
target_include_directories(ToolStateDisplayWidgetTest PRIVATE 
  ${PlusApp_BINARY_DIR}/PlusCommonWidgets
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets
  )
TARGET_LINK_LIBRARIES(ToolStateDisplayWidgetTest PRIVATE ${ToolStateDisplayWidgetTest_LIBS})

#--------------------------------------------------------------------------------------------

ADD_TEST(DeviceSetSelectorWidgetTest
  ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
  --device-set-configuration-dir=${ConfigFilesDir}/Testing
  --image-dir=${TestDataDir}
  )
SET_TESTS_PROPERTIES( DeviceSetSelectorWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(StatusIconTest
  ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
  --device-set-configuration-dir=${ConfigFilesDir}/Testing
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_fCal_SonixTouch_L14-5_Ascension3DG_1.0.xml
  --image-dir=${TestDataDir}
  )
SET_TESTS_PROPERTIES( StatusIconTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(SegmentationParameterDialogTest
  ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
  --device-set-configuration-dir=${ConfigFilesDir}/Testing
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SegmentationParameterDialog_SavedDataset_1.0.xml
  --image-dir=${TestDataDir}
  )
SET_TESTS_PROPERTIES( SegmentationParameterDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(ConfigFileSaverDialogTest
  ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
  --device-set-configuration-dir=${ConfigFilesDir}/Testing
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SegmentationParameterDialog_SavedDataset_1.0.xml
  --image-dir=${TestDataDir}
  --verify-saved-configuration-file
  )
SET_TESTS_PROPERTIES( ConfigFileSaverDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(ToolStateDisplayWidgetTest
  ${TEST_EXECUTABLE_OUTPUT_PATH}/ToolStateDisplayWidgetTest
  --device-set-configuration-dir=${ConfigFilesDir}/Testing
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_FakeTracker_ToolState.xml
  )
SET_TESTS_PROPERTIES( ToolStateDisplayWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
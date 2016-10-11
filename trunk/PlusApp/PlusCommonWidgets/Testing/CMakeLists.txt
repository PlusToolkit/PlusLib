SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# SegmentationParameterDialogTest
SET (SegmentationParameterDialogTest_LIBS 
  Qt5::Widgets
  Qt5::Test
  PlusCommonWidgets
  vtkPlusCommon
  vtkPlusCalibration
  vtkPlusDataCollection
  )

ADD_EXECUTABLE(SegmentationParameterDialogTest SegmentationParameterDialogTest.cxx)
SET_TARGET_PROPERTIES(SegmentationParameterDialogTest PROPERTIES 
  COMPILE_DEFINTIIONS ${Qt5Widgets_COMPILE_DEFINITIONS}
  FOLDER Tests
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
  Qt5::Test
  PlusCommonWidgets
  vtkPlusCommon
  vtkPlusDataCollection
  )
  
ADD_EXECUTABLE(ToolStateDisplayWidgetTest ToolStateDisplayWidgetTest.cxx 
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets/PlusCommonWidgets.qrc 
  )
SET_TARGET_PROPERTIES(ToolStateDisplayWidgetTest PROPERTIES 
  COMPILE_DEFINTIIONS ${Qt5Widgets_COMPILE_DEFINITIONS}
  FOLDER Tests
  )
target_include_directories(ToolStateDisplayWidgetTest PRIVATE 
  ${PlusApp_BINARY_DIR}/PlusCommonWidgets
  ${PlusApp_SOURCE_DIR}/PlusCommonWidgets
  )
TARGET_LINK_LIBRARIES( ToolStateDisplayWidgetTest PRIVATE ${ToolStateDisplayWidgetTest_LIBS} )

#--------------------------------------------------------------------------------------------

ADD_TEST(DeviceSetSelectorWidgetTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest)
SET_TESTS_PROPERTIES( DeviceSetSelectorWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(StatusIconTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest)
SET_TESTS_PROPERTIES( StatusIconTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(SegmentationParameterDialogTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest)
SET_TESTS_PROPERTIES( SegmentationParameterDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------

ADD_TEST(ConfigFileSaverDialogTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest)
SET_TESTS_PROPERTIES( ConfigFileSaverDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_TEST(ToolStateDisplayWidgetTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/ToolStateDisplayWidgetTest)
SET_TESTS_PROPERTIES( ToolStateDisplayWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
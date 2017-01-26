SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# SegmentationParameterDialogTest
SET (SegmentationParameterDialogTest_LIBS 
  Qt5::Widgets
  Qt5::Test
  PlusWidgets
  vtkPlusCommon
  vtkPlusCalibration
  vtkPlusDataCollection
  )

ADD_EXECUTABLE(SegmentationParameterDialogTest SegmentationParameterDialogTest.cxx)
SET_TARGET_PROPERTIES(SegmentationParameterDialogTest PROPERTIES 
  COMPILE_DEFINTIIONS ${Qt5Widgets_COMPILE_DEFINITIONS}
  FOLDER Tests
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

ADD_TEST(SegmentationParameterDialogTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest)
SET_TESTS_PROPERTIES( SegmentationParameterDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
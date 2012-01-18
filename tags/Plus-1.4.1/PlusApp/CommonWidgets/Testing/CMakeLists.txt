FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# SegmentationParameterDialogTest

QT4_WRAP_CPP( SegmentationParameterDialogTest_QT_MOC_Files SegmentationParameterDialogTest.h )			  
QT4_ADD_RESOURCES( SegmentationParameterDialogTest_QT_RCC_Files ${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc )

SOURCE_GROUP("Generated Files" FILES ${SegmentationParameterDialogTest_QT_MOC_Files} ${SegmentationParameterDialogTest_QT_RCC_Files})

include_directories(${PlusApp_BINARY_DIR}/CommonWidgets)
include_directories(${PlusApp_SOURCE_DIR}/CommonWidgets)

#--------------------------------------------------------------------------------------------
  
SET (SegmentationParameterDialogTest_LIBS 
    ${SegmentationParameterDialogTest_LIBS} 
    ${QT_LIBRARIES}
    CommonWidgets
    vtkPlusCommon
    PatternLocAlgo
    vtkDataCollection
  )
  
ADD_EXECUTABLE(
  SegmentationParameterDialogTest
  SegmentationParameterDialogTestMain.cxx
  SegmentationParameterDialogTest.cxx
  SegmentationParameterDialogTest.h
  ${SegmentationParameterDialogTest_QT_MOC_Files}
  ${SegmentationParameterDialogTest_QT_RCC_Files}
  )
  
TARGET_LINK_LIBRARIES(SegmentationParameterDialogTest ${SegmentationParameterDialogTest_LIBS})

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
# show up in the Start Menu in Windows
if( WIN32 AND NOT UNIX )
    SET(CPACK_PACKAGE_EXECUTABLES "SegmentationParameterDialogTest" "SegmentationParameterDialogTest")
endif()

INSTALL(TARGETS SegmentationParameterDialogTest 
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

# --------------------------------------------------------------------------
# ToolStateDisplayWidgetTest

QT4_WRAP_CPP( ToolStateDisplayWidgetTest_QT_MOC_Files ToolStateDisplayWidgetTest.h )			  
QT4_ADD_RESOURCES( ToolStateDisplayWidgetTest_QT_RCC_Files ${PlusApp_SOURCE_DIR}/CommonWidgets/CommonWidgets.qrc )

SOURCE_GROUP("Generated Files" FILES ${ToolStateDisplayWidgetTest_QT_MOC_Files} ${ToolStateDisplayWidgetTest_QT_RCC_Files})

include_directories(${PlusApp_BINARY_DIR}/CommonWidgets)
include_directories(${PlusApp_SOURCE_DIR}/CommonWidgets)

#--------------------------------------------------------------------------------------------
  
SET (ToolStateDisplayWidgetTest_LIBS 
    ${ToolStateDisplayWidgetTest_LIBS} 
    ${QT_LIBRARIES}
    CommonWidgets
    vtkPlusCommon
    vtkDataCollection
  )
  
ADD_EXECUTABLE(
  ToolStateDisplayWidgetTest
  ToolStateDisplayWidgetTestMain.cxx
  ToolStateDisplayWidgetTest.cxx
  ToolStateDisplayWidgetTest.h
  ${ToolStateDisplayWidgetTest_QT_MOC_Files}
  ${ToolStateDisplayWidgetTest_QT_RCC_Files}
  )
  
TARGET_LINK_LIBRARIES(ToolStateDisplayWidgetTest ${ToolStateDisplayWidgetTest_LIBS})

#--------------------------------------------------------------------------------------------

IF (PLUSAPP_TEST_GUI)

  ADD_TEST(DeviceSetSelectorWidgetTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/CommonWidgets/Testing/DeviceSetSelectorWidgetTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-directory-path=${ConfigFilesDir}
          --image-directory-path=${TestDataDir}
      )
  SET_TESTS_PROPERTIES( DeviceSetSelectorWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(StatusIconTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/CommonWidgets/Testing/StatusIconTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-directory-path=${ConfigFilesDir}
          --input-config-file-name=${ConfigFilesDir}/PlusConfiguration_SonixTouch_Ascension3DG_EC9_fCal1.xml
          --image-directory-path=${TestDataDir}
      )
  SET_TESTS_PROPERTIES( StatusIconTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(SegmentationParameterDialogTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/CommonWidgets/Testing/SegmentationParameterDialogTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-directory-path=${ConfigFilesDir}
          --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_SegmentationParameterDialog_SavedDataset_fCal_1.0_VideoOnly.xml
          --image-directory-path=${TestDataDir}
      )
  SET_TESTS_PROPERTIES( SegmentationParameterDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(ConfigFileSaverDialogTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/CommonWidgets/Testing/ConfigFileSaverDialogTest.sikuli
      --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
          --device-set-configuration-directory-path=${ConfigFilesDir}
          --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_SegmentationParameterDialog_SavedDataset_fCal_1.0_VideoOnly.xml
          --image-directory-path=${TestDataDir}
          --verify-saved-configuration-file
      )
  SET_TESTS_PROPERTIES( ConfigFileSaverDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #--------------------------------------------------------------------------------------------

  ADD_TEST(ToolStateDisplayWidgetTest
      ${SIKULI_BIN_DIR}/Sikuli-IDE.exe
      -stderr
      -run ${PlusApp_SOURCE_DIR}/CommonWidgets/Testing/ToolStateDisplayWidgetTest.sikuli
        --args
          ${TEST_EXECUTABLE_OUTPUT_PATH}/ToolStateDisplayWidgetTest
          --device-set-configuration-directory-path=${ConfigFilesDir}
          --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_VideoNone_FakeTracker_ToolState.xml
      )
  SET_TESTS_PROPERTIES( ToolStateDisplayWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ENDIF (PLUSAPP_TEST_GUI)

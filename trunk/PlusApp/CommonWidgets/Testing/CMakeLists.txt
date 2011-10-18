FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# Qt specific commands

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
    UsFidSegAlgo
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

# ADD_TEST(SegmentationParameterDialogTest
		# ${EXECUTABLE_OUTPUT_PATH}/SegmentationParameterDialogTest
		# --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_SavedDataset.xml
		# )
# SET_TESTS_PROPERTIES( SegmentationParameterDialogTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

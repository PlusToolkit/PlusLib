SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# --------------------------------------------------------------------------
# ToolStateDisplayWidgetTest
SET(ToolStateDisplayWidgetTest_LIBS 
  Qt5::Widgets
  Qt5::Core
  Qt5::Test
  PlusWidgets
  vtkPlusCommon
  vtkIGSIOCommon
  vtkPlusDataCollection
  )

ADD_EXECUTABLE(ToolStateDisplayWidgetTest ToolStateDisplayWidgetTest.cxx 
  ${CMAKE_CURRENT_SOURCE_DIR}/../PlusWidgets.qrc 
  )
SET_TARGET_PROPERTIES(ToolStateDisplayWidgetTest PROPERTIES 
  COMPILE_DEFINTIIONS ${Qt5Widgets_COMPILE_DEFINITIONS}
  FOLDER Tests
  )
target_include_directories(ToolStateDisplayWidgetTest PRIVATE 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
TARGET_LINK_LIBRARIES( ToolStateDisplayWidgetTest PRIVATE ${ToolStateDisplayWidgetTest_LIBS} )

#--------------------------------------------------------------------------------------------
ADD_TEST(ToolStateDisplayWidgetTest ${PLUS_EXECUTABLE_OUTPUT_PATH}/ToolStateDisplayWidgetTest)
SET_TESTS_PROPERTIES( ToolStateDisplayWidgetTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
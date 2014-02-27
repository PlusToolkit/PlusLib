SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(vtkUsSimulatorAlgoTest vtkUsSimulatorAlgoTest.cxx )
TARGET_LINK_LIBRARIES(vtkUsSimulatorAlgoTest vtkUsSimulatorAlgo vtkCommon )

ADD_TEST(vtkUsSimulatorAlgoRunTestLinear
  ${EXECUTABLE_OUTPUT_PATH}/vtkUsSimulatorAlgoTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_UsSimulatorAlgoTestLinear.xml
  --transforms-seq-file=${TestDataDir}/SpinePhantom2Freehand.mha
  --output-us-img-file=simulatorOutputLinear.mha 
  )
  SET_TESTS_PROPERTIES( vtkUsSimulatorAlgoRunTestLinear PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsSimulatorAlgoCompareToBaselineTestLinear
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/simulatorOutputLinear.mha 
   ${TestDataDir}/UsSimulatorOutputSpinePhantom2LinearBaseline.mha
  )
SET_TESTS_PROPERTIES(vtkUsSimulatorAlgoCompareToBaselineTestLinear PROPERTIES DEPENDS vtkUsSimulatorAlgoRunTestLinear)

ADD_TEST(vtkUsSimulatorAlgoRunTestCurvilinear
  ${EXECUTABLE_OUTPUT_PATH}/vtkUsSimulatorAlgoTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_UsSimulatorAlgoTestCurvilinear.xml
  --transforms-seq-file=${TestDataDir}/SpinePhantom2Freehand.mha
  --output-us-img-file=simulatorOutputCurvilinear.mha 
  )
  SET_TESTS_PROPERTIES( vtkUsSimulatorAlgoRunTestCurvilinear PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsSimulatorAlgoCompareToBaselineTestCurvilinear
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/simulatorOutputCurvilinear.mha 
   ${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha
  )
SET_TESTS_PROPERTIES(vtkUsSimulatorAlgoCompareToBaselineTestCurvilinear PROPERTIES DEPENDS vtkUsSimulatorAlgoRunTestCurvilinear)


#It is a test only, no need to include in the release package
#INSTALL(TARGETS vtkUsSimulatorAlgoTest
#  RUNTIME
#  DESTINATION bin
#  CONFIGURATIONS Release
#  COMPONENT RuntimeExecutables
#  )

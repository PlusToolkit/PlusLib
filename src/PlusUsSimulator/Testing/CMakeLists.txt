SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(vtkPlusUsSimulatorTest vtkUsSimulatorAlgoTest.cxx )
SET_TARGET_PROPERTIES(vtkPlusUsSimulatorTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkPlusUsSimulatorTest vtkPlusUsSimulator vtkFiltersGeneral)

ADD_TEST(vtkPlusUsSimulatorRunTestLinear
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusUsSimulatorTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_UsSimulatorAlgoTestLinear.xml
  --transforms-seq-file=${TestDataDir}/SpinePhantom2Freehand.igs.mha
  --output-us-img-file=simulatorOutputLinear.igs.mha 
  --use-compression=false
  )
SET_TESTS_PROPERTIES( vtkPlusUsSimulatorRunTestLinear PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkPlusUsSimulatorCompareToBaselineTestLinear
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/simulatorOutputLinear.igs.mha 
   ${TestDataDir}/UsSimulatorOutputSpinePhantom2LinearBaseline.igs.mha
  )
SET_TESTS_PROPERTIES(vtkPlusUsSimulatorCompareToBaselineTestLinear PROPERTIES DEPENDS vtkPlusUsSimulatorRunTestLinear)

ADD_TEST(vtkPlusUsSimulatorRunTestCurvilinear
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusUsSimulatorTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_UsSimulatorAlgoTestCurvilinear.xml
  --transforms-seq-file=${TestDataDir}/SpinePhantom2Freehand.igs.mha
  --output-us-img-file=simulatorOutputCurvilinear.igs.mha 
  --use-compression=false
  )
SET_TESTS_PROPERTIES( vtkPlusUsSimulatorRunTestCurvilinear PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkPlusUsSimulatorCompareToBaselineTestCurvilinear  
  ${CMAKE_COMMAND} -E compare_files 
  ${TEST_OUTPUT_PATH}/simulatorOutputCurvilinear.igs.mha
  ${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.igs.mha
  )
SET_TESTS_PROPERTIES(vtkPlusUsSimulatorCompareToBaselineTestCurvilinear PROPERTIES DEPENDS vtkPlusUsSimulatorRunTestCurvilinear)

#It is a test only, no need to include in the release package
#INSTALL(TARGETS vtkPlusUsSimulatorTest
#  RUNTIME
#  DESTINATION bin
#  COMPONENT RuntimeExecutables
#  )
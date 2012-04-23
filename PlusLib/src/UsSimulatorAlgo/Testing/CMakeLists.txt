SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
#SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#INCLUDE_DIRECTORIES( ${UsSimulatorTest_INCLUDE_DIRS} )
ADD_EXECUTABLE(UsSimulatorAlgoTest vtkUsSimulatorAlgoTest.cxx )
TARGET_LINK_LIBRARIES(UsSimulatorAlgoTest UsSimulatorAlgo vtkCommon )
ADD_TEST(UsSimulatorAlgoRunTest
	${EXECUTABLE_OUTPUT_PATH}/UsSimulatorAlgoTest
	--input-model-file=${TestDataDir}/UsSimulatorAlgoTestModel.stl
	--input-config-file=${TestDataDir}/UsSimulatorAlgoTestTrackingData_config.xml
	--input-transforms-file=${TestDataDir}/UsSimulatorAlgoTestTrackingData.mha  
	--output-us-img-file=simulatorOutput.mha 
	--input-transform-name=ImageToPhantom

	)
	SET_TESTS_PROPERTIES( UsSimulatorAlgoRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(UsSimulatorAlgoCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   simulatorOutput.mha 
   ${TestDataDir}/simulatorOutputBaseline.mha
  )
SET_TESTS_PROPERTIES(UsSimulatorAlgoCompareToBaselineTest PROPERTIES DEPENDS UsSimulatorAlgoRunTest)


INSTALL(TARGETS UsSimulatorAlgoTest
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeExecutables)
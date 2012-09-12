SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(vtkUsSimulatorAlgoTest vtkUsSimulatorAlgoTest.cxx )
TARGET_LINK_LIBRARIES(vtkUsSimulatorAlgoTest vtkUsSimulatorAlgo vtkCommon )
ADD_TEST(vtkUsSimulatorAlgoRunTest
	${EXECUTABLE_OUTPUT_PATH}/vtkUsSimulatorAlgoTest
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_UsSimulatorAlgoTest.xml
	--transforms-seq-file=${TestDataDir}/UsSimulatorAlgoTestTrackingData.mha  
	--output-us-img-file=simulatorOutput.mha 

	)
	SET_TESTS_PROPERTIES( vtkUsSimulatorAlgoRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsSimulatorAlgoCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   simulatorOutput.mha 
   ${TestDataDir}/simulatorOutputBaselineFastAlgo.mha
  )
SET_TESTS_PROPERTIES(vtkUsSimulatorAlgoCompareToBaselineTest PROPERTIES DEPENDS vtkUsSimulatorAlgoRunTest)

#It is a test only, no need to include in the release package
#INSTALL(TARGETS vtkUsSimulatorAlgoTest
#  RUNTIME
#  DESTINATION bin
#  CONFIGURATIONS Release
#  COMPONENT RuntimeExecutables
#  )

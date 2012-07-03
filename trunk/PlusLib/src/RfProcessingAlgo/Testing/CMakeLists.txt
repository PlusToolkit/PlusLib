SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(vtkRfToBrightnessConvertTest vtkRfToBrightnessConvertTest.cxx )
TARGET_LINK_LIBRARIES(vtkRfToBrightnessConvertTest vtkRfProcessingAlgo vtkCommon )
#ADD_TEST(vtkRfToBrightnessConvertRunTest
#	${EXECUTABLE_OUTPUT_PATH}/vtkRfToBrightnessConvertTest
#	--input-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_RfProcessingAlgoTest.xml
#	--input-rf-file=${TestDataDir}/UltrasonixLinearRfData.mha
#	--output-brightness-file=outputUltrasonixLinearBrightnessData.mha 
#	)
#SET_TESTS_PROPERTIES( vtkRfToBrightnessConvertRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#ADD_TEST(vtkRfToBrightnessConvertCompareToBaselineTest
#  ${CMAKE_COMMAND} -E compare_files 
#   outputUltrasonixLinearBrightnessData.mha
#   ${TestDataDir}/UltrasonixLinearBrightnessData.mha
#  )
#SET_TESTS_PROPERTIES(vtkRfToBrightnessConvertCompareToBaselineTest PROPERTIES DEPENDS vtkRfToBrightnessConvertRunTest)

#It is a test only, no need to include in the release package
#INSTALL(TARGETS vtkRfToBrightnessConvertTest
#  RUNTIME
#  DESTINATION bin
#  CONFIGURATIONS Release
#  COMPONENT RuntimeExecutables
#  )

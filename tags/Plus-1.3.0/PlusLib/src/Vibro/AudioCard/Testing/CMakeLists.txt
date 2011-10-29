#***************************  vibroAudioCardTest1 ***************************
ADD_EXECUTABLE(vibroAudioCardTest1 vibroAudioCardTest1.cxx)
TARGET_LINK_LIBRARIES(vibroAudioCardTest1 VibroAudioCard vtkPlusCommon )
ADD_TEST(vibroAudioCardTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vibroAudioCardTest1
  )
SET_TESTS_PROPERTIES( vibroAudioCardTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(vtkTRUSCalibrationTest vtkTRUSCalibrationTest.cxx)
    TARGET_LINK_LIBRARIES(vtkTRUSCalibrationTest itkvnl itkvnl_algo vtkCommon vtkRendering vtkHybrid vtkCalibrationAlgo vtkDataCollection )

    ADD_TEST(vtkTRUSCalibrationTest
      ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
      --calibration-seq-file=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
			--validation-seq-file=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
      --probe-rotation-seq-file=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
      --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_iCal.xml
      --baseline-file=${TestDataDir}/UsTemplateCalibration.results.xml 
      )
    SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

    ADD_TEST(vtkTRUSCalibrationTest_FrameGrabber
      ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
      --calibration-seq-file=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
      --validation-seq-file=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
      --probe-rotation-seq-file=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
      --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml
      --baseline-file=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
      )
    SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest_FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

    ADD_TEST(vtkTRUSCalibrationTest_3NWires
      ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
      --calibration-seq-file=${TestDataDir}/USTC_3NWires_RandomStepperMotionCalibration.mha 
			--validation-seq-file=${TestDataDir}/USTC_3NWires_RandomStepperMotionValidation.mha 
      --probe-rotation-seq-file=${TestDataDir}/USTC_3NWires_ProbeRotation.mha
      --config-file=${ConfigFilesDir}/Queens/PlusConfiguration_SonixTouch_BlackTargetGuideStepper_iCal_1.1.xml
      --baseline-file=${TestDataDir}/UsTemplateCalibration_3NWires.results.xml 
      )
    SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest_3NWires PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

    ENDIF ()
		
#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkStylusCalibrationTest vtkStylusCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkStylusCalibrationTest itkvnl itkvnl_algo vtkCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkStylusCalibrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkStylusCalibrationTest
		--config-file=${ConfigFilesDir}/PlusConfiguration_VideoNone_FakeTracker_PivotCalibration_fCal.xml
		--baseline-file=${TestDataDir}/StylusCalibration.results.xml 
		)
SET_TESTS_PROPERTIES( vtkStylusCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPhantomRegistrationTest vtkPhantomRegistrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkPhantomRegistrationTest itkvnl itkvnl_algo vtkCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkPhantomRegistrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkPhantomRegistrationTest
		--config-file=${ConfigFilesDir}/PlusConfiguration_VideoNone_FakeTracker_RecordPhantomLandmarks_fCal2.0.xml
		--baseline-file=${TestDataDir}/PhantomRegistration.results.xml
		)
SET_TESTS_PROPERTIES( vtkPhantomRegistrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkFreehandCalibrationTest vtkFreehandCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkFreehandCalibrationTest itkvnl itkvnl_algo vtkCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkFreehandCalibration2NWiresTest
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixTouch_Ulterius_fCal.xml
		--calibration-seq-file=${TestDataDir}/fCal_Test_Calibration.mha 
		--validation-seq-file=${TestDataDir}/fCal_Test_Validation.mha 
		--baseline-file=${TestDataDir}/FreehandCalibration2NWires.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibration2NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibration3NWiresTest
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--config-file=${ConfigFilesDir}/PlusConfiguration_File_fCal_1.2.xml
		--calibration-seq-file=${TestDataDir}/fCal_Test_Calibration_3NWires.mha 
		--validation-seq-file=${TestDataDir}/fCal_Test_Validation_3NWires.mha 
		--baseline-file=${TestDataDir}/FreehandCalibration3NWires.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibration3NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibration3NWiresfCal20Test
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--config-file=${ConfigFilesDir}/PlusConfiguration_File_fCal_2.0.xml
		--calibration-seq-file=${TestDataDir}/fCal_Test_Calibration_3NWires_fCal2.0.mha 
		--validation-seq-file=${TestDataDir}/fCal_Test_Validation_3NWires_fCal2.0.mha 
		--baseline-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibration3NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkCenterOfRotationCalibAlgoTest vtkCenterOfRotationCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkCenterOfRotationCalibAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(CenterOfRotationCalibration-Ulterius
    ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
    --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml 
    --source-seq-file=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha
    --baseline-file=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
    )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(CenterOfRotationCalibration-FrameGrabber
    ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
    --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml 
    --source-seq-file=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha
    --baseline-file=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
    )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(CenterOfRotationCalibration-3NWires
    ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
    --config-file=${ConfigFilesDir}/Queens/PlusConfiguration_SonixTouch_BlackTargetGuideStepper_iCal_1.1.xml
    --source-seq-file=${TestDataDir}/USTC_3NWires_ProbeRotation.mha 
    --baseline-file=${TestDataDir}/USTC_3NWires_StepperCalibrationResultBaseline.xml 
    )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-3NWires PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkSpacingCalibAlgoTest vtkSpacingCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkSpacingCalibAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(SpacingCalibAlgoTest-Ulterius
    ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
    --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml 
    --source-seq-files ${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
    --baseline-file=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
    )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(SpacingCalibAlgoTest-FrameGrabber
    ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
    --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml 
    --source-seq-files ${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
    --baseline-file=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
    )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(SpacingCalibAlgoTest-3NWires
    ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
    --config-file=${ConfigFilesDir}/Queens/PlusConfiguration_SonixTouch_BlackTargetGuideStepper_iCal_1.1.xml
    --source-seq-files ${TestDataDir}/USTC_3NWires_ProbeRotation.mha 
    --baseline-file=${TestDataDir}/USTC_3NWires_StepperCalibrationResultBaseline.xml 
    )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-3NWires PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(TemporalCalibrationAlgoTest TemporalCalibrationAlgoTest.cxx)
TARGET_LINK_LIBRARIES( TemporalCalibrationAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkCommon vtkDataCollection vtkCharts)

ADD_TEST(TemporalCalibrationAlgoTest1 
    ${EXECUTABLE_OUTPUT_PATH}/TemporalCalibrationAlgoTest
    --tracker-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer.mha
    --video-seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha
    --sampling-resolution-sec=0.001
    --verbose=2
    )
SET_TESTS_PROPERTIES( TemporalCalibrationAlgoTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

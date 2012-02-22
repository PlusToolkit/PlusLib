SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(vtkTRUSCalibrationTest vtkTRUSCalibrationTest.cxx)
    TARGET_LINK_LIBRARIES(vtkTRUSCalibrationTest itkvnl itkvnl_algo vtkCommon vtkRendering vtkHybrid vtkCalibrationAlgo vtkDataCollection )

    ADD_TEST(vtkTRUSCalibrationTest
            ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
            --input-calibration-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
            --input-validation-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
            --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_iCal.xml
            --input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
            )
    SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

    ADD_TEST(vtkTRUSCalibrationTest_FrameGrabber
            ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
            --input-calibration-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
            --input-validation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
            --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml
            --input-baseline-file-name=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
            )
    SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest_FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ENDIF ()
		
#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkStylusCalibrationTest vtkStylusCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkStylusCalibrationTest itkvnl itkvnl_algo vtkCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkStylusCalibrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkStylusCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_VideoNone_FakeTracker_PivotCalibration_fCal.xml
		--input-baseline-file-name=${TestDataDir}/StylusCalibration.results.xml 
		)
SET_TESTS_PROPERTIES( vtkStylusCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPhantomRegistrationTest vtkPhantomRegistrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkPhantomRegistrationTest itkvnl itkvnl_algo vtkCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkPhantomRegistrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkPhantomRegistrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_VideoNone_FakeTracker_RecordPhantomLandmarks_fCal.xml
		--input-baseline-file-name=${TestDataDir}/PhantomRegistration.results.xml
		)
SET_TESTS_PROPERTIES( vtkPhantomRegistrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkFreehandCalibrationTest vtkFreehandCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkFreehandCalibrationTest itkvnl itkvnl_algo vtkCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkFreehandCalibration2NWiresTest
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixTouch_Ulterius_fCal.xml
		--input-calibration-sequence-metafile=${TestDataDir}/fCal_Test_Calibration.mha 
		--input-validation-sequence-metafile=${TestDataDir}/fCal_Test_Validation.mha 
		--input-baseline-file-name=${TestDataDir}/FreehandCalibration2NWires.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibration2NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibration3NWiresTest
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/PlusConfiguration_File_fCal_1.2.xml
		--input-calibration-sequence-metafile=${TestDataDir}/fCal_Test_Calibration_3NWires.mha 
		--input-validation-sequence-metafile=${TestDataDir}/fCal_Test_Validation_3NWires.mha 
		--input-baseline-file-name=${TestDataDir}/FreehandCalibration3NWires.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibration3NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkCenterOfRotationCalibAlgoTest vtkCenterOfRotationCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkCenterOfRotationCalibAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkImageAcquisition )

ADD_TEST(CenterOfRotationCalibration-Ulterius
        ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
        --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml 
        --input-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha
        --input-baseline-file-name=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
        )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(CenterOfRotationCalibration-FrameGrabber
        ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
        --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml 
        --input-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha
        --input-baseline-file-name=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
        )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkSpacingCalibAlgoTest vtkSpacingCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkSpacingCalibAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkImageAcquisition )

ADD_TEST(SpacingCalibAlgoTest-Ulterius
            ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml 
            --input-sequence-metafiles ${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
            --input-baseline-file-name=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
            )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(SpacingCalibAlgoTest-FrameGrabber
            ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml 
            --input-sequence-metafiles ${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
            --input-baseline-file-name=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
            )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(TemporalCalibrationAlgoTest TemporalCalibrationAlgoTest.cxx)
TARGET_LINK_LIBRARIES( TemporalCalibrationAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkCommon vtkImageAcquisition vtkCharts)

#Disable the test for now, it takes too long time to execute
#ADD_TEST(TemporalCalibrationAlgoTest1 
#  ${EXECUTABLE_OUTPUT_PATH}/TemporalCalibrationAlgoTest
#  --input-sequence-metafile-tracker-position=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer.mha
#  --input-US-image-sequence-metafile=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha
#  --sampling-resolution-sec=0.01
#  )
#SET_TESTS_PROPERTIES( TemporalCalibrationAlgoTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

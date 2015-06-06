SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
IF (PLUS_USE_BRACHY_TRACKER)
  ADD_EXECUTABLE(vtkTRUSCalibrationTest vtkTRUSCalibrationTest.cxx)
  TARGET_LINK_LIBRARIES(vtkTRUSCalibrationTest itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )

  ADD_TEST(vtkTRUSCalibrationTest
    ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
    --calibration-seq-file=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
    --validation-seq-file=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
    --probe-rotation-seq-file=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly.xml
    --baseline-file=${TestDataDir}/UsTemplateCalibration.results.xml 
    )
  SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(vtkTRUSCalibrationTest_FrameGrabber
    ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
    --calibration-seq-file=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
    --validation-seq-file=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
    --probe-rotation-seq-file=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_FrameGrabber.xml
    --baseline-file=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
    )
  SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest_FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(vtkTRUSCalibrationTest_3NWires
    ${EXECUTABLE_OUTPUT_PATH}/vtkTRUSCalibrationTest
    --calibration-seq-file=${TestDataDir}/USTC_3NWires_RandomStepperMotionCalibration.mha 
    --validation-seq-file=${TestDataDir}/USTC_3NWires_RandomStepperMotionValidation.mha 
    --probe-rotation-seq-file=${TestDataDir}/USTC_3NWires_ProbeRotation.mha
    --config-file=${ConfigFilesDir}/Queens/PlusDeviceSet_iCal_SonixTouch_BlackTargetGuideStepper_1.1.xml
    --baseline-file=${TestDataDir}/UsTemplateCalibration_3NWires.results.xml 
    )
  # A warning is expected for non-orthogonal ImageToProbeTransform axes, so don't include "WARNING" in the FAIL_REGULAR_EXPRESSION
  SET_TESTS_PROPERTIES( vtkTRUSCalibrationTest_3NWires PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR" )

ENDIF ()
        
#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkStylusCalibrationTest vtkStylusCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkStylusCalibrationTest itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkStylusCalibrationTest
  ${EXECUTABLE_OUTPUT_PATH}/vtkStylusCalibrationTest
  --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Sim_PivotCalibration.xml
  --baseline-file=${TestDataDir}/StylusCalibration.results.xml 
  --outlier-generation-probability=0.05
  )
SET_TESTS_PROPERTIES( vtkStylusCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

##--------------------------------------------------------------------------------------------
#ADD_EXECUTABLE(vtkPivotDetectionTest vtkPivotDetectionTest.cxx)
#TARGET_LINK_LIBRARIES(vtkPivotDetectionTest itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )

#ADD_TEST(vtkPivotDetectionTest
#  ${EXECUTABLE_OUTPUT_PATH}/vtkPivotDetectionTest
#  --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Sim_PivotCalibration.xml
#  --tracker-input-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer.mha
#  --baseline-file=${TestDataDir}/PivotDetection.results.xml 
#  --outlier-generation-probability=0.05
#  )
#SET_TESTS_PROPERTIES( vtkPivotDetectionTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPhantomRegistrationTest vtkPhantomRegistrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkPhantomRegistrationTest itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkPhantomRegistrationTest
  ${EXECUTABLE_OUTPUT_PATH}/vtkPhantomRegistrationTest
  --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Sim_RecordPhantomLandmarks_1.2.xml
  --baseline-file=${TestDataDir}/PhantomRegistration.results.xml
  )
SET_TESTS_PROPERTIES( vtkPhantomRegistrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPhantomRegistrationLandmarkDetectionTest vtkPhantomRegistrationLandmarkDetectionTest.cxx)
TARGET_LINK_LIBRARIES(vtkPhantomRegistrationLandmarkDetectionTest itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkPhantomRegistrationLandmarkDetectionTest
  ${EXECUTABLE_OUTPUT_PATH}/vtkPhantomRegistrationLandmarkDetectionTest
  --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Sim_RecordPhantomLandmarks.xml
  --seq-file=${TestDataDir}/EightLandmarkPointsTrackedForPhantomRegistration.mha
  --baseline-file=${TestDataDir}/PhantomRegistrationLandmarkDetection.results.xml
  )
SET_TESTS_PROPERTIES( vtkPhantomRegistrationLandmarkDetectionTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------


ADD_EXECUTABLE(ProbeCalibration ProbeCalibration.cxx)
TARGET_LINK_LIBRARIES(ProbeCalibration itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )
GENERATE_HELP_DOC(ProbeCalibration)

ADD_EXECUTABLE(vtkFreehandCalibrationStatisticalEvaluation vtkFreehandCalibrationStatisticalEvaluation.cxx)
TARGET_LINK_LIBRARIES(vtkFreehandCalibrationStatisticalEvaluation itkvnl itkvnl_algo ${VTK_LIBRARIES} vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(vtkFreehandCalibration3NWiresTest
  ${EXECUTABLE_OUTPUT_PATH}/ProbeCalibration
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_fCal_Sim_SpatialCalibration_1.2.xml
  --calibration-seq-file=${TestDataDir}/fCal_Test_Calibration_3NWires.mha 
  --validation-seq-file=${TestDataDir}/fCal_Test_Validation_3NWires.mha 
  --baseline-file=${TestDataDir}/FreehandCalibration3NWires.results.xml
  )
SET_TESTS_PROPERTIES( vtkFreehandCalibration3NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibration3NWiresfCal20Test
  ${EXECUTABLE_OUTPUT_PATH}/ProbeCalibration
  --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Sim_SpatialCalibration_2.0.xml
  --calibration-seq-file=${TestDataDir}/fCal_Test_Calibration_3NWires_fCal2.0.mha 
  --validation-seq-file=${TestDataDir}/fCal_Test_Validation_3NWires_fCal2.0.mha 
  --baseline-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0.results.xml
  )
SET_TESTS_PROPERTIES( vtkFreehandCalibration3NWiresTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibrationIPEIOptimizationMethodTest
  ${EXECUTABLE_OUTPUT_PATH}/ProbeCalibration
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IPEI_OptimizationMethod.xml
  --calibration-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_1.mha 
  --validation-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_2.mha 
  --baseline-file=${TestDataDir}/IPEI_OptimizationMethod_Calibration.results.xml
  )
SET_TESTS_PROPERTIES( vtkFreehandCalibrationIPEIOptimizationMethodTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibrationIPEAOptimizationMethodTest
  ${EXECUTABLE_OUTPUT_PATH}/ProbeCalibration
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IPEA_OptimizationMethod.xml
  --calibration-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_1.mha 
  --validation-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_2.mha 
  --baseline-file=${TestDataDir}/IPEA_OptimizationMethod_Calibration.results.xml
  )
SET_TESTS_PROPERTIES( vtkFreehandCalibrationIPEAOptimizationMethodTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibrationOPEIOptimizationMethodTest
  ${EXECUTABLE_OUTPUT_PATH}/ProbeCalibration
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OPEI_OptimizationMethod.xml
  --calibration-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_1.mha 
  --validation-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_2.mha 
  --baseline-file=${TestDataDir}/OPEI_OptimizationMethod_Calibration.results.xml
  )
SET_TESTS_PROPERTIES( vtkFreehandCalibrationOPEIOptimizationMethodTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibrationOPEAOptimizationMethodTest
  ${EXECUTABLE_OUTPUT_PATH}/ProbeCalibration
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OPEA_OptimizationMethod.xml
  --calibration-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_1.mha 
  --validation-seq-file=${TestDataDir}/FreehandCalibration3NWires_fCal2.0_Depth15_2.mha 
  --baseline-file=${TestDataDir}/OPEA_OptimizationMethod_Calibration.results.xml
  )
SET_TESTS_PROPERTIES( vtkFreehandCalibrationOPEAOptimizationMethodTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkCenterOfRotationCalibAlgoTest vtkCenterOfRotationCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkCenterOfRotationCalibAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(CenterOfRotationCalibration-Ulterius
  ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_SonixRP_Ulterius.xml 
  --source-seq-file=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha
  --baseline-file=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
  )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(CenterOfRotationCalibration-FrameGrabber
  ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_FrameGrabber.xml 
  --source-seq-file=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha
  --baseline-file=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
  )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(CenterOfRotationCalibration-3NWires
  ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
  --config-file=${ConfigFilesDir}/Queens/PlusDeviceSet_iCal_SonixTouch_BlackTargetGuideStepper_1.1.xml
  --source-seq-file=${TestDataDir}/USTC_3NWires_ProbeRotation.mha 
  --baseline-file=${TestDataDir}/USTC_3NWires_StepperCalibrationResultBaseline.xml 
  )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-3NWires PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkSpacingCalibAlgoTest vtkSpacingCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkSpacingCalibAlgoTest vtkPlusCommon vtkCalibrationAlgo vtkDataCollection )

ADD_TEST(SpacingCalibAlgoTest-Ulterius
  ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_SonixRP_Ulterius.xml 
  --source-seq-files ${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
  --baseline-file=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
  )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(SpacingCalibAlgoTest-FrameGrabber
  ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_FrameGrabber.xml 
  --source-seq-files ${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
  --baseline-file=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
  )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(SpacingCalibAlgoTest-3NWires
  ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
  --config-file=${ConfigFilesDir}/Queens/PlusDeviceSet_iCal_SonixTouch_BlackTargetGuideStepper_1.1.xml
  --source-seq-files ${TestDataDir}/USTC_3NWires_ProbeRotation.mha 
  --baseline-file=${TestDataDir}/USTC_3NWires_StepperCalibrationResultBaseline.xml 
  )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-3NWires PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(TemporalCalibration TemporalCalibration.cxx)
TARGET_LINK_LIBRARIES( TemporalCalibration vtkPlusCommon vtkCalibrationAlgo ${VTK_LIBRARIES} vtkDataCollection)
GENERATE_HELP_DOC(TemporalCalibration)

ADD_TEST(TemporalCalibrationAlgoTest1 
  ${EXECUTABLE_OUTPUT_PATH}/TemporalCalibration
  --moving-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer.mha
  --moving-probe-to-reference-transform=ProbeToReference
  --fixed-seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha
  --sampling-resolution-sec=0.001
  --baseline-file=${TestDataDir}/TemporalCalibrationResultsBaseline.xml
  )
SET_TESTS_PROPERTIES( TemporalCalibrationAlgoTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkLineSegmentationAlgoTest vtkLineSegmentationAlgoTest.cxx)
TARGET_LINK_LIBRARIES( vtkLineSegmentationAlgoTest vtkPlusCommon vtkCalibrationAlgo ${VTK_LIBRARIES})

ADD_TEST(vtkLineSegmentationAlgoTest1
  ${EXECUTABLE_OUTPUT_PATH}/vtkLineSegmentationAlgoTest
  --seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha
  --baseline-file=${TestDataDir}/LineSegmentationResultsBaseline.xml
  --clip-rect-origin 225 40 --clip-rect-size 350 510
  )
SET_TESTS_PROPERTIES( vtkLineSegmentationAlgoTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


###################################################
ADD_EXECUTABLE( ConvertFcsvToXml
  ConvertFcsvToXml.cxx 
  )

 SET(ITK_LIBRARIES
  ITKCommon
  )

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( ConvertFcsvToXml
  vtkCalibrationAlgo
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  vtkDataCollection
  ) 
  
ADD_TEST(ConvertFcsvToXml
  ${EXECUTABLE_OUTPUT_PATH}/ConvertFcsvToXml
  --test-data-dir=${TestDataDir}
  --fcsv-file=UsTestSeqIla5ManualFidSeg.fcsv
  --img-seq-file=ila5.mhd
  --testcase=ila5
  --output-xml-file=${CMAKE_CURRENT_BINARY_DIR}/UsTestSeqIla5ManualFidSeg.xml
  )
SET_TESTS_PROPERTIES( ConvertFcsvToXml PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( ConvertXmlToFcsv
  ConvertXmlToFcsv.cxx 
  )

 SET(ITK_LIBRARIES
  ITKCommon
  )

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( ConvertXmlToFcsv
  vtkCalibrationAlgo
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  vtkDataCollection
  ) 
  
ADD_TEST(ConvertXmlToFcsv
  ${EXECUTABLE_OUTPUT_PATH}/ConvertXmlToFcsv
  --xml-file=${CMAKE_CURRENT_BINARY_DIR}/UsTestSeqIla5ManualFidSeg.xml
  --output-xml-file=${CMAKE_CURRENT_BINARY_DIR}/UsTestSeqIla5ManualFidSeg2.xml
  )
SET_TESTS_PROPERTIES( ConvertXmlToFcsv PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( PatternLocTest
  PatternLocTest.cxx
  )

INCLUDE_DIRECTORIES( ${PlusLib_SOURCE_DIR}/src/CalibrationAlgo )

SET(ITK_LIBRARIES
  ITKCommon
  )

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( PatternLocTest
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  vtkDataCollection
  vtkCalibrationAlgo  
  vtkDataCollection
  ) 


###################################################
ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened
  ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
  --test-data-dir=${TestDataDir}
  --img-seq-file=UsTestSeqBaselineThomasShortened.mha
  --testcase=UsTestSeqBaselineThomasShortened
  --baseline=${TestDataDir}/UsTestSeqBaselineThomasShortened_baseline.xml
  --output-xml-file=testcomparisons.xml
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_SonixRP_Ulterius.xml
  )
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2
  ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
  --test-data-dir=${TestDataDir}
  --img-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2.mha
  --testcase=SegmentationTest_BKMedical_RandomStepperMotionData2
  --baseline=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Segmentation_baseline.xml
  --output-xml-file=testcomparisons.xml
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_BKMedical_FrameGrabber.xml
  )
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2
  ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
  --test-data-dir=${TestDataDir}
  --img-seq-file=SegmentationTest_VLCUS_RandomStepperMotionData2.mha
  --testcase=SegmentationTest_VLCUS_RandomStepperMotionData2
  --baseline=${TestDataDir}/SegmentationTest_VLCUS_RandomStepperMotionData2_Segmentation_baseline.xml
  --output-xml-file=testcomparisons.xml
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_VLCUS_FrameGrabber.xml
  )
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData
  ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
  --test-data-dir=${TestDataDir}
  --img-seq-file=USTC_FrameGrabber_ProbeRotationData.mha
  --testcase=USTC_FrameGrabber_ProbeRotationData
  --baseline=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData_Segmentation_baseline.xml
  --output-xml-file=testcomparisons.xml
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_SonixRP_FrameGrabber.xml
  )
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData
  ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
  --test-data-dir=${TestDataDir}
  --img-seq-file=USTC_Ulterius_ProbeRotationData.mha
  --testcase=USTC_Ulterius_ProbeRotationData
  --baseline=${TestDataDir}/USTC_Ulterius_ProbeRotationData_Segmentation_baseline.xml
  --output-xml-file=testcomparisons.xml
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_iCal_CalibrationOnly_SonixRP_Ulterius.xml
  )
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CIRS_PHANTOM_13_POINT_TranslationData1
  ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
  --test-data-dir=${TestDataDir}
  --img-seq-file=CIRS_TranslationData1.mha
  --testcase=CIRS_TranslationData1
  --baseline=${TestDataDir}/CIRS_Phantom_TranslationData1_Baseline.xml
  --output-xml-file=testcomparisons.xml
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_CalibrationOnly_Ultrasonix_CIRS_Phantom.xml
  )
SET_TESTS_PROPERTIES( PatternLocTest_CIRS_PHANTOM_13_POINT_TranslationData1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( vtkSegmentedWiresPositionsTest
  vtkSegmentedWiresPositionsTest.cxx
  )

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( vtkSegmentedWiresPositionsTest
  vtkCalibrationAlgo
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  vtkDataCollection
  ) 

# --------------------------------------------------------------------------
# Install
#

INSTALL(
  TARGETS
    TemporalCalibration
    ProbeCalibration
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )

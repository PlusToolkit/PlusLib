SET ( PLUSLIB_CONFIG_FILES
    ${PLUSLIB_DATA_DIR}/ConfigFiles/PhantomDefinition_fCal_1.0_Wiring_1.0.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/PhantomDefinition_iCal_1.0_Wiring_1.0.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/PhantomDefinition_ThomasFreehand_1.0_Wiring_1.0.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/Probe_L14-5_38.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/SonixGPSTrackedUltrasound.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_fCal_SonixRP.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_fCal_SonixTouch.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_FrameGrabber.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_iCal_BKMedical_FrameGrabber.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_iCal_SonixRP_FrameGrabber.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_iCal_SonixRP_Ulterius.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_iCal_VLCUS_FrameGrabber.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_Thomas_SonixRP_FrameGrabber.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_Thomas_SonixRP_Ulterius.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USCalibrationConfig_Thomas_SonixTouch_Ulterius.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USDataCollectionConfig_SavedDataset.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USDataCollectionConfig_SonixRP_L14-5_NDICertus.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USDataCollectionConfig_SonixRP_TrackerNone.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USDataCollectionConfig_SonixTouch_L14-5_NDICertus.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USDataCollectionConfig_SonixTouch_TrackerNone.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/USTemplateCalibrationConfig.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/VolRec_SonixRP_Linear14-5_D45mm.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/VolRec_SonixRP_TRUS_D70mm.xml 
    ${PLUSLIB_DATA_DIR}/ConfigFiles/VolumeReconstructionConfig.xml 
)

INSTALL(FILES 
	${PLUSLIB_CONFIG_FILES}
	DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}
	COMPONENT RuntimeLibraries
  )
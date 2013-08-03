# --------------------------------------------------------------------------
# Install
# 
SET ( PLUSLIB_CONFIG_FILES
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_BkProFocus_OpenIGTLinkTracker.xml 
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_Epiphan_NDIPolaris.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_File_fCal_2.0.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_File_PlusServer.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_NoVideo_NDIAurora.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_NwirePhantomFreehandReconstructionOnly.xml 
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_SavedDataset_TemporalCalibration.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_SonixRP_L14-5_NDICertus_fCal_1.0.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_SonixTouch_Ascension3DG_L14_fCal1.0.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_SonixTouch_Ascension3DG_L14_fCal1.1.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_SonixTouch_Ascension3DG_L14_fCal2.0.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_SonixTouch_L14-5_NDICertus_fCal_1.0.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_VideoNone_FakeTracker_PivotCalibration_fCal.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_VideoNone_FakeTracker_RecordPhantomLandmarks_fCal2.0.xml
  ${PLUSLIB_DATA_DIR}/ConfigFiles/PlusConfiguration_VideoNone_3dConnexion.xml
  ${PLUSLIB_DATA_DIR}/CADModels/EndocavityProbe/EC9-5_10_Assembly.stl
  ${PLUSLIB_DATA_DIR}/CADModels/fCalPhantom/FCal_1.0.stl
  ${PLUSLIB_DATA_DIR}/CADModels/fCalPhantom/FCal_1.1.stl
  ${PLUSLIB_DATA_DIR}/CADModels/fCalPhantom/FCal_1.2.stl
  ${PLUSLIB_DATA_DIR}/CADModels/fCalPhantom/fCal_2.0.stl
  ${PLUSLIB_DATA_DIR}/CADModels/LinearProbe/L14-5_38_ProbeModel.stl
  ${PLUSLIB_DATA_DIR}/CADModels/Stylus/Stylus_Example.stl
)

SET ( NDI_TOOL_DEFINITIONS 
  ${PLUSLIB_DATA_DIR}/ConfigFiles/NdiToolDefinitions/8700339.rom 
  ${PLUSLIB_DATA_DIR}/ConfigFiles/NdiToolDefinitions/8700340.rom 
  ${PLUSLIB_DATA_DIR}/ConfigFiles/NdiToolDefinitions/8700449.rom 
)

SET (BK_SETTINGS
  ${PLUSLIB_DATA_DIR}/ConfigFiles/BkSettings/IniFile.ccf 
  ${PLUSLIB_DATA_DIR}/ConfigFiles/BkSettings/IniFile.ini 
)

SET ( PLUSLIB_DATA_FILES
  ${PLUSLIB_DATA_DIR}/TestImages/fCal_Test_Calibration_3NWires.mha
  ${PLUSLIB_DATA_DIR}/TestImages/fCal_Test_Calibration_3NWires_fCal2.0.mha
  ${PLUSLIB_DATA_DIR}/TestImages/WaterTankBottomTranslationTrackerBuffer.mha
  ${PLUSLIB_DATA_DIR}/TestImages/WaterTankBottomTranslationVideoBuffer.mha
)

# Install Qt libs 
INSTALL(FILES 
  ${QT_BINARY_DIR}/QtCore4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtGui4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtNetwork4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtSql4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtWebKit4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtXmlPatterns4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/phonon4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtOpenGL4${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${QT_BINARY_DIR}/QtXml4${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
)

IF(PLUSAPP_INSTALL_GNUPLOT_DIR)
  INSTALL(
    DIRECTORY ${GNUPLOT_BIN_DIR}/
    DESTINATION ${PLUSAPP_INSTALL_GNUPLOT_DIR}
    CONFIGURATIONS Release
    COMPONENT Scripts
    PATTERN ".svn" EXCLUDE
  )
ENDIF()


IF(PLUSAPP_INSTALL_SCRIPTS_DIR)
  # Gnuplot scripts 
  FILE(GLOB GNUSCRIPTS "${PLUSLIB_SCRIPTS_DIR}/gnuplot/[^.]*.gnu")
  INSTALL( FILES
    ${GNUSCRIPTS}
    DESTINATION "${PLUSAPP_INSTALL_SCRIPTS_DIR}/gnuplot"
    CONFIGURATIONS Release
    COMPONENT Scripts
  )
ENDIF()


IF(PLUSAPP_INSTALL_CONFIG_DIR)
  INSTALL(FILES 
    ${PLUSLIB_CONFIG_FILES}
    DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}
    COMPONENT RuntimeLibraries
    )
  INSTALL(FILES 
    ${NDI_TOOL_DEFINITIONS}
    DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}/NdiToolDefinitions
    COMPONENT RuntimeLibraries
    )
  INSTALL(FILES 
    ${BK_SETTINGS}
    DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}/BkSettings
    COMPONENT RuntimeLibraries
    )    
ENDIF()

IF(PLUSAPP_INSTALL_DATA_DIR)
  INSTALL(FILES 
    ${PLUSLIB_DATA_FILES}
    DESTINATION ${PLUSAPP_INSTALL_DATA_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()

INSTALL(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/PlusConfig.xml
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
)

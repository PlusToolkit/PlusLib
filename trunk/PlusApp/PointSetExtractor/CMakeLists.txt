# --------------------------------------------------------------------------
# PointSetExtractor
ADD_EXECUTABLE(PointSetExtractor PointSetExtractor.cxx)
TARGET_LINK_LIBRARIES(PointSetExtractor PUBLIC vtkPlusCommon)
GENERATE_HELP_DOC(PointSetExtractor)

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS PointSetExtractor
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )  
ENDIF()

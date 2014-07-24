SET (PSEXT_LIBS
     vtkPlusCommon
    )

# ---------------------- DiagDataCollection ---------------------
#
ADD_EXECUTABLE(PointSetExtractor PointSetExtractor.cxx)
TARGET_LINK_LIBRARIES(PointSetExtractor ${PSEXT_LIBS} )
GENERATE_HELP_DOC(PointSetExtractor)

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
INSTALL(TARGETS PointSetExtractor
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

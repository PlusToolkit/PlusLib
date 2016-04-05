# --------------------------------------------------------------------------
# Install

SET(PLUSLIB_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Path to the scripts directory" FORCE) 

IF(PLUS_INSTALL_SCRIPTS_DIR)
#  Uncomment the following lines if anything has to be installed from this directory
#  INSTALL( FILES
#    ${FILENAMES_TO_INSTALL}
#    DESTINATION "${PLUS_INSTALL_SCRIPTS_DIR}"
#    CONFIGURATIONS Release
#    COMPONENT Scripts
#    )
ENDIF(PLUS_INSTALL_SCRIPTS_DIR)

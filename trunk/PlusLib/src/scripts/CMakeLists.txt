# --------------------------------------------------------------------------
# Install
#

IF(PLUS_INSTALL_SCRIPTS_DIR)
    FILE(GLOB NOTDOT "${CMAKE_CURRENT_SOURCE_DIR}/gnuplot/[^.]*.gnu")
 
    INSTALL( FILES
        ${NOTDOT}
        DESTINATION ${PLUS_INSTALL_SCRIPTS_DIR}
        CONFIGURATIONS Release
        COMPONENT Scripts
    )

ENDIF(PLUS_INSTALL_SCRIPTS_DIR)

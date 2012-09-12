# --------------------------------------------------------------------------
# Install

SET(PLUSLIB_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Path to the scripts directory" FORCE) 

IF(PLUS_INSTALL_SCRIPTS_DIR)
    FILE(GLOB NOTDOT "${CMAKE_CURRENT_SOURCE_DIR}/gnuplot/[^.]*.gnu")
 
    INSTALL( FILES
        ${NOTDOT}
        DESTINATION "${PLUS_INSTALL_SCRIPTS_DIR}/gnuplot"
        CONFIGURATIONS Release
        COMPONENT Scripts
    )

ENDIF(PLUS_INSTALL_SCRIPTS_DIR)

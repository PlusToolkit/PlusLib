
SET(PLUSBUILD_OpenIGTLink_ARGS)
IF (PLUSBUILD_USE_OpenIGTLink)
    SET(PLUSBUILD_OpenIGTLink_ARGS
            -DPLUS_USE_OpenIGTLink:BOOL=${PLUSBUILD_USE_OpenIGTLink}
            -DOpenIGTLink_DIR:PATH=${OpenIGTLink_DIR}
        )
ELSE()
    SET(PLUSBUILD_OpenIGTLink_ARGS
            -DPLUS_USE_OpenIGTLink:BOOL=${PLUSBUILD_USE_OpenIGTLink}
        )
ENDIF ()

SET(PLUSBUILD_PLTools_ARGS)
IF(PLUSBUILD_USE_PLTools)
    SET(PLUSBUILD_PLTools_ARGS
            -DPLUS_USE_PLTools:BOOL=${PLUSBUILD_USE_PLTools}
            -DPLTOOLS_DIR:PATH=${PLTOOLS_DIR}
    )
ELSE()
     SET(PLUSBUILD_PLTools_ARGS
            -DPLUS_USE_PLTools:BOOL=${PLUSBUILD_USE_PLTools}
    )
ENDIF()

# --------------------------------------------------------------------------
# PlusLib
SET (PLUS_PLUSLIB_DIR ${CMAKE_BINARY_DIR}/PlusLib CACHE INTERNAL "Path to store PlusLib contents.")
ExternalProject_Add(PlusLib
            TMP_DIR "${PLUS_PLUSLIB_DIR}/tmp"
            #--Download step--------------
            DOWNLOAD_DIR "${PLUS_PLUSLIB_DIR}/download"
            SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
            SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
            SVN_REPOSITORY https://subversion.assembla.com/svn/plus/trunk/PlusLib
            #--Configure step-------------
            SOURCE_DIR "${PLUS_PLUSLIB_DIR}/src"
            CMAKE_ARGS 
                -DVTK_DIR:PATH=${VTK_DIR}
                -DITK_DIR:PATH=${ITK_DIR}
                -DSubversion_SVN_EXECUTABLE:FILEPATH=${Subversion_SVN_EXECUTABLE}
                ${PLUSBUILD_OpenIGTLink_ARGS}
                ${PLUSBUILD_PLTools_ARGS}
                
            INSTALL_COMMAND ""
            STAMP_DIR "${PLUS_PLUSLIB_DIR}/stamp"
            BINARY_DIR "${PLUS_PLUSLIB_DIR}/bin"
            INSTALL_DIR "${PLUS_PLUSLIB_DIR}/bin"
            DEPENDS ${PlusLib_DEPENDENCIES}
            )
SET(PLUS_DIR ${PLUS_PLUSLIB_DIR}/bin CACHE PATH "The directory containing PLTools trunk repository" FORCE)                

SET (PLUS_PLTOOLS_DIR ${CMAKE_BINARY_DIR}/PLTools CACHE INTERNAL "Path to store PLTools contents.")

ExternalProject_Add( PLTools
            TMP_DIR "${PLUS_PLTOOLS_DIR}/tmp"
            #--Download step--------------
            DOWNLOAD_DIR "${PLUS_PLTOOLS_DIR}/download"
            SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
            SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
            SVN_REPOSITORY https://subversion.assembla.com/svn/pltools/trunk
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            STAMP_DIR "${PLUS_PLTOOLS_DIR}/stamp"
            SOURCE_DIR "${PLUS_PLTOOLS_DIR}/src"
            BINARY_DIR "${PLUS_PLTOOLS_DIR}/bin"
            INSTALL_DIR "${PLUS_PLTOOLS_DIR}/bin"
            )
            
SET(PLTOOLS_DIR ${PLUS_PLTOOLS_DIR}/src CACHE PATH "The directory containing PLTools trunk repository" FORCE)                

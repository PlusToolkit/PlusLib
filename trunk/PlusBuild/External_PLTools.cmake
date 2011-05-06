SET (PLUS_PLTOOLS_DIR ${CMAKE_BINARY_DIR}/PLTools CACHE INTERNAL "Path to store PLTools contents.")

ExternalProject_Add( PLTools
            SOURCE_DIR "${PLUS_PLTOOLS_DIR}" 
            #BINARY_DIR "PLTools-bin"
            #--Download step--------------
            SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
            SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
            SVN_REPOSITORY https://subversion.assembla.com/svn/pltools/trunk
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            )
            
SET(PLTOOLS_DIR ${PLUS_PLTOOLS_DIR} CACHE PATH "The directory containing PLTools trunk repository" FORCE)                

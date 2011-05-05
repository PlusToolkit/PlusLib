
# --------------------------------------------------------------------------
# OpenIGTLink 
SET (PLUS_OPENIGTLINK_DIR ${CMAKE_BINARY_DIR}/OpenIGTLink CACHE INTERNAL "Path to store OpenIGTLink sources.")
ExternalProject_Add( OpenIGTLink
            TMP_DIR "${PLUS_OPENIGTLINK_DIR}/tmp"
            #--Download step--------------
            DOWNLOAD_DIR "${PLUS_OPENIGTLINK_DIR}/download"
            SVN_REPOSITORY http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/
            #--Configure step-------------
            SOURCE_DIR "${PLUS_OPENIGTLINK_DIR}/src" 
            CMAKE_ARGS 
                -DBUILD_SHARED_LIBS:BOOL=ON 
                -DBUILD_TESTING:BOOL=OFF 
                -DBUILD_EXAMPLES:BOOL=OFF
                -DOpenIGTLink_DIR:FILEPATH=${PLUS_OPENIGTLINK_DIR}/bin
                -DOpenIGTLink_PROTOCOL_VERSION_2:BOOL=ON
                -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
                -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
            #--Build step-----------------
            BINARY_DIR "${PLUS_OPENIGTLINK_DIR}/bin"
            INSTALL_DIR "${PLUS_OPENIGTLINK_DIR}/bin"
            STAMP_DIR "${PLUS_OPENIGTLINK_DIR}/stamp"
            #--Install step-----------------
            INSTALL_COMMAND ""
            DEPENDS ${OpenIGTLink_DEPENDENCIES}
            )
SET(OpenIGTLink_DIR ${PLUS_OPENIGTLINK_DIR}/bin CACHE PATH "The directory containing a CMake configuration file for OpenIGTLink" FORCE)

# --------------------------------------------------------------------------
# PlusApp
SET(PLUSBUILD_SVN_REVISION_ARGS)
IF ( NOT PLUS_SVN_REVISION STREQUAL "0" )
    SET(PLUSBUILD_SVN_REVISION_ARGS 
        SVN_REVISION -r "${PLUS_SVN_REVISION}"
        )
ENDIF() 

SET (PLUS_PLUSAPP_DIR ${CMAKE_BINARY_DIR}/PlusApp CACHE INTERNAL "Path to store PlusApp contents.")
ExternalProject_Add(PlusApp
            SOURCE_DIR "${PLUS_PLUSAPP_DIR}" 
            BINARY_DIR "PlusApp-bin"
            #--Download step--------------
            SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
            SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
            SVN_REPOSITORY https://subversion.assembla.com/svn/plus/trunk/PlusApp
            ${PLUSBUILD_SVN_REVISION_ARGS}
            #--Configure step-------------
            CMAKE_ARGS 
                ${ep_common_args}
                -DPLUS_EXECUTABLE_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
                -DPLUSLIB_DIR:PATH=${PLUSLIB_DIR}
                -DSubversion_SVN_EXECUTABLE:FILEPATH=${Subversion_SVN_EXECUTABLE}
                -DPLUSAPP_BUILD_DiagnosticTools:BOOL=ON
                -DPLUSAPP_BUILD_fCal:BOOL=ON
                -DPLUSAPP_TEST_GUI:BOOL=${PLUSAPP_TEST_GUI}
                -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
                -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
                -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
            #--Build step-----------------
            #--Install step-----------------
            INSTALL_COMMAND ""
            DEPENDS ${PlusApp_DEPENDENCIES}
            )
SET(PLUSAPP_DIR ${CMAKE_BINARY_DIR}/PlusApp-bin CACHE PATH "The directory containing PlusApp binaries" FORCE)                

# --------------------------------------------------------------------------
# Copy Qt binaries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY "${QT_BINARY_DIR}/"
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
        FILES_MATCHING REGEX .*[^d]4${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    FILE(COPY "${QT_BINARY_DIR}/"
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
        FILES_MATCHING REGEX .*d4${CMAKE_SHARED_LIBRARY_SUFFIX}
        )    
ELSE()
    FILE(COPY "${QT_BINARY_DIR}/"
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )        
ENDIF()

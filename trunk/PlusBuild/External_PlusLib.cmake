
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
            SOURCE_DIR "${PLUS_PLUSLIB_DIR}" 
            BINARY_DIR "PlusLib-bin"
            #--Download step--------------
            SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
            SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
            SVN_REPOSITORY https://subversion.assembla.com/svn/plus/trunk/PlusLib
            #--Configure step-------------
            CMAKE_ARGS 
                -DVTK_DIR:PATH=${VTK_DIR}
                -DITK_DIR:PATH=${ITK_DIR}
                -DSubversion_SVN_EXECUTABLE:FILEPATH=${Subversion_SVN_EXECUTABLE}
                ${PLUSBUILD_OpenIGTLink_ARGS}
                ${PLUSBUILD_PLTools_ARGS}
                -DPLUS_USE_SONIX_VIDEO:BOOL=${PLUS_USE_SONIX_VIDEO}
                -DPLUS_USE_ICCAPTURING_VIDEO:BOOL=${PLUS_USE_ICCAPTURING_VIDEO}
                -DPLUS_USE_VFW_VIDEO:BOOL=${PLUS_USE_VFW_VIDEO}
                -DPLUS_USE_POLARIS:BOOL=${PLUS_USE_POLARIS}
                -DPLUS_USE_CERTUS:BOOL=${PLUS_USE_CERTUS}
                -DPLUS_USE_FLOCK:BOOL=${PLUS_USE_FLOCK}
                -DPLUS_USE_MICRONTRACKER:BOOL=${PLUS_USE_MICRONTRACKER}
                -DPLUS_USE_AMS_TRACKER:BOOL=${PLUS_USE_AMS_TRACKER}
                -DPLUS_USE_Ascension3DG:BOOL=${PLUS_USE_Ascension3DG}
                -DPLUS_USE_HEARTSIGNALBOX:BOOL=${PLUS_USE_HEARTSIGNALBOX}
                -DPLUS_USE_USBECGBOX:BOOL=${PLUS_USE_USBECGBOX}
                -DQT_BINARY_DIR:PATH=${QT_BINARY_DIR}
                -DULTERIUS_MAJOR_VERSION=${PLUS_ULTERIUS_MAJOR_VERSION}
                -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
                -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
            #--Build step-----------------
            #--Install step-----------------
            INSTALL_COMMAND ""
            DEPENDS ${PlusLib_DEPENDENCIES}
            )
SET(PLUSLIB_DIR ${CMAKE_BINARY_DIR}/PlusLib-bin CACHE PATH "The directory containing PlusLib binaries" FORCE)                

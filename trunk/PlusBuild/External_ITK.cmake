
# --------------------------------------------------------------------------
# ITK 
SET (PLUS_ITK_DIR ${CMAKE_BINARY_DIR}/itk CACHE INTERNAL "Path to store itk sources.")
ExternalProject_Add( itk
            TMP_DIR "${PLUS_ITK_DIR}/tmp"
            #--Download step--------------
            DOWNLOAD_DIR "${PLUS_ITK_DIR}/download"
            GIT_REPOSITORY git://itk.org/ITK.git
            GIT_TAG "v3.16.0"
            #SVN_REPOSITORY http://svn.github.com/Kitware/ITK.git/
            #--Configure step-------------
            SOURCE_DIR "${PLUS_ITK_DIR}/src" 
            CMAKE_ARGS 
                -DBUILD_SHARED_LIBS:BOOL=ON 
                -DBUILD_TESTING:BOOL=OFF 
                -DBUILD_EXAMPLES:BOOL=OFF
                -DITK_LEGACY_REMOVE:BOOL=ON
                -DKWSYS_USE_MD5:BOOL=ON
                -DITK_USE_REVIEW:BOOL=ON
                -DITK_USE_REVIEW_STATISTICS:BOOL=ON
                -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
                -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
                -DITK_USE_OPTIMIZED_REGISTRATION_METHODS:BOOL=ON
                -DITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY:BOOL=ON
                -DITK_USE_TRANSFORM_IO_FACTORIES:BOOL=ON
                -DITK_LEGACY_REMOVE:BOOL=ON
                -DKWSYS_USE_MD5:BOOL=ON
            #--Build step-----------------
            BINARY_DIR "${PLUS_ITK_DIR}/bin"
            INSTALL_DIR "${PLUS_ITK_DIR}/bin"
            STAMP_DIR "${PLUS_ITK_DIR}/stamp"
            #--Install step-----------------
            INSTALL_COMMAND ""
            DEPENDS ${ITK_DEPENDENCIES}
            )

SET(ITK_DIR ${PLUS_ITK_DIR}/bin CACHE PATH "The directory containing a CMake configuration file for ITK" FORCE)

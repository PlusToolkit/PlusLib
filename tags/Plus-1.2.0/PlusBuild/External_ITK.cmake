
# --------------------------------------------------------------------------
# ITK 
SET (PLUS_ITK_DIR ${CMAKE_BINARY_DIR}/itk CACHE INTERNAL "Path to store itk sources.")
ExternalProject_Add( itk
            SOURCE_DIR "${PLUS_ITK_DIR}" 
            BINARY_DIR "itk-bin"
            #--Download step--------------
            GIT_REPOSITORY "${GIT_PROTOCOL}://itk.org/ITK.git"
            GIT_TAG "v3.16.0"
            #SVN_REPOSITORY http://svn.github.com/Kitware/ITK.git/
            #--Configure step-------------
            CMAKE_ARGS 
                -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS} 
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
            #--Install step-----------------
            INSTALL_COMMAND ""
            DEPENDS ${ITK_DEPENDENCIES}
            )

SET(ITK_DIR ${CMAKE_BINARY_DIR}/itk-bin CACHE PATH "The directory containing a CMake configuration file for ITK" FORCE)

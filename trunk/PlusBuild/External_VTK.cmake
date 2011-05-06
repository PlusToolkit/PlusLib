# --------------------------------------------------------------------------
# VTK 
SET(VTK_QT_ARGS)
IF(NOT APPLE)
  SET(VTK_QT_ARGS
    #-DDESIRED_QT_VERSION:STRING=4 # Unused
    -DVTK_USE_GUISUPPORT:BOOL=ON
    -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
    -DVTK_USE_QT:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    )
ELSE()
  SET(VTK_QT_ARGS
    -DVTK_USE_CARBON:BOOL=OFF
    -DVTK_USE_COCOA:BOOL=ON # Default to Cocoa, VTK/CMakeLists.txt will enable Carbon and disable cocoa if needed
    -DVTK_USE_X:BOOL=OFF
    #-DVTK_USE_RPATH:BOOL=ON # Unused
    #-DDESIRED_QT_VERSION:STRING=4 # Unused
    -DVTK_USE_GUISUPPORT:BOOL=ON
    -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
    -DVTK_USE_QT:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    )
ENDIF()

SET (PLUS_VTK_DIR ${CMAKE_BINARY_DIR}/vtk CACHE INTERNAL "Path to store vtk sources.")
ExternalProject_Add( vtk
            SOURCE_DIR "${PLUS_VTK_DIR}" 
            BINARY_DIR "vtk-bin"
            #--Download step--------------
            #URL http://www.vtk.org/files/release/5.6/vtk-5.6.1.tar.gz
            GIT_REPOSITORY "${GIT_PROTOCOL}://vtk.org/VTK.git"
            GIT_TAG "v5.6.1"
            #--Configure step-------------
            CMAKE_ARGS 
                -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS} 
                -DBUILD_TESTING:BOOL=OFF 
                -DVTK_USE_PARALLEL:BOOL=ON
                -DBUILD_EXAMPLES:BOOL=OFF
                ${VTK_QT_ARGS}
                -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
                -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
            #--Build step-----------------
            #--Install step-----------------
            INSTALL_COMMAND ""
            DEPENDS ${VTK_DEPENDENCIES}
            )
SET(VTK_DIR ${CMAKE_BINARY_DIR}/vtk-bin CACHE PATH "The directory containing a CMake configuration file for VTK" FORCE)
SET(VTK_SOURCE_DIR ${PLUS_VTK_DIR})            

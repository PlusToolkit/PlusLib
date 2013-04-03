IF(VTK_DIR)

  # VTK has been built already
  FIND_PACKAGE(VTK REQUIRED PATHS ${VTK_DIR} NO_DEFAULT_PATH)
  
  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY 
      ${VTK_LIBRARY_DIRS}/Release/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY 
      ${VTK_LIBRARY_DIRS}/Debug/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )    
  ELSE()
      FILE(COPY 
        ${VTK_LIBRARY_DIRS}/
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
  ENDIF()
  SET (PLUS_VTK_DIR "${VTK_DIR}" CACHE INTERNAL "Path to store vtk binaries")
    
ELSE(VTK_DIR)

  # VTK has not been built yet, so download and build it as an external project

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

  SET (PLUS_VTK_SRC_DIR "${CMAKE_BINARY_DIR}/vtk")
  SET (PLUS_VTK_DIR "${CMAKE_BINARY_DIR}/vtk-bin" CACHE INTERNAL "Path to store vtk binaries")
  ExternalProject_Add( vtk
    SOURCE_DIR "${PLUS_VTK_SRC_DIR}"
    BINARY_DIR "${PLUS_VTK_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://vtk.org/VTK.git"
    GIT_TAG "v5.10.1"
    #--Configure step-------------
    CMAKE_ARGS 
        -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
        -DEXECUTABLE_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
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

ENDIF(VTK_DIR)

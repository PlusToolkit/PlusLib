IF(VTK_DIR)

  # VTK has been built already
  FIND_PACKAGE(VTK REQUIRED PATHS ${VTK_DIR} NO_DEFAULT_PATH)
  
  MESSAGE(STATUS "Using VTK available at: ${VTK_DIR}")
  
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
  
  OPTION(PLUSBUILD_USE_VTK6 "Plus uses VTK6 instead of VTK5" ON)
  
  IF (PLUSBUILD_USE_VTK6)
    SET(VTK_GIT_REPOSITORY "github.com/Slicer/VTK.git")
    SET(VTK_GIT_TAG "95aea46c32ab995f3359cf6d9c5b9691e73e4ae9") #v6.2.0 from 2015-03-02
    SET(VTK_VERSION_SPECIFIC_ARGS
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DVTK_Group_Qt:BOOL=ON
      )
    IF(APPLE)
      SET(VTK_QT_ARGS ${VTK_VERSION_SPECIFIC_ARGS}
        -DVTK_USE_CARBON:BOOL=OFF
        -DVTK_USE_COCOA:BOOL=ON # Default to Cocoa, VTK/CMakeLists.txt will enable Carbon and disable cocoa if needed
        -DVTK_USE_X:BOOL=OFF
        )
    ENDIF(APPLE)
  ELSE()
    SET(VTK_GIT_REPOSITORY "vtk.org/VTK.git")
    SET(VTK_GIT_TAG "v5.10.1")
    SET(VTK_VERSION_SPECIFIC_ARGS
      -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DEXECUTABLE_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DVTK_USE_PARALLEL:BOOL=ON
      -DVTK_USE_GUISUPPORT:BOOL=ON
      -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
      -DVTK_USE_QT:BOOL=ON
      )
    IF(APPLE)
      SET(VTK_QT_ARGS ${VTK_VERSION_SPECIFIC_ARGS}
        -DVTK_USE_CARBON:BOOL=OFF
        -DVTK_USE_COCOA:BOOL=ON # Default to Cocoa, VTK/CMakeLists.txt will enable Carbon and disable cocoa if needed
        -DVTK_USE_X:BOOL=OFF
        )
    ENDIF(APPLE)
  ENDIF()

  SET (PLUS_VTK_SRC_DIR "${CMAKE_BINARY_DIR}/vtk")
  SET (PLUS_VTK_DIR "${CMAKE_BINARY_DIR}/vtk-bin" CACHE INTERNAL "Path to store vtk binaries")
  ExternalProject_Add( vtk
    SOURCE_DIR "${PLUS_VTK_SRC_DIR}"
    BINARY_DIR "${PLUS_VTK_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://${VTK_GIT_REPOSITORY}"
    GIT_TAG ${VTK_GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        ${VTK_VERSION_SPECIFIC_ARGS}
        -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS} 
        -DBUILD_TESTING:BOOL=OFF 
        -DBUILD_EXAMPLES:BOOL=OFF
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DVTK_QT_VERSION:STRING=${QT_VERSION_MAJOR}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${VTK_DEPENDENCIES}
    )

ENDIF(VTK_DIR)

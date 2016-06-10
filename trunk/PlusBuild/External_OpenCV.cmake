IF(OpenCV_DIR)

  # OpenCV has been built already
  FIND_PACKAGE(OpenCV 3.1 REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

  MESSAGE(STATUS "Using OpenCV available at: ${OpenCV_DIR}")
  
  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    FILE(COPY 
      ${OpenCV_LIBRARY_DIRS}/Release/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY 
      ${OpenCV_LIBRARY_DIRS}/Debug/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )    
  ELSE()
      FILE(COPY 
        ${OpenCV_LIBRARY_DIRS}/
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
  ENDIF()
  SET (PLUS_OpenCV_DIR "${OpenCV_DIR}" CACHE INTERNAL "Path to store OpenCV binaries")
  
ELSE(OpenCV_DIR)
  SET(EXTRA_OPENCV_ARGS)
  FIND_PACKAGE(CUDA 7.5 QUIET)
  
  IF( NOT CUDA_FOUND )
    SET(EXTRA_OPENCV_ARGS -DWITH_CUDA:BOOL=OFF)
  ENDIF()

  # OpenCV has not been built yet, so download and build it as an external project
  SET (PLUS_OpenCV_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/OpenCV")
  SET (PLUS_OpenCV_DIR "${CMAKE_BINARY_DIR}/Deps/OpenCV-bin" CACHE INTERNAL "Path to store OpenCV binaries")
  ExternalProject_Add( OpenCV
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    PREFIX "${CMAKE_BINARY_DIR}/Deps/OpenCV-prefix"
    SOURCE_DIR "${PLUS_OpenCV_SRC_DIR}"
    BINARY_DIR "${PLUS_OpenCV_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/Itseez/opencv.git"
    GIT_TAG "3.1.0"
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
        -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS}
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_DOCS:BOOL=OFF
        -DVTK_DIR:PATH=${PLUS_VTK_DIR} 
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        ${EXTRA_OPENCV_ARGS}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${OpenCV_DEPENDENCIES}
    )  
    
ENDIF(OpenCV_DIR)
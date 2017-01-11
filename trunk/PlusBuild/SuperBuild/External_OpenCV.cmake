IF(OpenCV_DIR)
  FIND_PACKAGE(OpenCV 3.1.0 REQUIRED NO_MODULE)

  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  FOREACH(lib ${OpenCV_LIBS})
    IF(NOT TARGET ${lib})
      continue()
    ENDIF()

    GET_TARGET_PROPERTY(DEBUG_FILE ${lib} IMPORTED_LOCATION_DEBUG)
    GET_TARGET_PROPERTY(RELEASE_FILE ${lib} IMPORTED_LOCATION_RELEASE)

    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      IF(EXISTS ${RELEASE_FILE})
        FILE(COPY ${RELEASE_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release)
      ENDIF()
      IF(EXISTS ${DEBUG_FILE})
        FILE(COPY ${DEBUG_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug)
      ENDIF()
    ELSE()
      IF(DEBUG_FILE EQUAL RELEASE_FILE AND EXISTS ${RELEASE_FILE})
        FILE(COPY ${RELEASE_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH})
      ELSE()
        IF(EXISTS ${RELEASE_FILE})
          FILE(COPY ${RELEASE_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH})
        ENDIF()
        IF(EXISTS ${DEBUG_FILE})
          FILE(COPY ${DEBUG_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH})
        ENDIF()
      ENDIF()
    ENDIF()
  ENDFOREACH()

  SET(PLUS_OpenCV_DIR ${OpenCV_DIR} CACHE INTERNAL "Path to store OpenCV binaries")
ELSE()
  # No OpenCV is specified, so download and build
  SET(OpenCV_REPOSITORY https://github.com/opencv/opencv.git)

  FIND_PACKAGE(CUDA QUIET)
  
  SET(OpenCV_PLATFORM_SPECIFIC_ARGS)
  SET(_cuda OFF)
  IF(CUDA_FOUND)
    IF(MSVC AND NOT "${CMAKE_GENERATOR}" MATCHES "Win64")
      # CUDA 32 bit is only available on versions <= 6.5
      IF(NOT ${CUDA_VERSION} VERSION_GREATER "6.5")
        SET(_cuda ON)
        LIST(APPEND OpenCV_PLATFORM_SPECIFIC_ARGS -DCUDA_TOOLKIT_ROOT_DIR:PATH=${CUDA_TOOLKIT_ROOT_DIR})
      ENDIF()
    ELSE()
      SET(_cuda ON)
      LIST(APPEND OpenCV_PLATFORM_SPECIFIC_ARGS -DCUDA_TOOLKIT_ROOT_DIR:PATH=${CUDA_TOOLKIT_ROOT_DIR})
    ENDIF()
    
    IF(_cuda)
      LIST(APPEND OpenCV_PLATFORM_SPECIFIC_ARGS -DWITH_CUDA:BOOL=ON)
    ENDIF()
    
    SET(_generations "Fermi" "Kepler" "Maxwell")
    IF(${CUDA_VERSION} VERSION_GREATER 7)
      LIST(APPEND _generations "Pascal")
    ENDIF()
    IF(NOT CMAKE_CROSSCOMPILING)
      LIST(APPEND _generations "Auto")
    ENDIF()

    SET(PLUSBUILD_OpenCV_CUDA_GENERATION "" CACHE STRING "Build CUDA device code only for specific GPU architecture. Leave empty to build for all architectures.")
    set_property( CACHE PLUSBUILD_OpenCV_CUDA_GENERATION PROPERTY STRINGS "" ${_generations} )

    IF(PLUSBUILD_OpenCV_CUDA_GENERATION)
      IF(NOT ";${_generations};" MATCHES ";${PLUSBUILD_OpenCV_CUDA_GENERATION};")
        STRING(REPLACE ";" ", " _generations "${_generations}")
        MESSAGE(FATAL_ERROR "ERROR: Only CUDA ${_generations} generations are supported.")
      ENDIF()
    ENDIF()
    
    LIST(APPEND OpenCV_PLATFORM_SPECIFIC_ARGS -DCUDA_GENERATION:STRING=${PLUSBUILD_OpenCV_CUDA_GENERATION})
  ELSE()
    LIST(APPEND OpenCV_PLATFORM_SPECIFIC_ARGS -DWITH_CUDA:BOOL=OFF)
  ENDIF()
  
  IF(Qt5_FOUND)
    LIST(APPEND OpenCV_PLATFORM_SPECIFIC_ARGS -DWITH_QT:BOOL=ON -DQt5_DIR:PATH=${Qt5_DIR})
  ENDIF()

  MESSAGE(STATUS "Downloading OpenCV from: ${OpenCV_REPOSITORY}")

  SET (PLUS_OpenCV_src_DIR ${CMAKE_BINARY_DIR}/Deps/OpenCV CACHE INTERNAL "Path to store OpenCV contents.")
  SET (PLUS_OpenCV_prefix_DIR ${CMAKE_BINARY_DIR}/Deps/OpenCV-prefix CACHE INTERNAL "Path to store OpenCV prefix data.")
  SET (PLUS_OpenCV_DIR ${CMAKE_BINARY_DIR}/Deps/OpenCV-bin CACHE INTERNAL "Path to store OpenCV binaries")
  ExternalProject_Add( OpenCV
    PREFIX ${PLUS_OpenCV_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_OpenCV_src_DIR}"
    BINARY_DIR "${PLUS_OpenCV_DIR}"
    #--Download step--------------
    GIT_REPOSITORY ${OpenCV_REPOSITORY}
    GIT_TAG 6c12533160bc5e6de1f09847d9cd6dd548667a55 # 3.1.0 with many fixes
    #--Configure step-------------
    CMAKE_ARGS
      ${ep_common_args}
      ${OpenCV_PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      #-DVTK_DIR:PATH=${PLUS_VTK_DIR} re-enable OpenCV-VTK7 support
      -DWITH_VTK:BOOL=OFF
      -DBUILD_TESTS:BOOL=OFF
      -DBUILD_PERF_TESTS:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS}
    #--Build step-----------------
    BUILD_ALWAYS 1
    #--Install step-----------------
    INSTALL_COMMAND "" # Do not install, we have access to ${PLUS_OpenCV_DIR}/OpenCVConfig.cmake
    #--Dependencies-----------------
    #DEPENDS vtk RE-ADD with OpenCV-VTK7 support
    )
ENDIF()
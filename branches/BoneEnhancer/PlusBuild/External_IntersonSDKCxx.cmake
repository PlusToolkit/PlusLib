IF(IntersonSDKCxx_DIR)

  # IntersonSDKCxx has been built already
  FIND_PACKAGE(IntersonSDKCxx REQUIRED PATHS ${IntersonSDKCxx_DIR} NO_DEFAULT_PATH)
  
  MESSAGE(STATUS "Using IntersonSDKCxx available at: ${IntersonSDKCxx_DIR}")

  SET( _config )
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    SET( _config "/Release" )
  ENDIF()
  FOREACH(_libpath ${IntersonSDKCxx_RUNTIME_LIBRARY_DIRS})
    IF( EXISTS "${_libpath}/IntersonCxx.${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      FILE(COPY "${_libpath}"
        DESTINATION "${PLUS_EXECUTABLE_OUTPUT_PATH}${_config}"
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
  ENDFOREACH()
  SET (PLUS_IntersonSDKCxx_DIR "${IntersonSDKCxx_DIR}" CACHE INTERNAL "Path to store IntersonSDKCxx binaries")
  
ELSE(IntersonSDKCxx_DIR)

  # IntersonSDKCxx has not been built yet, so download and build it as an external project
  SET (IntersonSDKCxx_REPOSITORY ${GIT_PROTOCOL}://github.com/KitwareMedical/IntersonSDKCxx.git)
  SET (IntersonSDKCxx_GIT_TAG 819d620052be7e9b232e12d8946793c15cfbf5a3)

  SET (PLUS_IntersonSDKCxx_SRC_DIR "${CMAKE_BINARY_DIR}/IntersonSDKCxx")
  SET (PLUS_IntersonSDKCxx_DIR "${CMAKE_BINARY_DIR}/IntersonSDKCxx-bin" CACHE INTERNAL "Path to store IntersonSDKCxx binaries")
  ExternalProject_Add( IntersonSDKCxx
    SOURCE_DIR "${PLUS_IntersonSDKCxx_SRC_DIR}"
    BINARY_DIR "${PLUS_IntersonSDKCxx_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${IntersonSDKCxx_REPOSITORY}"
    GIT_TAG "${IntersonSDKCxx_GIT_TAG}"
    #--Configure step-------------
    CMAKE_ARGS 
      ${ep_common_args}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS} 
      -DBUILD_TESTING:BOOL=OFF
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${IntersonSDKCxx_DEPENDENCIES}
    )

ENDIF(IntersonSDKCxx_DIR)

IF(OvrvisionPro_DIR)
  FIND_PACKAGE(OvrvisionPro REQUIRED NO_MODULE)
  
  SET(PLUS_OvrvisionPro_DIR ${OvrvisionPro_DIR} CACHE INTERNAL "Path to store OvrvisionPro binaries")
ELSE()
  IF(NOT OpenCV_FOUND)
    # We are building our own OpenCV, make sure the dependency order is set
    SET(OvrvisionPro_DEPENDENCIES OpenCV)
  ELSE()
    SET(OvrvisionPro_DEPENDENCIES)
  ENDIF()

  SET(OvrvisionPro_REPOSITORY https://github.com/PLUSToolkit/OvrvisionProCMake.git)

  MESSAGE(STATUS "Downloading OvrvisionPro SDK from: ${OvrvisionPro_REPOSITORY}")

  # --------------------------------------------------------------------------
  # OvrvisionPro SDK
  SET (PLUS_OvrvisionPro_src_DIR ${CMAKE_BINARY_DIR}/Deps/OvrvisionPro CACHE INTERNAL "Path to store OvrvisionPro contents.")
  SET (PLUS_OvrvisionPro_prefix_DIR ${CMAKE_BINARY_DIR}/Deps/OvrvisionPro-prefix CACHE INTERNAL "Path to store OvrvisionPro prefix data.")
  SET (PLUS_OvrvisionPro_DIR ${CMAKE_BINARY_DIR}/Deps/OvrvisionPro-bin CACHE INTERNAL "Path to store OvrvisionPro binaries")
  
  # Since OvrvisionPro SDK uses #pragma comment(lib...) commands, we need to pass in the directories containing the requested libraries directly...
  #   ippicvmt.lib
  IF( MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64" )
    SET(OvrvisionPro_PRAGMA_HACK -DPragmaHack_DIR:PATH=${PLUS_OpenCV_src_DIR}/3rdparty/ippicv/unpack/ippicv_win/lib/intel64)
  ELSEIF(MSVC)
    SET(OvrvisionPro_PRAGMA_HACK -DPragmaHack_DIR:PATH=${PLUS_OpenCV_src_DIR}/3rdparty/ippicv/unpack/ippicv_win/lib/ia32)
  ELSEIF(APPLE)
    # Mac?
  ELSE()
    # Linux?
  ENDIF()

  ExternalProject_Add( OvrvisionPro
      PREFIX ${PLUS_OvrvisionPro_prefix_DIR}
      "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
      SOURCE_DIR "${PLUS_OvrvisionPro_src_DIR}"
      BINARY_DIR "${PLUS_OvrvisionPro_DIR}"
      #--Download step--------------
      GIT_REPOSITORY ${OvrvisionPro_REPOSITORY}
      GIT_TAG master
      #--Configure step-------------
      CMAKE_ARGS
          ${ep_common_args}
          ${OvrvisionPro_PRAGMA_HACK}
          -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
          -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
          -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
          -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
          -DOpenCV_DIR:PATH=${PLUS_OpenCV_DIR}
      #--Override install step-----------------
      INSTALL_COMMAND "" # Do not install
      #--Dependencies-----------------
      DEPENDS ${OvrvisionPro_DEPENDENCIES}
      )
ENDIF()
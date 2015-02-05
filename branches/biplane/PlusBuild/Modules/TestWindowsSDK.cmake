MESSAGE(STATUS "Checking if the latest Windows SDK is compatible (${WINDOWSSDK_LATEST_DIR})")

IF( ${CMAKE_GENERATOR} MATCHES Win64 )
  SET( MF_LIB_DIR ${WINDOWSSDK_LATEST_DIR}/Lib/x64 )
ELSE()
  SET( MF_LIB_DIR ${WINDOWSSDK_LATEST_DIR}/Lib )
ENDIF()  
TRY_COMPILE(PLUS_WINDOWS_SDK_IS_COMPATIBLE
            "${CMAKE_BINARY_DIR}/CMakeTmp/WindowsSdkCompatibility"
            "${CMAKE_CURRENT_SOURCE_DIR}/Modules/PlusTestWindowsSdkCompatible.cxx"              
            CMAKE_FLAGS
              "-DINCLUDE_DIRECTORIES:STRING=${WINDOWSSDK_LATEST_DIR}/Include"
              "-DLINK_LIBRARIES:STRING=${MF_LIB_DIR}/mfuuid.lib"
            OUTPUT_VARIABLE OUTPUT)
            
IF(PLUS_WINDOWS_SDK_IS_COMPATIBLE)
  MESSAGE(STATUS "Checking if the latest Windows SDK is compatible -- yes")
  SET(PLUS_WINDOWS_SDK_IS_COMPATIBLE 1 CACHE INTERNAL "Windows SDK is compatible with Plus")
  WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
    "Determining if the C++ compiler supports type bool "
    "passed with the following output:\n"
    "${OUTPUT}\n" APPEND)
ELSE(PLUS_WINDOWS_SDK_IS_COMPATIBLE)
  MESSAGE(STATUS "Checking if the latest Windows SDK is compatible -- no")
  SET(PLUS_WINDOWS_SDK_IS_COMPATIBLE 0 CACHE INTERNAL "Windows SDK is compatible with Plus")
  WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
    "Determining if the Windows SDK is compatible with Plus "
    "failed with the following output:\n"
    "${OUTPUT}\n" APPEND)
ENDIF(PLUS_WINDOWS_SDK_IS_COMPATIBLE)

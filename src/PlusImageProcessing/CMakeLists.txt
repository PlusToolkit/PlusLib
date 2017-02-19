IF(PLUS_USE_INTEL_MKL)
  FIND_PACKAGE( OpenMP REQUIRED)
  IF(OPENMP_FOUND)
    MESSAGE(STATUS "OpenMP found.")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  ENDIF()

  FIND_PATH(IntelComposerXEdir foldermap.sc.xml
    PATHS "C:/Program Files (x86)/Intel/Composer XE 2015/"
    DOC "A path to Intel Composer XE" )

  IF(NOT IntelComposerXEdir)
    MESSAGE(FATAL_ERROR "A path to Intel Composer XE is invalid.")
  ENDIF()
ENDIF()

# Sources
SET(PlusImageProcessing_SRCS
  vtkPlusTrackedFrameProcessor.cxx
  vtkPlusBoneEnhancer.cxx
  vtkPlusRfToBrightnessConvert.cxx
  vtkPlusUsScanConvert.cxx
  vtkPlusUsScanConvertLinear.cxx
  vtkPlusUsScanConvertCurvilinear.cxx
  vtkPlusRfProcessor.cxx
  vtkPlusTransverseProcessEnhancer.cxx
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(PlusImageProcessing_HDRS
    vtkPlusTrackedFrameProcessor.h
    vtkPlusBoneEnhancer.h
    vtkPlusRfToBrightnessConvert.h
    vtkPlusUsScanConvert.h
    vtkPlusUsScanConvertLinear.h
    vtkPlusUsScanConvertCurvilinear.h
    vtkPlusRfProcessor.h
    vtkPlusTransverseProcessEnhancer.h
    )
ENDIF()

SET(PlusImageProcessing_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

IF(PLUS_USE_INTEL_MKL)
  LIST(APPEND PlusImageProcessing_SRCS vtkPlusForoughiBoneSurfaceProbability.cxx)
  IF(WIN32)
    LIST(APPEND PlusImageProcessing_HDRS vtkPlusForoughiBoneSurfaceProbability.h)
  ENDIF()
  
  LIST(APPEND PlusImageProcessing_INCLUDE_DIRS "${IntelComposerXEdir}/mkl/include")
ENDIF()

# --------------------------------------------------------------------------
# Build the library
SET(External_Libraries_Install)

SET(PlusImageProcessing_LIBS
  vtkPlusCommon
  vtkImagingStatistics
  vtkImagingGeneral
  vtkImagingMorphological
  )

IF(PLUS_USE_INTEL_MKL)
  IF(${CMAKE_GENERATOR} MATCHES Win64)
    SET(MKL_LIBS
      "${IntelComposerXEdir}/mkl/lib/intel64/mkl_intel_lp64_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/intel64/mkl_intel_thread_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/intel64/mkl_core_dll.lib"
      "${IntelComposerXEdir}/compiler/lib/intel64/libiomp5md.lib"
      )
    SET(REDIST_DIR "${IntelComposerXEdir}/redist/intel64/mkl")
    LIST(APPEND External_Libraries_Install
      "${REDIST_DIR}/libimalloc${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_avx${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_avx2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_avx512${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_core${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_def${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_intel_thread${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_mc${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_mc3${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_rt${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_sequential${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_avx${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_avx2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_avx512${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_cmpt${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_def${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_mc${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_mc2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_mc3${CMAKE_SHARED_LIBRARY_SUFFIX}"
      )
  ELSE()
    SET(MKL_LIBS
      "${IntelComposerXEdir}/mkl/lib/ia32/mkl_intel_c_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/ia32/mkl_intel_thread_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/ia32/mkl_core_dll.lib"
      )
    SET(REDIST_DIR "${IntelComposerXEdir}/redist/ia32/mkl")
    LIST(APPEND External_Libraries_Install
      "${REDIST_DIR}/libimalloc${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_avx${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_avx2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_avx512${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_blacs${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_blacs_intelmpi${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_blacs_mpich2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_cdft_core${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_core${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_intel_thread${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_p4${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_p4m${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_p4m3${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_pgi_thread${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_rt${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_scalapack_core${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_sequential${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_avx${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_avx2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_avx512${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_cmpt${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_ia${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_p4${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_p4m${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_p4m2${CMAKE_SHARED_LIBRARY_SUFFIX}"
      "${REDIST_DIR}/mkl_vml_p4m3${CMAKE_SHARED_LIBRARY_SUFFIX}"
      )
  ENDIF()

  LIST(APPEND PlusImageProcessing_LIBS ${MKL_LIBS})
ENDIF()

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusImageProcessing)
ADD_LIBRARY(vtkPlusImageProcessing ${PlusImageProcessing_SRCS} ${PlusImageProcessing_HDRS})
FOREACH(p IN LISTS PlusImageProcessing_INCLUDE_DIRS)
  target_include_directories(vtkPlusImageProcessing PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtkPlusImageProcessing PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtkPlusImageProcessing ${PlusImageProcessing_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusImageProcessing CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusImageProcessing vtkPlusImageProcessing;${PlusLib_BINARY_DIR}/src/PlusImageProcessing/vtkPlusImageProcessing.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
    )
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
    )
ELSE()
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
    )
ENDIF()

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #---------------------------------------------------------------------------
  ADD_EXECUTABLE(RfProcessor Tools/RfProcessor.cxx )
  SET_TARGET_PROPERTIES(RfProcessor PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(RfProcessor vtkPlusImageProcessing )
  GENERATE_HELP_DOC(RfProcessor)

  #---------------------------------------------------------------------------
  ADD_EXECUTABLE(DrawScanLines Tools/DrawScanLines.cxx )
  SET_TARGET_PROPERTIES(DrawScanLines PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(DrawScanLines vtkPlusImageProcessing vtkPlusCommon)
  GENERATE_HELP_DOC(DrawScanLines)

  #---------------------------------------------------------------------------
  ADD_EXECUTABLE(ExtractScanLines Tools/ExtractScanLines.cxx)
  SET_TARGET_PROPERTIES(ExtractScanLines PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(ExtractScanLines vtkPlusImageProcessing)
  GENERATE_HELP_DOC(ExtractScanLines)

  #---------------------------------------------------------------------------
  ADD_EXECUTABLE(ScanConvert Tools/ScanConvert.cxx)
  SET_TARGET_PROPERTIES(ScanConvert PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(ScanConvert vtkPlusImageProcessing)
  GENERATE_HELP_DOC(ScanConvert)

  #---------------------------------------------------------------------------
  IF(PLUS_USE_INTEL_MKL)
    ADD_EXECUTABLE(EnhanceBone Tools/EnhanceBone.cxx )
  SET_TARGET_PROPERTIES(EnhanceBone PROPERTIES FOLDER Tools)
    TARGET_LINK_LIBRARIES(EnhanceBone vtkPlusImageProcessing )
    GENERATE_HELP_DOC(EnhanceBone)
  ENDIF()

  # --------------------------------------------------------------------------
  SET(_install_targets
    RfProcessor
    DrawScanLines
    ExtractScanLines
    ScanConvert
    )
  IF(PLUS_USE_INTEL_MKL)
    LIST(APPEND ${_install_targets} EnhanceBone)
  ENDIF()

  INSTALL(TARGETS ${_install_targets} EXPORT PlusLib
    RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusImageProcessing EXPORT PlusLib
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusImageProcessing_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
  )

  # --------------------------------------------------------------------------
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()
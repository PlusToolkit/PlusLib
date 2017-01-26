IF (PLUS_USE_INTEL_MKL)
  FIND_PACKAGE( OpenMP REQUIRED)
  if(OPENMP_FOUND)
    message("OPENMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  endif()

  find_path(IntelComposerXEdir foldermap.sc.xml
    PATHS "C:/Program Files (x86)/Intel/Composer XE 2015/"
    DOC "A path to Intel Composer XE" )

  if (NOT IntelComposerXEdir)
    message( FATAL_ERROR "A path to Intel Composer XE is invalid." )
  endif()
ENDIF()

# Sources
SET (PlusImageProcessing_SRCS
    vtkPlusTrackedFrameProcessor.cxx
    vtkPlusBoneEnhancer.cxx
    vtkPlusRfToBrightnessConvert.cxx
    vtkPlusUsScanConvert.cxx
    vtkPlusUsScanConvertLinear.cxx
    vtkPlusUsScanConvertCurvilinear.cxx
    vtkPlusRfProcessor.cxx
    vtkPlusTransverseProcessEnhancer.cxx
    )

SET (PlusImageProcessing_HDRS)
if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET (PlusImageProcessing_HDRS
    vtkPlusTrackedFrameProcessor.h
    vtkPlusBoneEnhancer.h
    vtkPlusRfToBrightnessConvert.h
    vtkPlusUsScanConvert.h
    vtkPlusUsScanConvertLinear.h
    vtkPlusUsScanConvertCurvilinear.h
    vtkPlusRfProcessor.h
    vtkPlusTransverseProcessEnhancer.h
    )
endif ()

SET (PlusImageProcessing_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

IF (PLUS_USE_INTEL_MKL)
  SET (PlusImageProcessing_SRCS ${PlusImageProcessing_SRCS}
      vtkPlusForoughiBoneSurfaceProbability.cxx
      )
  if (WIN32)
    SET (PlusImageProcessing_HDRS ${PlusImageProcessing_HDRS}
      vtkPlusForoughiBoneSurfaceProbability.h
      )
  endif (WIN32)
  SET (PlusImageProcessing_INCLUDE_DIRS ${PlusImageProcessing_INCLUDE_DIRS}
    "${IntelComposerXEdir}/mkl/include"
    CACHE INTERNAL "" FORCE)
ENDIF (PLUS_USE_INTEL_MKL)

# --------------------------------------------------------------------------
# Build the library
SET (External_Libraries_Install)

SET (PlusImageProcessing_LIBS
  vtkPlusCommon
  vtkImagingStatistics
  vtkImagingGeneral
  vtkImagingMorphological
  )

IF (PLUS_USE_INTEL_MKL)
  IF( ${CMAKE_GENERATOR} MATCHES Win64 )
    SET( MKL_LIBS
      "${IntelComposerXEdir}/mkl/lib/intel64/mkl_intel_lp64_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/intel64/mkl_intel_thread_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/intel64/mkl_core_dll.lib"
      "${IntelComposerXEdir}/compiler/lib/intel64/libiomp5md.lib"
      )
    SET ( REDIST_DIR "${IntelComposerXEdir}/redist/intel64/mkl")
    SET ( External_Libraries_Install ${External_Libraries_Install}
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
    SET( MKL_LIBS
      "${IntelComposerXEdir}/mkl/lib/ia32/mkl_intel_c_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/ia32/mkl_intel_thread_dll.lib"
      "${IntelComposerXEdir}/mkl/lib/ia32/mkl_core_dll.lib"
      )
    SET ( REDIST_DIR "${IntelComposerXEdir}/redist/ia32/mkl")
    SET( External_Libraries_Install ${External_Libraries_Install}
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

  SET (PlusImageProcessing_LIBS ${PlusImageProcessing_LIBS}
    ${MKL_LIBS}
    )
ENDIF (PLUS_USE_INTEL_MKL)

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusImageProcessing)
ADD_LIBRARY(vtkPlusImageProcessing ${PlusImageProcessing_SRCS} ${PlusImageProcessing_HDRS})
target_include_directories(vtkPlusImageProcessing PUBLIC ${PlusImageProcessing_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES(vtkPlusImageProcessing ${PlusImageProcessing_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusImageProcessing CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusImageProcessing vtkPlusImageProcessing;${PlusLib_BINARY_DIR}/src/PlusImageProcessing/vtkPlusImageProcessing.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
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
  IF (PLUS_USE_INTEL_MKL)
    ADD_EXECUTABLE(EnhanceBone Tools/EnhanceBone.cxx )
  SET_TARGET_PROPERTIES(EnhanceBone PROPERTIES FOLDER Tools)
    TARGET_LINK_LIBRARIES(EnhanceBone vtkPlusImageProcessing )
    GENERATE_HELP_DOC(EnhanceBone)
  ENDIF (PLUS_USE_INTEL_MKL)

  # --------------------------------------------------------------------------
  SET (INSTALL_TARGETS
    RfProcessor
    DrawScanLines
    ExtractScanLines
    ScanConvert
    )
  IF (PLUS_USE_INTEL_MKL)
    SET (INSTALL_TARGETS ${INSTALL_TARGETS} EnhanceBone )
  ENDIF (PLUS_USE_INTEL_MKL)

  INSTALL(
    TARGETS ${INSTALL_TARGETS}
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeExecutables
    )
ENDIF()

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS vtkPlusImageProcessing
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION lib CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib CONFIGURATIONS Release COMPONENT Development
  )
INSTALL(FILES ${PlusImageProcessing_HDRS}
  DESTINATION include
  )
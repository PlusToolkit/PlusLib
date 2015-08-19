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
SET (ImageProcessingAlgo_SRCS
    vtkTrackedFrameProcessor.cxx
    vtkBoneEnhancer.cxx
    vtkRfToBrightnessConvert.cxx
    vtkUsScanConvert.cxx 
    vtkUsScanConvertLinear.cxx 
    vtkUsScanConvertCurvilinear.cxx 
    vtkRfProcessor.cxx
    )

SET (ImageProcessingAlgo_HDRS)
if (WIN32)    
  SET (ImageProcessingAlgo_HDRS
    vtkTrackedFrameProcessor.h
    vtkBoneEnhancer.h
    vtkRfToBrightnessConvert.h 
    vtkUsScanConvert.h
    vtkUsScanConvertLinear.h 
    vtkUsScanConvertCurvilinear.h 
    vtkRfProcessor.h
    )
endif (WIN32)

SET (ImageProcessingAlgo_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

IF (PLUS_USE_INTEL_MKL)    
  SET (ImageProcessingAlgo_SRCS ${ImageProcessingAlgo_SRCS}
      vtkForoughiBoneSurfaceProbability.cxx
      )
  if (WIN32)    
    SET (ImageProcessingAlgo_HDRS ${ImageProcessingAlgo_HDRS}
      vtkForoughiBoneSurfaceProbability.h
      )
  endif (WIN32)
  SET (ImageProcessingAlgo_INCLUDE_DIRS ${ImageProcessingAlgo_INCLUDE_DIRS}
    "${IntelComposerXEdir}/mkl/include"
    CACHE INTERNAL "" FORCE)  
ENDIF (PLUS_USE_INTEL_MKL)
  
# --------------------------------------------------------------------------
# Build the library

SET (External_Libraries_Install)

SET (ImageProcessingAlgo_LIBS
  vtkPlusCommon
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
  
  SET (ImageProcessingAlgo_LIBS ${ImageProcessingAlgo_LIBS}
    ${MKL_LIBS}
    )
ENDIF (PLUS_USE_INTEL_MKL)

INCLUDE_DIRECTORIES( ${ImageProcessingAlgo_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(vtkImageProcessingAlgo)
ADD_LIBRARY(vtkImageProcessingAlgo ${ImageProcessingAlgo_SRCS} ${ImageProcessingAlgo_HDRS})
TARGET_LINK_LIBRARIES(vtkImageProcessingAlgo ${ImageProcessingAlgo_LIBS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkImageProcessingAlgo CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkImageProcessingAlgo vtkImageProcessingAlgo;"${PlusLib_BINARY_DIR}/src/ImageProcessingAlgo/vtkImageProcessingAlgo.vcproj";vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
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

# --------------------------------------------------------------------------
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF (BUILD_TESTING)
# --------------------------------------------------------------------------

# Install
#

INSTALL(TARGETS vtkImageProcessingAlgo
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION bin CONFIGURATIONS Release COMPONENT Development
  )

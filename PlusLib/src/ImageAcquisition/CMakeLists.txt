# --------------------------------------------------------------------------
# Options

OPTION (PLUS_USE_SONIX_VIDEO "Provide support for the SonixRP video source" OFF)
OPTION (PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION (PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)
SET (PLUS_ULTRASONIX_SDK_MAJOR_VERSION 5 CACHE STRING "Set Ultrasonix ULTRASONIX_SDK SDK major version")
SET (PLUS_ULTRASONIX_SDK_MINOR_VERSION 6 CACHE STRING "Set Ultrasonix ULTRASONIX_SDK SDK minor version")

# --------------------------------------------------------------------------
# Sources

SET (ImageAcquisition_SRCS
  vtkMetaImageSequenceIO.cxx 
  itkFcsvReader.cxx
  itkFcsvWriter.cxx
  vtkVideoBuffer.cxx 
  vtkPlusVideoSource.cxx 
  vtkTrackedFrameList.cxx 
  vtkSavedDataVideoSource.cxx
  PlusVideoFrame.cxx
  UsImageConverterCommon.cxx
  )
	
SET (ImageAcquisition_HDRS)
if (WIN32)	
	SET (ImageAcquisition_HDRS
    vtkMetaImageSequenceIO.h
    itkFcsvReader.h
    itkFcsvWriter.h
    vtkVideoBuffer.h 
    vtkPlusVideoSource.h 
    vtkTrackedFrameList.h
    vtkSavedDataVideoSource.h 
    PlusVideoFrame.h
    UsImageConverterCommon.h
    )
endif (WIN32)	

SET (ImageAcquisition_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)
SET (External_Libraries_Install "")

# --------------------------------------------------------------------------
# Ultrasonix support

# Macro for setting up Ultrasonix SDK (major version 5 or later)
# Ulterius and Porta libraries are supported.
macro(UltrasonixSdk SDK_MAJOR SDK_MINOR SDK_PATCH)
	SET(ULTRASONIX_SDK_DIR ${PLTOOLS_DIR}/Ultrasonix/sdk-${SDK_MAJOR}.${SDK_MINOR}.${SDK_PATCH} CACHE PATH "Path to the Ultrasonix SDK directory." FORCE) 			
	SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ultrasonix SDK binary directory." FORCE)
	SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/ulterius/inc CACHE PATH "Path to the Ultrasonix SDK Ulterius include directory." FORCE)
	SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/ulterius/lib CACHE PATH "Path to the Ultrasonix SDK Ulterius library directory." FORCE)
	SET(PORTA_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/porta/inc CACHE PATH "Path to the Ultrasonix SDK Porta include directory." FORCE)
	SET(PORTA_LIB_DIR ${ULTRASONIX_SDK_DIR}/porta/lib CACHE PATH "Path to the Ultrasonix SDK Porta library directory." FORCE)			      			
	SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
		${ULTERIUS_LIB_DIR}/ulterius.lib
		${PORTA_LIB_DIR}/porta.lib
		)
	SET ( External_Libraries_Install ${External_Libraries_Install}
	  ${ULTRASONIX_SDK_BINARY_DIR}/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/porta${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/ftd2xx${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/umc_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/umc_xml${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/umc_string_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/umc_unit_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ULTRASONIX_SDK_BINARY_DIR}/xerces-c_2_7_u_vc6${CMAKE_SHARED_LIBRARY_SUFFIX}
		)    
endmacro(UltrasonixSdk) 

IF (PLUS_USE_SONIX_VIDEO)

	SET (ImageAcquisition_SRCS
    ${ImageAcquisition_SRCS}
    SonixVideo/vtkSonixVideoSource.cxx
	SonixVideo/vtkSonixPortaVideoSource.cxx 
    SonixVideo/vtkSonixVolumeReader.cxx
    SonixVideo/itkUlteriusImageIO.cxx
    SonixVideo/itkUlteriusImageIOFactory.cxx  
    )
	
	if (WIN32)	
		SET (ImageAcquisition_HDRS
      ${ImageAcquisition_HDRS}
      SonixVideo/vtkSonixVideoSource.h
	  SonixVideo/vtkSonixPortaVideoSource.h
      SonixVideo/vtkSonixVolumeReader.h
      SonixVideo/itkUlteriusImageIO.h
      SonixVideo/itkUlteriusImageIOFactory.h       
      )
	endif (WIN32)	

	IF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 1)
    # Use Ultrasonix SDK 1.2 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-1.2/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)        
    SET(ULTERIUS_INCLUDE_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-1.2/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-1.2/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)    
    SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
      optimized ${ULTERIUS_LIB_DIR}/r/ulterius.lib
      debug ${ULTERIUS_LIB_DIR}/d/ulterius.lib
      )    
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${ULTRASONIX_SDK_BINARY_DIR}/r/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/r/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
	ELSEIF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 2)
    # Use Ultrasonix SDK 2.0 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-2.0/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
    SET(ULTERIUS_INCLUDE_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-2.0/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-2.0/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)    
    SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS}
      ${ULTERIUS_LIB_DIR}/ulterius.lib
      )
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${ULTRASONIX_SDK_BINARY_DIR}/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_string_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_unit_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_xml${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/xerces-c_2_7_u_vc6${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
	ELSEIF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 5)
		# Use Ultrasonix SDK 5.x		
		IF (PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 6)
			# Ultrasonix SDK 5.6.x requested
			UltrasonixSdk(5 6 4)
		ELSEIF (PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 7)		
			# Ultrasonix SDK 5.7.x requested
			UltrasonixSdk(5 7 0)		
		ELSE (PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 6)
			MESSAGE( FATAL_ERROR "The chosen PLUS_ULTRASONIX_SDK_MINOR_VERSION value is not supported.")
		ENDIF (PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 6)			
	ELSE (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 1)
		MESSAGE( FATAL_ERROR "The chosen PLUS_ULTRASONIX_SDK_MAJOR_VERSION value is not supported.")
	ENDIF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 1)
    
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS}
    ${ULTERIUS_INCLUDE_DIR}
	  ${PORTA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SonixVideo 
    CACHE INTERNAL "" FORCE
    )
    
ENDIF (PLUS_USE_SONIX_VIDEO)

# --------------------------------------------------------------------------
# ICCapturing support
IF (PLUS_USE_ICCAPTURING_VIDEO)
	SET (ImageAcquisition_SRCS
		${ImageAcquisition_SRCS}
		ICCapturing/vtkICCapturingSource.cxx
		ICCapturing/ICCapturingListener.cxx
	)	
	if (WIN32)	
		SET (ImageAcquisition_HDRS
			${ImageAcquisition_HDRS}
			ICCapturing/vtkICCapturingSource.h
			ICCapturing/ICCapturingListener.h 
			)
	endif (WIN32)	
	# Look for ICCapturing libraries

  SET(ICCAPTURING_INCLUDE_DIR ${PLTOOLS_DIR}/ImagingControl/ImagingControl-3.1/include CACHE PATH "Path to the ImagingControl frame grabber include directory." FORCE)
  IF ( WIN32 )
    SET(ICCAPTURING_BIN_DIR ${PLTOOLS_DIR}/ImagingControl/ImagingControl-3.1/win32 CACHE PATH "Path to the ImagingControl frame grabber binary directory." FORCE)
  ENDIF ( WIN32 )
  IF ( CMAKE_HOST_WIN32 AND CMAKE_CL_64 )
    SET(ICCAPTURING_BIN_DIR ${PLTOOLS_DIR}/ImagingControl/ImagingControl-3.1/x64 CACHE PATH "Path to the ImagingControl frame grabber binary directory." FORCE)
  ENDIF ( CMAKE_HOST_WIN32 AND CMAKE_CL_64 )
    
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS} 
    ${ICCAPTURING_INCLUDE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ICCapturing
    CACHE INTERNAL "" FORCE
    )

	SET( SUFFIX_TIS_UDSHL08 "_vc9")
	IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio 7" )
		SET( SUFFIX_TIS_UDSHL08 "_vc71")
	ENDIF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio 7" )
	
	IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio 8" )
		SET( SUFFIX_TIS_UDSHL08 "_vc8")
	ENDIF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio 8" )
	
	IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio 9" )
		SET( SUFFIX_TIS_UDSHL08 "_vc9")
	ENDIF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio 9" )
	
	SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
		optimized ${ICCAPTURING_BIN_DIR}/release/TIS_UDSHL08${SUFFIX_TIS_UDSHL08}${CMAKE_STATIC_LIBRARY_SUFFIX}
		debug ${ICCAPTURING_BIN_DIR}/debug/TIS_UDSHL08${SUFFIX_TIS_UDSHL08}d${CMAKE_STATIC_LIBRARY_SUFFIX}
		)

	SET ( External_Libraries_Install ${External_Libraries_Install}
		${ICCAPTURING_BIN_DIR}/release/TIS_UDSHL08${SUFFIX_TIS_UDSHL08}${CMAKE_SHARED_LIBRARY_SUFFIX}
		${ICCAPTURING_BIN_DIR}/release/TIS_DShowLib08${CMAKE_SHARED_LIBRARY_SUFFIX}
		)
ENDIF (PLUS_USE_ICCAPTURING_VIDEO)

    
# --------------------------------------------------------------------------
# Video-for-Windows video digitizer support

IF (PLUS_USE_VFW_VIDEO)
	SET (ImageAcquisition_SRCS ${ImageAcquisition_SRCS} 
		VideoForWindows/vtkWin32VideoSource2.cxx
		)
	if (WIN32)	
		SET (ImageAcquisition_HDRS ${ImageAcquisition_HDRS}
			VideoForWindows/vtkWin32VideoSource2.h
			)
	endif (WIN32)	
	SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
    ws2_32
    )		
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS}
  	${CMAKE_CURRENT_SOURCE_DIR}/VideoForWindows 	
  	CACHE INTERNAL "" FORCE
    )      
ENDIF (PLUS_USE_VFW_VIDEO)

# --------------------------------------------------------------------------
# Build the library

SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
  vtkCommon 
  vtkHybrid 
  vtkPlusCommon
  ITKIO
  ITKBasicFilters
  )

INCLUDE_DIRECTORIES( ${ImageAcquisition_INCLUDE_DIRS} )
ADD_LIBRARY(vtkImageAcquisition ${ImageAcquisition_SRCS} ${ImageAcquisition_HDRS})
TARGET_LINK_LIBRARIES(vtkImageAcquisition ${ImageAcquisition_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkImageAcquisition CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkImageAcquisition vtkImageAcquisition ${PlusLib_BINARY_DIR}/src/ImageAcquisition/vtkImageAcquisition.vcproj vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#

INSTALL(FILES 
	${External_Libraries_Install}
	DESTINATION bin
    CONFIGURATIONS Release
	COMPONENT RuntimeLibraries
	)

INSTALL(TARGETS vtkImageAcquisition
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

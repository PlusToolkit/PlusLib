# --------------------------------------------------------------------------
# Options

OPTION (PLUS_USE_ULTRASONIX_VIDEO "Provide support for Ultrasonix ultrasound scanners" OFF)
OPTION (PLUS_USE_BKPROFOCUS_VIDEO "Provide support for BK ProFocus ultrasound scanners" OFF)
OPTION (PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION (PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)
OPTION (PLUS_USE_EPIPHAN "Provide support for the Epiphan" OFF)

# --------------------------------------------------------------------------
# Sources

SET (ImageAcquisition_SRCS
  itkFcsvReader.cxx
  itkFcsvWriter.cxx
  vtkPlusVideoSource.cxx 
  vtkPlusVideoSourceFactory.cxx
  vtkSavedDataVideoSource.cxx
  vtkUsSimulatorVideoSource.cxx
  )
	
SET (ImageAcquisition_HDRS)
if (WIN32)	
	SET (ImageAcquisition_HDRS
    itkFcsvReader.h
    itkFcsvWriter.h
    vtkPlusVideoSource.h 
    vtkPlusVideoSourceFactory.h
    vtkSavedDataVideoSource.h
    vtkUsSimulatorVideoSource.h
    )
endif (WIN32)	

SET (ImageAcquisition_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)
SET (External_Libraries_Install)
SET (External_Libraries_Debug)
SET (ImageAcquisition_LIBS) 

# --------------------------------------------------------------------------
# OpenIGTLink support

IF (PLUS_USE_OpenIGTLink)

	SET (ImageAcquisition_SRCS
    ${ImageAcquisition_SRCS}
		OpenIGTLinkVideo/vtkOpenIGTLinkVideoSource.cxx
		)
	IF (WIN32)	
    SET (ImageAcquisition_HDRS
      ${ImageAcquisition_HDRS}
      OpenIGTLinkVideo/vtkOpenIGTLinkVideoSource.h
    )
	ENDIF (WIN32)
	
	SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS} 
		${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLinkVideo
    )
	
	SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS}
    OpenIGTLink
    vtkPlusOpenIGTLink
    )
	
ENDIF(PLUS_USE_OpenIGTLink)

# --------------------------------------------------------------------------
# Ultrasonix support

IF (PLUS_USE_ULTRASONIX_VIDEO)

	SET (ImageAcquisition_SRCS
    ${ImageAcquisition_SRCS}
    SonixVideo/vtkSonixVideoSource.cxx
    SonixVideo/vtkSonixVolumeReader.cxx
    SonixVideo/itkUlteriusImageIO.cxx
    SonixVideo/itkUlteriusImageIOFactory.cxx   
    SonixVideo/vtkSonixPortaVideoSource.cxx
    )
		
	IF (WIN32)	
    SET (ImageAcquisition_HDRS
      ${ImageAcquisition_HDRS}
      SonixVideo/vtkSonixVideoSource.h
      SonixVideo/vtkSonixVolumeReader.h
      SonixVideo/itkUlteriusImageIO.h
      SonixVideo/itkUlteriusImageIOFactory.h
      SonixVideo/vtkSonixPortaVideoSource.h
      )
	ENDIF (WIN32)	

  MESSAGE(STATUS "Ultrasonix SDK directory: ${ULTRASONIX_SDK_DIR}")
	IF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 1)
    # Use Ultrasonix SDK 1.2 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)        
    SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)    
    SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
      optimized ${ULTERIUS_LIB_DIR}/r/ulterius.lib
      debug ${ULTERIUS_LIB_DIR}/d/ulterius.lib
      )    
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${ULTRASONIX_SDK_BINARY_DIR}/r/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/r/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
   SET  ( External_Libraries_Debug ${External_Libraries_Debug}
      ${ULTRASONIX_SDK_BINARY_DIR}/d/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/d/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
	ELSEIF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 2)
    # Use Ultrasonix SDK 2.0 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
    SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)    
    SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS}
      ${ULTERIUS_LIB_DIR}/ulterius.lib
      )
    SET ( Ultrasonix_External_Libs 
      ${ULTRASONIX_SDK_BINARY_DIR}/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_string_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_unit_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_xml${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/xerces-c_2_7_u_vc6${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    SET ( External_Libraries_Install ${External_Libraries_Install} ${Ultrasonix_External_Libs} )
    SET ( External_Libraries_Debug ${External_Libraries_Debug} ${Ultrasonix_External_Libs} )
  ELSE()
    SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ultrasonix SDK binary directory." FORCE)
    SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/ulterius/inc CACHE PATH "Path to the Ultrasonix SDK Ulterius include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/ulterius/lib CACHE PATH "Path to the Ultrasonix SDK Ulterius library directory." FORCE)
    SET(PORTA_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/porta/inc ${ULTRASONIX_SDK_DIR}/porta/wrapper CACHE PATH "Path to the Ultrasonix SDK Porta include directory." FORCE)
    SET(PORTA_LIB_DIR ${ULTRASONIX_SDK_DIR}/porta/lib CACHE PATH "Path to the Ultrasonix SDK Porta library directory." FORCE)			      			

    SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
      ${ULTERIUS_LIB_DIR}/ulterius.lib
      ${PORTA_LIB_DIR}/porta.lib
      )
		
		IF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 6)
			# All DLLs except ITK, VTK, and QT libraries
			SET(ULTRASONIX_SDK_SHARED_LIBRARIES 
				"${ULTRASONIX_SDK_BINARY_DIR}/actions.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/adaptivefilter.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/amplio.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/ATC3DG.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/BCAD.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/CadDLL.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/CadSDKWin32.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/console.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/cvgop.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/cvie.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/cvltk.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/daq.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/DICOM.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/DICOMNet.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/DigitalSignature.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/display.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/ecg.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/eizmon.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/elasto.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/ewfapi.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/fluctus.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/ftd2xx.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/gps.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/impero.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/libmmd.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/measurements.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/memoryleakdetector.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/mfc100u.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/msvcp100.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/msvcp71.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/msvcr100.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/msvcr71.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/panoramic.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/patient.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/patientreview.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/PDF.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/porta.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/presets.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/reporting.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/reslicevolume.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/StarBurn.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/strategy.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/surf.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/sx32w.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/texo.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/ulterius.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/umcDICOM.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/umc_string_convert.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/umc_unit_convert.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/umc_xml.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utils_controls.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utils_sse.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utils_statics.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utx_driver.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utx_imgproc.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utx_seq.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/utx_utils.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/variables.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/xerces-c_2_7_u_vc10.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/xerces-c_2_8.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/zlib1.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/zlibwapi.dll"
				"${ULTRASONIX_SDK_BINARY_DIR}/zones.dll"
				)
		ELSE()
			# Prior to SDK 6.x all DLLs can be added, no filtering is needed
			FILE(GLOB ULTRASONIX_SDK_SHARED_LIBRARIES "${ULTRASONIX_SDK_BINARY_DIR}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}") 
		ENDIF()
		
    SET ( External_Libraries_Install ${External_Libraries_Install} ${ULTRASONIX_SDK_SHARED_LIBRARIES} )
    SET ( External_Libraries_Debug ${External_Libraries_Debug} ${ULTRASONIX_SDK_SHARED_LIBRARIES} )
  ENDIF()
    
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS}
    ${ULTERIUS_INCLUDE_DIR}        
    ${PORTA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SonixVideo 
    )
    	
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

# --------------------------------------------------------------------------
# BK ProFocus support
IF (PLUS_USE_BKPROFOCUS_VIDEO)  
  SET (ImageAcquisition_SRCS
    ${ImageAcquisition_SRCS}
    BkProFocus/vtkBkProFocusVideoSource.cxx
    BkProFocus/PlusBkProFocusReceiver.cxx
    )
  if (WIN32)	
    SET (ImageAcquisition_HDRS
      ${ImageAcquisition_HDRS}
      BkProFocus/vtkBkProFocusVideoSource.h
      BkProFocus/PlusBkProFocusReceiver.h
      )
  endif (WIN32)	
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS} 
    ${GRABBIELIB_SOURCE_DIR}/AcquisitionLibrary
    ${GRABBIELIB_SOURCE_DIR}/BkmDataFileLib
    ${GRABBIELIB_SOURCE_DIR}/cfuff
    ${GRABBIELIB_SOURCE_DIR}/CommandAndControl
    ${GRABBIELIB_SOURCE_DIR}/LogLibrary
    ${GRABBIELIB_SOURCE_DIR}/OemParams
    ${GRABBIELIB_SOURCE_DIR}/ParametersLibrary        
    ${CMAKE_CURRENT_SOURCE_DIR}/BkProFocus
    )
  SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
    GrabbieLib
    ${DALSASAPERA_LIB_DIR}/SapClassBasic${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
ENDIF (PLUS_USE_BKPROFOCUS_VIDEO)

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
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS} 
    ${ICCAPTURING_INCLUDE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ICCapturing
    )
	
	SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
		optimized ${ICCAPTURING_TIS_UDSHL09_STATIC_LIB}
		debug ${ICCAPTURING_TIS_UDSHL09D_STATIC_LIB}
		)

	SET ( External_Libraries_Install ${External_Libraries_Install}
		${ICCAPTURING_TIS_UDSHL09_SHARED_LIB}
		${ICCAPTURING_TIS_DSHOWLIB09_SHARED_LIB}
		)
  SET ( External_Libraries_Debug ${External_Libraries_Debug} 
    ${ICCAPTURING_TIS_UDSHL09D_SHARED_LIB}
		${ICCAPTURING_TIS_DSHOWLIB09D_SHARED_LIB}
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
  	)      
  SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
    Vfw32.lib
    )
ENDIF (PLUS_USE_VFW_VIDEO)

# --------------------------------------------------------------------------
# Epiphan support

IF (PLUS_USE_EPIPHAN)

  IF( ${CMAKE_GENERATOR} MATCHES Win64 )
  	SET (EPIPHAN_PLATFORM_SUBDIR x64)
  ELSE()
    SET (EPIPHAN_PLATFORM_SUBDIR Win32)
  ENDIF()

  SET (EPIPHAN_SDK_BASE_DIR ${PLUSLIB_TOOLS_DIR}/epiphan)
  SET (EPIPHAN_INCLUDE_DIR ${EPIPHAN_SDK_BASE_DIR} CACHE PATH "Path to the Epiphan SDK include (*.h) directory." FORCE)  
  SET (EPIPHAN_BINARY_DIR ${EPIPHAN_SDK_BASE_DIR}/${EPIPHAN_PLATFORM_SUBDIR} CACHE PATH "Path to the Epiphan SDK binary (*.dll) directory." FORCE)
  SET (EPIPHAN_LIB_DIR ${EPIPHAN_SDK_BASE_DIR}/${EPIPHAN_PLATFORM_SUBDIR} CACHE PATH "Path to the Epiphan SDK library (*.lib) directory." FORCE)

	SET (ImageAcquisition_SRCS ${ImageAcquisition_SRCS} 
		Epiphan/vtkEpiphanVideoSource.cxx
		)
	if (WIN32)	
		SET (ImageAcquisition_HDRS ${ImageAcquisition_HDRS}
			Epiphan/vtkEpiphanVideoSource.h
			)
	endif (WIN32)	
	SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
    ${EPIPHAN_LIB_DIR}/frmgrab${CMAKE_STATIC_LIBRARY_SUFFIX}
    )		
  SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS}     
    ${EPIPHAN_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Epiphan
    )        	
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug} 
    ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF (PLUS_USE_EPIPHAN)

# --------------------------------------------------------------------------
# Build the library

SET (ImageAcquisition_LIBS ${ImageAcquisition_LIBS} 
  vtkCommon 
  vtkHybrid 
  vtkPlusCommon
  ITKIO
  ITKBasicFilters
  vtkTracking
  vtkUsSimulatorAlgo
  vtkRfProcessingAlgo
  )

SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS} 
  ${UsSimulatorAlgo_INCLUDE_DIRS}
  ${Tracking_INCLUDE_DIRS}
  )

SET (ImageAcquisition_INCLUDE_DIRS ${ImageAcquisition_INCLUDE_DIRS} CACHE INTERNAL "" FORCE)      
INCLUDE_DIRECTORIES( ${ImageAcquisition_INCLUDE_DIRS} )
ADD_LIBRARY(vtkImageAcquisition ${ImageAcquisition_SRCS} ${ImageAcquisition_HDRS})
TARGET_LINK_LIBRARIES(vtkImageAcquisition ${ImageAcquisition_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkImageAcquisition CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkImageAcquisition vtkImageAcquisition;"${PlusLib_BINARY_DIR}/src/ImageAcquisition/vtkImageAcquisition.vcproj";vtkPlusCommon;vtkTracking;vtkUsSimulatorAlgo CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
    )
  FILE(COPY ${External_Libraries_Debug}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
    )                
ELSE()
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
    )        
ENDIF()
    
# --------------------------------------------------------------------------
# Install
INSTALL(FILES ${External_Libraries_Install}
  DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )

INSTALL(TARGETS vtkImageAcquisition
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

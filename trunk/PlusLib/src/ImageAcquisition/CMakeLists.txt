# --------------------------------------------------------------------------
# Options

OPTION (PLUS_USE_SONIX_VIDEO "Provide support for the SonixRP video source" OFF)
OPTION (PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION (PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)
OPTION (PLUS_PRINT_VIDEO_TIMESTAMP_DEBUG_INFO "Print video timestamp filtering debug info to file in gnuplot data format" OFF)
SET (ULTERIUS_MAJOR_VERSION 5 CACHE STRING "Set Ultrasonix Ulterius SDK major version")

# --------------------------------------------------------------------------
# Sources

SET (ImageAcquisition_SRCS
	itkMetaImageSequenceIO.cxx 
	itkFcsvReader.cxx
	itkFcsvWriter.cxx
	vtkVideoBuffer2.cxx 
	vtkVideoFrame2.cxx 
	vtkVideoSource2.cxx 
	vtkBufferedVideoSource.cxx 
	vtkTrackedFrameList.cxx 
    vtkSavedDataVideoSource.cxx 
	)
	
SET (ImageAcquisition_HDRS)
if (WIN32)	
	SET (ImageAcquisition_HDRS
		itkMetaImageSequenceIO.h
		itkFcsvReader.h
		itkFcsvWriter.h
		vtkVideoBuffer2.h 
		vtkVideoFrame2.h 
		vtkVideoSource2.h 
		vtkBufferedVideoSource.h
		vtkTrackedFrameList.h
        vtkSavedDataVideoSource.h 
		)
endif (WIN32)	

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

SET (External_Libraries_Install)

# --------------------------------------------------------------------------
# SonixRP support

SET (SONIX_LIBRARY)
IF (PLUS_USE_SONIX_VIDEO)

	SET (ImageAcquisition_SRCS
		${ImageAcquisition_SRCS}
		SonixVideo/vtkSonixVideoSource.cxx
		SonixVideo/vtkSonixVideoSource2.cxx
		SonixVideo/vtkSonixVolumeReader.cxx
        SonixVideo/itkUlteriusImageIO.cxx
        SonixVideo/itkUlteriusImageIOFactory.cxx  
	)
	
	if (WIN32)	
		SET (ImageAcquisition_HDRS
			${ImageAcquisition_HDRS}
			SonixVideo/vtkSonixVideoSource.h
			SonixVideo/vtkSonixVideoSource2.h
			SonixVideo/vtkSonixVolumeReader.h
            SonixVideo/itkUlteriusImageIO.h
            SonixVideo/itkUlteriusImageIOFactory.h       
			)
	endif (WIN32)	

	FIND_PATH(ULTERIUS_INCLUDE_DIR "ulterius.h")
	FIND_PATH(ULTERIUS_BINARY_DIR "ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    
    SET (PLUS_INCLUDE_DIRS ${PLUS_INCLUDE_DIRS} ${ULTERIUS_INCLUDE_DIR} CACHE INTERNAL "" )
	
	IF (ULTERIUS_MAJOR_VERSION EQUAL 1)
		# Look for SonixRP Ulterius SDK 1.2
		FIND_PATH(ULTERIUS_LIB "d/ulterius.lib")
		
		SET (SONIX_LIBRARY 
			optimized ${ULTERIUS_LIB}/r/ulterius.lib
			debug ${ULTERIUS_LIB}/d/ulterius.lib
			)

		SET ( External_Libraries_Install ${External_Libraries_Install}
			${ULTERIUS_BINARY_DIR}/r/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/r/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	ELSEIF (ULTERIUS_MAJOR_VERSION EQUAL 2)
		# Look for SonixRP Ulterius SDK 2.0
		FIND_PATH(ULTERIUS_LIB "ulterius.lib")
		
		SET (SONIX_LIBRARY
			${ULTERIUS_LIB}/ulterius.lib
			)
		SET ( External_Libraries_Install ${External_Libraries_Install}
			${ULTERIUS_BINARY_DIR}/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_string_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_unit_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_xml${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/xerces-c_2_7_u_vc6${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	ELSEIF (ULTERIUS_MAJOR_VERSION EQUAL 5)
		# Look for SonixRP Ulterius SDK 5.6.4
		FIND_PATH(ULTERIUS_LIB "ulterius.lib")
		
		SET (SONIX_LIBRARY 
			${ULTERIUS_LIB}/ulterius.lib
			)
		SET ( External_Libraries_Install ${External_Libraries_Install}
			${ULTERIUS_BINARY_DIR}/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/ftd2xx${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_xml${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_string_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/umc_unit_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${ULTERIUS_BINARY_DIR}/xerces-c_2_7_u_vc6${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
			
	ENDIF (ULTERIUS_MAJOR_VERSION EQUAL 1)
    
ENDIF (PLUS_USE_SONIX_VIDEO)


# --------------------------------------------------------------------------
# ICCapturing support
SET (ICCapturing_LIBRARY)
IF (PLUS_USE_ICCAPTURING_VIDEO)
	SET (ImageAcquisition_SRCS
		${ImageAcquisition_SRCS}
		ICCapturing/vtkICCapturingSource.cxx
		ICCapturing/vtkICCapturingSource2.cxx
		ICCapturing/ICCapturingListener.cxx
	)
	
	if (WIN32)	
		SET (ImageAcquisition_HDRS
			${ImageAcquisition_HDRS}
			ICCapturing/vtkICCapturingSource.h
			ICCapturing/vtkICCapturingSource2.h
			ICCapturing/ICCapturingListener.h 
			)
	endif (WIN32)	
	# Look for ICCapturing libraries
	FIND_PATH(ICCAPTURING_INCLUDE_DIR "tisudshl.h")
	SET (PLUS_INCLUDE_DIRS ${PLUS_INCLUDE_DIRS} ${ICCAPTURING_INCLUDE_DIR} CACHE INTERNAL "" )
	FIND_PATH(ICCAPTURING_BIN_DIR "win32/release/TIS_DShowLib08.dll")

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
	
	SET (ICCapturing_LIBRARY ${ICCapturing_LIBRARY} 
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

SET (VFW_LIBRARY)
IF (PLUS_USE_VFW_VIDEO)
	SET (ImageAcquisition_SRCS
		${ImageAcquisition_SRCS} 
		VideoForWindows/vtkWin32VideoSource2.cxx
		)
		if (WIN32)	
		SET (ImageAcquisition_HDRS
			${ImageAcquisition_HDRS}
			VideoForWindows/vtkWin32VideoSource2.h
			)
	endif (WIN32)	
	SET (VFW_LIBRARY ${VFW_LIBRARY} ws2_32.lib)
	
	SET ( External_Libraries_Install ${External_Libraries_Install}
		
		)
		
ENDIF (PLUS_USE_VFW_VIDEO)

# --------------------------------------------------------------------------
# Build the library

INCLUDE_DIRECTORIES( ${PLUS_INCLUDE_DIRS} )

SET (ImageAcquisition_LIBS 
        vtkCommon 
        vtkHybrid 
        ITKIO
        vtkPlusCommon 
        ${SONIX_LIBRARY} 
        ${VFW_LIBRARY} 
        ${ICCapturing_LIBRARY} 
        )

ADD_LIBRARY(vtkImageAcquisition ${ImageAcquisition_SRCS} ${ImageAcquisition_HDRS})
TARGET_LINK_LIBRARIES(vtkImageAcquisition ${ImageAcquisition_LIBS})

# --------------------------------------------------------------------------
# Install
#

INSTALL(FILES 
	${External_Libraries_Install}
	DESTINATION bin
	COMPONENT Runtime
	)

INSTALL(TARGETS vtkImageAcquisition
  DESTINATION bin
  COMPONENT Runtime)

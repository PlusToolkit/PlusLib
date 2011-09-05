# --------------------------------------------------------------------------
# Options

OPTION (PLUS_USE_SONIX_VIDEO "Provide support for the SonixRP video source" OFF)
OPTION (PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION (PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)
SET (ULTERIUS_MAJOR_VERSION 5 CACHE STRING "Set Ultrasonix Ulterius SDK major version")

# --------------------------------------------------------------------------
# Sources

SET (ImageAcquisition_SRCS
	itkMetaImageSequenceIO.cxx 
	itkFcsvReader.cxx
	itkFcsvWriter.cxx
	vtkVideoBuffer.cxx 
	vtkPlusVideoSource.cxx 
	vtkTrackedFrameList.cxx 
    vtkSavedDataVideoSource.cxx
	PlusVideoFrame.cxx
	)
	
SET (ImageAcquisition_HDRS)
if (WIN32)	
	SET (ImageAcquisition_HDRS
		itkMetaImageSequenceIO.h
		itkFcsvReader.h
		itkFcsvWriter.h
		vtkVideoBuffer.h 
		vtkPlusVideoSource.h 
		vtkTrackedFrameList.h
        vtkSavedDataVideoSource.h 
		PlusVideoFrame.h
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
		SonixVideo/vtkSonixVolumeReader.cxx
        SonixVideo/itkUlteriusImageIO.cxx
        SonixVideo/itkUlteriusImageIOFactory.cxx  
	)
	
	if (WIN32)	
		SET (ImageAcquisition_HDRS
			${ImageAcquisition_HDRS}
			SonixVideo/vtkSonixVideoSource.h
			SonixVideo/vtkSonixVolumeReader.h
            SonixVideo/itkUlteriusImageIO.h
            SonixVideo/itkUlteriusImageIOFactory.h       
			)
	endif (WIN32)	

	IF (ULTERIUS_MAJOR_VERSION EQUAL 1)
		# Look for SonixRP Ulterius SDK 1.2
        SET(ULTERIUS_INCLUDE_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-1.2/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
        SET(ULTERIUS_BINARY_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-1.2/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
        SET(ULTERIUS_LIB_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-1.2/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)
		
		SET (SONIX_LIBRARY 
			optimized ${ULTERIUS_LIB_DIR}/r/ulterius.lib
			debug ${ULTERIUS_LIB_DIR}/d/ulterius.lib
			)

		SET ( External_Libraries_Install ${External_Libraries_Install}
			${ULTERIUS_BINARY_DIR}/r/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
			${ULTERIUS_BINARY_DIR}/r/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	ELSEIF (ULTERIUS_MAJOR_VERSION EQUAL 2)
    
        SET(ULTERIUS_INCLUDE_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-2.0/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
        SET(ULTERIUS_BINARY_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-2.0/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
        SET(ULTERIUS_LIB_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-2.0/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)
		
		SET (SONIX_LIBRARY
			${ULTERIUS_LIB_DIR}/ulterius.lib
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
		# Look for SonixRP Ulterius SDK 5.6.5
        SET(ULTERIUS_INCLUDE_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-5.6.5/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
        SET(ULTERIUS_BINARY_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-5.6.5/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
        SET(ULTERIUS_LIB_DIR ${PLTOOLS_DIR}/Ultrasonix/Ulterius-5.6.5/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)
		
		SET (SONIX_LIBRARY 
			${ULTERIUS_LIB_DIR}/ulterius.lib
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
    
    SET (PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${ULTERIUS_INCLUDE_DIR} CACHE INTERNAL "" )
    
ENDIF (PLUS_USE_SONIX_VIDEO)



# --------------------------------------------------------------------------
# ICCapturing support
SET (ICCapturing_LIBRARY)
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
        
	
	SET (PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${ICCAPTURING_INCLUDE_DIR} CACHE INTERNAL "" )
	

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

INCLUDE_DIRECTORIES( ${PLUSLIB_INCLUDE_DIRS} )

SET (ImageAcquisition_LIBS 
        vtkCommon 
        vtkHybrid 
        vtkPlusCommon
		ITKIO
		ITKBasicFilters
        ${SONIX_LIBRARY} 
        ${VFW_LIBRARY} 
        ${ICCapturing_LIBRARY} 
        )

ADD_LIBRARY(vtkImageAcquisition ${ImageAcquisition_SRCS} ${ImageAcquisition_HDRS})
TARGET_LINK_LIBRARIES(vtkImageAcquisition ${ImageAcquisition_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkImageAcquisition CACHE INTERNAL "" FORCE)
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

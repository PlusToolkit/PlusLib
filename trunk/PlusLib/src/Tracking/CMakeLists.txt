# --------------------------------------------------------------------------
# Options
OPTION (PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION (PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
OPTION (PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION (PLUS_USE_BRACHY_TRACKER "Provide support for the brachy stepper tracking device" ON)
OPTION (PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" OFF)
OPTION (PLUS_USE_Ascension3DGm "Provide support for the Ascension 3DGm MedSafe Tracker" OFF)

# --------------------------------------------------------------------------
# Sources
SET (Tracking_SRCS
  vtkFakeTracker.cxx 
  vtkTracker.cxx 
  vtkTrackerBuffer.cxx 
  vtkTrackerTool.cxx 
  vtkSavedDataTracker.cxx 
	)

SET (Tracking_HDRS)

if (WIN32)	
  SET (Tracking_HDRS
    vtkFakeTracker.h
    vtkTracker.h 
    vtkTrackerBuffer.h 
    vtkTrackerTool.h 
    vtkSavedDataTracker.h
    )
endif (WIN32)	

SET (External_Libraries_Install "")
SET (Tracking_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# NDI common support

IF (PLUS_USE_CERTUS OR PLUS_USE_POLARIS)

  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ndicapi
    CACHE INTERNAL "" FORCE
    )

	SET (Tracking_LIBS ${Tracking_LIBS} 
		vtkndicapi
		)
    
  ADD_SUBDIRECTORY(ndicapi)

ENDIF (PLUS_USE_CERTUS OR PLUS_USE_POLARIS)

# --------------------------------------------------------------------------
# NDI Certus support

IF (PLUS_USE_CERTUS)

  SET(NDIOAPI_INCLUDE_DIR ${PLTOOLS_DIR}/NDI/Certus/NDIoapi-3.0.0.66/include CACHE PATH "Path to the NDI oapi SDK include directory." FORCE)
  SET(NDIOAPI_BINARY_DIR ${PLTOOLS_DIR}/NDI/Certus/NDIoapi-3.0.0.66/bin CACHE PATH "Path to the NDI oapi SDK binary directory." FORCE)

  SET (Tracking_SRCS ${Tracking_SRCS}
    NDICertusTracking/vtkNDICertusTracker.cxx
    )
    
  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} 
    ${NDIOAPI_INCLUDE_DIR}
    CACHE INTERNAL "" FORCE
    )
	
	if (WIN32)	
		SET (Tracking_HDRS ${Tracking_HDRS}
			NDICertusTracking/vtkNDICertusTracker.h
			)
	endif (WIN32)
	  
  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/NDICertusTracking 
    CACHE INTERNAL "" FORCE
    )
  	  	  
	SET (Tracking_LIBS ${Tracking_LIBS} 
		${NDIOAPI_BINARY_DIR}/oapi${CMAKE_STATIC_LIBRARY_SUFFIX}
		)
	
	SET ( External_Libraries_Install ${External_Libraries_Install}
		${NDIOAPI_BINARY_DIR}/oapi${CMAKE_SHARED_LIBRARY_SUFFIX}
		)

ENDIF (PLUS_USE_CERTUS)

#--------------------------------------------------------------------------
# NDI POLARIS support
IF (PLUS_USE_POLARIS)

	SET (Tracking_SRCS ${Tracking_SRCS}
		PolarisTracking/vtkPOLARISTracker.cxx
		PolarisTracking/vtkNDITracker.cxx
	  )
	
	if (WIN32)	
		SET (Tracking_HDRS ${Tracking_HDRS}
			PolarisTracking/vtkPOLARISTracker.h
			PolarisTracking/vtkNDITracker.h
			)
	endif (WIN32)

  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/PolarisTracking
    CACHE INTERNAL "" FORCE)
					  	
ENDIF (PLUS_USE_POLARIS)

#--------------------------------------------------------------------------
# MICRONTRACKER support
IF (PLUS_USE_MICRONTRACKER)

	SET (Tracking_SRCS ${Tracking_SRCS}
		MicronTracking/vtkMicronTracker.cxx
	)
	
	if (WIN32)	
		SET (Tracking_HDRS ${Tracking_HDRS}
			MicronTracking/vtkMicronTracker.h
			)
	endif (WIN32)

  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicronTracking 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicronTracking/Utils
    CACHE INTERNAL "" FORCE
    )
	 
  INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
  
	SET (Tracking_LIBS ${Tracking_LIBS} 
		vtkMicrontrackerfiles
		)
    
  ADD_SUBDIRECTORY(MicronTracking/Utils)
  
  SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkMicrontrackerfiles CACHE INTERNAL "" FORCE	)
  
ENDIF (PLUS_USE_MICRONTRACKER)

#--------------------------------------------------------------------------
# BrachyTracker support
IF (PLUS_USE_BRACHY_TRACKER)

  SET(USDIGITAL_SEI_BASE_DIR ${PLUSLIB_TOOLS_DIR}/UsDigital/SEI_5.22)

  SET(USDIGITAL_SEI_INCLUDE_DIR ${USDIGITAL_SEI_BASE_DIR}/inc CACHE PATH "Path to the US Digital SEI include directory (needed for the CIVCO stepper)." FORCE)
  SET(USDIGITAL_SEI_BINARY_DIR ${USDIGITAL_SEI_BASE_DIR}/bin CACHE PATH "Path to the US Digital SEI binary directory (needed for the CIVCO stepper)." FORCE)
  SET(USDIGITAL_SEI_LIB_DIR ${USDIGITAL_SEI_BASE_DIR}/lib CACHE PATH "Path to the US Digital SEI library directory (needed for the CIVCO stepper)." FORCE)

	SET (Tracking_SRCS ${Tracking_SRCS}
		BrachyTracking/vtkBrachyTracker.cxx
	)
	
  if (WIN32)	
    SET (Tracking_HDRS
      ${Tracking_HDRS}
      BrachyTracking/vtkBrachyTracker.h
      )
  endif (WIN32)
    
  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/BrachyTracking
    ${CMAKE_CURRENT_SOURCE_DIR}/BrachyTracking/Utils
    CACHE INTERNAL "" FORCE
    )		
  
  SET (Tracking_LIBS ${Tracking_LIBS}
    ${USDIGITAL_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX}
    BrachyStepper
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${USDIGITAL_SEI_BINARY_DIR}/SEISrv32.exe
    )

  ADD_SUBDIRECTORY(BrachyTracking/Utils)
    
ENDIF (PLUS_USE_BRACHY_TRACKER)

#--------------------------------------------------------------------------
# Ascension 3DG support (trakSTAR)
IF( PLUS_USE_Ascension3DG )

  SET( ATC_TRAKSTAR_BASE_DIR ${PLUSLIB_TOOLS_DIR}/Ascension/trakSTAR_940038_RevD)

  SET( ATC_TRAKSTAR_INCLUDE_DIR ${ATC_TRAKSTAR_BASE_DIR}/inc CACHE PATH "Path to the Ascension trakSTAR 3D Guidance library folder (where the ATC3DG.h file is located)" FORCE )
  SET( ATC_TRAKSTAR_LIB_DIR ${ATC_TRAKSTAR_BASE_DIR}/lib CACHE PATH "Path to the Ascension trakSTAR 3D Guidance library folder" FORCE )
  SET( ATC_TRAKSTAR_BINARY_DIR ${ATC_TRAKSTAR_BASE_DIR}/bin CACHE PATH "Path to the Ascension trakSTAR 3D Guidance dll folder" FORCE ) 
    
  SET (Tracking_SRCS ${Tracking_SRCS}
    Ascension3DGTracking/vtkAscension3DGTracker.cxx 
    )
    
  if (WIN32)
    SET (Tracking_HDRS ${Tracking_HDRS} 
      Ascension3DGTracking/vtkAscension3DGTracker.h 
      )
  endif (WIN32)

  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} 
    ${ATC_TRAKSTAR_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    CACHE INTERNAL "" FORCE
    )

  SET (Tracking_LIBS ${Tracking_LIBS}
    ${ATC_TRAKSTAR_LIB_DIR}/ATC3DG${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${ATC_TRAKSTAR_BINARY_DIR}/ATC3DG${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
        
ENDIF( PLUS_USE_Ascension3DG )

#--------------------------------------------------------------------------
# Ascension 3DGm support (medSAFE)
IF( PLUS_USE_Ascension3DGm )
  
  SET(ATC_MEDSAFE_BASE_DIR ${PLUSLIB_TOOLS_DIR}/Ascension/medSAFE_940033_Rev_F)

  SET( ATC_MEDSAFE_INCLUDE_DIR ${ATC_MEDSAFE_BASE_DIR}/inc CACHE PATH "Path to the Ascension medSAFE 3D Guidance library folder (where the ATC3DGm.h file is located)" FORCE )
  SET( ATC_MEDSAFE_LIB_DIR ${ATC_MEDSAFE_BASE_DIR}/lib CACHE PATH "Path to the Ascension medSAFE 3D Guidance library folder" FORCE )
  SET( ATC_MEDSAFE_BINARY_DIR ${ATC_MEDSAFE_BASE_DIR}/bin CACHE PATH "Path to the Ascension medSAFE 3D Guidance dll folder" FORCE ) 

      
  SET (Tracking_SRCS ${Tracking_SRCS} Ascension3DGTracking/vtkAscension3DGTracker.cxx )
	if (WIN32)	
		SET (Tracking_HDRS ${Tracking_HDRS} Ascension3DGTracking/vtkAscension3DGTracker.h )
	endif (WIN32)
	
  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} 
    ${ATC_MEDSAFE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    CACHE INTERNAL "" FORCE
    )

  SET (Tracking_LIBS ${Tracking_LIBS}
    ${ATC_MEDSAFE_LIB_DIR}/ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${ATC_MEDSAFE_BINARY_DIR}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
        
ENDIF( PLUS_USE_Ascension3DGm )

#--------------------------------------------------------------------------
# OpenIGTLink tracker
IF( PLUS_USE_OpenIGTLink )
  
  SET (Tracking_SRCS ${Tracking_SRCS}
    OpenIGTLinkTracking/vtkOpenIGTLinkTracker.cxx 
    )
  
  if (WIN32)
    SET (Tracking_HDRS ${Tracking_HDRS} 
      OpenIGTLinkTracking/vtkOpenIGTLinkTracker.h 
      )
  endif (WIN32)

  SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLinkTracking
    CACHE INTERNAL "" FORCE
    )

ENDIF( PLUS_USE_OpenIGTLink )

# --------------------------------------------------------------------------
# Build the library

SET (Tracking_LIBS ${Tracking_LIBS}
  vtkPlusCommon 
  vtkImageAcquisition 
  vtkCommon 
  vtkHybrid 
  vtkFiltering 
  )

INCLUDE_DIRECTORIES( ${Tracking_INCLUDE_DIRS} )
ADD_LIBRARY(vtkTracking ${Tracking_SRCS} ${Tracking_HDRS})
TARGET_LINK_LIBRARIES( vtkTracking ${Tracking_LIBS} )

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkTracking CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkTracking vtkTracking "${PlusLib_BINARY_DIR}/src/Tracking/vtkTracking.vcproj" vtkPlusCommon vtkImageAcquisition CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY ${External_Libraries_Install}
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    FILE(COPY ${External_Libraries_Install}
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )                
ELSE()
    FILE(COPY "${QT_BINARY_DIR}/"
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )        
ENDIF()

# --------------------------------------------------------------------------
# Install
#

INSTALL(FILES 
	${External_Libraries_Install}
	DESTINATION bin
    CONFIGURATIONS Release
	COMPONENT RuntimeLibraries
	)
	
INSTALL(TARGETS vtkTracking
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)

# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
	ADD_SUBDIRECTORY(Testing)
ENDIF (BUILD_TESTING)

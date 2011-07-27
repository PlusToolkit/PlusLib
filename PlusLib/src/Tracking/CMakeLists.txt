# --------------------------------------------------------------------------
# Options
OPTION (PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION (PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
OPTION (PLUS_USE_FLOCK "Provide support for the Ascension Flock of Birds" OFF)
OPTION (PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION (PLUS_USE_AMS_TRACKER "Provide support for the AMS Stepper tracking device" ON)
OPTION (PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" OFF)
OPTION (PLUS_USE_Ascension3DGm "Provide support for the Ascension 3DGm MedSafe Tracker" OFF)

# --------------------------------------------------------------------------
# Sources
SET (Tracking_SRCS
	vtkFakeTracker.cxx 
	vtkTracker.cxx 
	vtkTrackerBuffer.cxx 
	vtkTrackerTool.cxx 
	vtkBufferedTracker.cxx 
    vtkSavedDataTracker.cxx 
	)

SET (Tracking_HDRS)
if (WIN32)	
	SET (Tracking_HDRS
		vtkFakeTracker.h
		vtkTracker.h 
		vtkTrackerBuffer.h 
		vtkTrackerTool.h 
		vtkBufferedTracker.h
        vtkSavedDataTracker.h
		)
endif (WIN32)	

IF (PLUS_USE_CERTUS OR PLUS_USE_POLARIS)
	ADD_SUBDIRECTORY(ndicapi)
ENDIF (PLUS_USE_CERTUS OR PLUS_USE_POLARIS)

SET (External_Libraries_Install)

# --------------------------------------------------------------------------
# NDICertus support
SET (CERTUS_LIBRARY)
IF (PLUS_USE_CERTUS)
	SET (Tracking_SRCS
		${Tracking_SRCS}
		NDICertusTracking/vtkNDICertusTracker.cxx
	)
	
	if (WIN32)	
		SET (Tracking_HDRS
			${Tracking_HDRS}
			NDICertusTracking/vtkNDICertusTracker.h
			)
	endif (WIN32)
	
    IF(PLUS_USE_PLTools)
       SET(NDIOAPI_INCLUDE_DIR ${PLTOOLS_DIR}/NDI/Certus/NDIoapi-3.0.0.66/include CACHE PATH "Path to the NDI oapi SDK include directory." FORCE)
       SET(NDIOAPI_BINARY_DIR ${PLTOOLS_DIR}/NDI/Certus/NDIoapi-3.0.0.66/bin CACHE PATH "Path to the NDI oapi SDK binary directory." FORCE)
    ELSE(PLUS_USE_PLTools)
        FIND_PATH(NDIOAPI_INCLUDE_DIR ndopto.h DOC "Path to the NDI oapi SDK include directory.")
        FIND_PATH(NDIOAPI_BINARY_DIR oapi.dll DOC "Path to the NDI oapi SDK binary directory.")
    ENDIF(PLUS_USE_PLTools)

    SET (PLUSLIB_INCLUDE_DIRS 
            ${PLUSLIB_INCLUDE_DIRS} 
            ${VTKNDICAPI_SOURCE_DIR} 
            ${VTKNDICAPI_BINARY_DIR}
			${NDIOAPI_INCLUDE_DIR}
            CACHE INTERNAL ""
            )

	SET (CERTUS_LIBRARY ${CERTUS_LIBRARY} 
		vtkndicapi
		${NDIOAPI_BINARY_DIR}/oapi${CMAKE_STATIC_LIBRARY_SUFFIX}
		)
	
	SET ( External_Libraries_Install ${External_Libraries_Install}
		${NDIOAPI_BINARY_DIR}/oapi${CMAKE_SHARED_LIBRARY_SUFFIX}
		)

ENDIF (PLUS_USE_CERTUS)


#--------------------------------------------------------------------------
# POLARIS support
SET (POLARIS_LIBRARY)
IF (PLUS_USE_POLARIS)
	SET (Tracking_SRCS
		${Tracking_SRCS}
		PolarisTracking/vtkPOLARISTracker.cxx
		PolarisTracking/vtkNDITracker.cxx
	)
	
	if (WIN32)	
		SET (Tracking_HDRS
			${Tracking_HDRS}
			PolarisTracking/vtkPOLARISTracker.h
			PolarisTracking/vtkNDITracker.h
			)
	endif (WIN32)

    SET (PLUSLIB_INCLUDE_DIRS 
            ${PLUSLIB_INCLUDE_DIRS} 
            ${VTKNDICAPI_SOURCE_DIR}
            ${VTKNDICAPI_BINARY_DIR}
            CACHE INTERNAL ""
			)
					  
	SET (POLARIS_LIBRARY vtkndicapi)
	SET ( External_Libraries_Install ${External_Libraries_Install}
		
		)
ENDIF (PLUS_USE_POLARIS)

#--------------------------------------------------------------------------
# FLOCK support
SET (FLOCK_LIBRARY)
IF (PLUS_USE_FLOCK)
	SET (Tracking_SRCS
		${Tracking_SRCS}
		FlockTracking/vtkFlockTracker.cxx
	)
	
	if (WIN32)	
		SET (Tracking_HDRS
			${Tracking_HDRS}
			FlockTracking/vtkFlockTracker.h
			)
	endif (WIN32)
	
	ADD_SUBDIRECTORY(FlockTracking/Utils)
    SET (PLUSLIB_INCLUDE_DIRS 
                ${PLUSLIB_INCLUDE_DIRS} 
                ${VTKFLOCK_SOURCE_DIR}
                ${VTKFLOCK_BINARY_DIR}
                CACHE INTERNAL ""
				)
					  
	SET (FLOCK_LIBRARY vtkflock)
	SET ( External_Libraries_Install ${External_Libraries_Install}
		
		)
ENDIF (PLUS_USE_FLOCK)


#--------------------------------------------------------------------------
# MICRONTRACKER support
SET (MICRONTRACKER_LIBRARY)
IF (PLUS_USE_MICRONTRACKER)
	SET (Tracking_SRCS
		${Tracking_SRCS}
		MicronTracking/vtkMicronTracker.cxx
	)
	
	if (WIN32)	
		SET (Tracking_HDRS
			${Tracking_HDRS}
			MicronTracking/vtkMicronTracker.h
			)
	endif (WIN32)
	
	ADD_SUBDIRECTORY(MicronTracking/Utils)

    SET (PLUSLIB_INCLUDE_DIRS 
                ${PLUSLIB_INCLUDE_DIRS} 
                ${MICRONTRACKERFILES_SOURCE_DIR}
				${MICRONTRACKERFILES_BINARY_DIR}
                CACHE INTERNAL ""
				)
	INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
  
  SET (MICRONTRACKER_LIBRARY vtkMicrontrackerfiles)
  SET ( External_Libraries_Install ${External_Libraries_Install}
		
		)
ENDIF (PLUS_USE_MICRONTRACKER)


#--------------------------------------------------------------------------
# AMSTRACKER support
SET (AMSTRACKER_LIBRARY)
IF (PLUS_USE_AMS_TRACKER)
	SET (Tracking_SRCS
		${Tracking_SRCS}
		AMSTracking/vtkAMSTracker.cxx
	)
	
	if (WIN32)	
		SET (Tracking_HDRS
			${Tracking_HDRS}
			AMSTracking/vtkAMSTracker.h
			)
	endif (WIN32)
	
	ADD_SUBDIRECTORY(AMSTracking/Utils)
	
    SET (PLUSLIB_INCLUDE_DIRS 
                ${PLUSLIB_INCLUDE_DIRS} 
                ${AMSStepper_SOURCE_DIR}
                ${AMSStepper_BINARY_DIR}
                CACHE INTERNAL ""
				)
 
  SET (AMSTRACKER_LIBRARY vtkAMSStepper)
ENDIF (PLUS_USE_AMS_TRACKER)


#--------------------------------------------------------------------------
# Ascension 3DG support
SET(ATC_LIBRARY CACHE INTERNAL FORCE)
IF( PLUS_USE_Ascension3DG )

    # Use non MedSafe version
    IF( PLUS_USE_PLTools )
      SET( ATC_INCLUDE_DIRECTORY ${PLTOOLS_DIR}/Ascension/Ascension3DG/inc CACHE PATH "Directory where the ATC3DG.h file is located" FORCE )
      SET( ATC_LIBRARY_DIRECTORY ${PLTOOLS_DIR}/Ascension/Ascension3DG/lib CACHE PATH "Path to the Ascension 3D Guidance library folder" FORCE )
      SET( ATC_BINARY_DIRECTORY ${PLTOOLS_DIR}/Ascension/Ascension3DG/bin CACHE PATH "Path to the Ascension 3D Guidance dll folder" FORCE )
    ELSE( PLUS_USE_PLTools )
      FIND_PATH( ATC_INCLUDE_DIRECTORY ATC3DG.h PATHS "C:/Program Files/Ascension/3D Guidance (Rev D)/3D Guidance API" DOC "Directory where the ATC3DG.h file is located")
      FIND_PATH( ATC_LIBRARY_DIRECTORY ATC3DG${CMAKE_STATIC_LIBRARY_SUFFIX} PATHS "C:/Program Files/Ascension/3D Guidance (Rev D)/3D Guidance API" DOC "Path to the Ascension 3D Guidance library folder" )
      FIND_PATH( ATC_BINARY_DIRECTORY ATC3DG${CMAKE_SHARED_LIBRARY_SUFFIX} PATHS "C:/Program Files/Ascension/3D Guidance (Rev D)/3D Guidance API" DOC "Path to the Ascension 3D Guidance library folder" )
    ENDIF( PLUS_USE_PLTools )
  
  SET (PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} 
            ${ATC_INCLUDE_DIRECTORY}
            ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
            CACHE INTERNAL ""
            )
  
  SET (Tracking_SRCS ${Tracking_SRCS} Ascension3DGTracking/vtkAscension3DGTracker.cxx )
	if (WIN32)	
		SET (Tracking_HDRS ${Tracking_HDRS} Ascension3DGTracking/vtkAscension3DGTracker.h )
	endif (WIN32)
	
  SET ( ATC_LIBRARY ${ATC_LIBRARY} ${ATC_LIBRARY_DIRECTORY}/ATC3DG${CMAKE_STATIC_LIBRARY_SUFFIX} )
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
        ${ATC_BINARY_DIRECTORY}/ATC3DG${CMAKE_SHARED_LIBRARY_SUFFIX} )
        
ENDIF( PLUS_USE_Ascension3DG )

#--------------------------------------------------------------------------
# Ascension 3DGm support (MedSafe!!!)
IF( PLUS_USE_Ascension3DGm )
  
    # MedSafe version
    IF(PLUS_USE_PLTools)
        SET(ATCM_INCLUDE_DIRECTORY ${PLTOOLS_DIR}/Ascension/Ascension3DGm/inc CACHE PATH "Directory where the ATC3DGm.h file is located" FORCE)
        SET(ATCM_LIBRARY_DIRECTORY ${PLTOOLS_DIR}/Ascension/Ascension3DGm/lib CACHE PATH "Path to the Ascension 3D Guidance medSAFE library folder" FORCE)
    ELSE(PLUS_USE_PLTools)
         FIND_PATH( ATCM_INCLUDE_DIRECTORY ATC3DGm.h PATHS "C:/Program Files/Ascension/3D Guidance medSAFE (Rev F)/3D Guidance API" DOC "Directory where the ATC3DGm.h file is located")
         FIND_PATH( ATCM_LIBRARY_DIRECTORY ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX} PATHS "C:/Program Files/Ascension/3D Guidance medSAFE (Rev F)/3D Guidance API" DOC "Path to the Ascension 3D Guidance medSAFE library folder" )
    ENDIF(PLUS_USE_PLTools)
    
  SET (PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} 
            ${ATCM_INCLUDE_DIRECTORY}
            ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
            CACHE INTERNAL ""
            )
  
  SET (Tracking_SRCS ${Tracking_SRCS} Ascension3DGTracking/vtkAscension3DGTracker.cxx )
	if (WIN32)	
		SET (Tracking_HDRS ${Tracking_HDRS} Ascension3DGTracking/vtkAscension3DGTracker.h )
	endif (WIN32)
	
  SET ( ATC_LIBRARY ${ATC_LIBRARY} ${ATCM_LIBRARY_DIRECTORY}/ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX} )
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
		    ${ATCM_LIBRARY_DIRECTORY}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX} )
        
ENDIF( PLUS_USE_Ascension3DGm )

INCLUDE_DIRECTORIES( ${PLUSLIB_INCLUDE_DIRS} )

IF (BUILD_TESTING)
	ADD_SUBDIRECTORY(Testing)
ENDIF (BUILD_TESTING)

# --------------------------------------------------------------------------
# Build the library

SET (Tracking_LIBS 	vtkPlusCommon vtkImageAcquisition vtkCommon vtkHybrid vtkFiltering ${Tracking_LIBS} ${CERTUS_LIBRARY} ${POLARIS_LIBRARY} ${FLOCK_LIBRARY} ${MICRONTRACKER_LIBRARY} ${AMSTRACKER_LIBRARY} ${ATC_LIBRARY} )

ADD_LIBRARY(vtkTracking ${Tracking_SRCS} ${Tracking_HDRS})

TARGET_LINK_LIBRARIES(	vtkTracking ${Tracking_LIBS} )


# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkTracking CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkTracking vtkTracking "${PlusLib_BINARY_DIR}/src/Tracking/vtkTracking.vcproj" vtkPlusCommon vtkImageAcquisition CACHE INTERNAL "" FORCE)

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



# --------------------------------------------------------------------------
# Options
OPTION (PLUS_USE_POLARIS "Provide support for the NDI Polaris and Aurora" OFF)
OPTION (PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
OPTION (PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION (PLUS_USE_BRACHY_TRACKER "Provide support for the brachy stepper tracking device" ON)
OPTION (PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" OFF)
OPTION (PLUS_USE_Ascension3DGm "Provide support for the Ascension 3DGm MedSafe Tracker" OFF)
OPTION (PLUS_USE_PHIDGET_SPATIAL_TRACKER "Provide support for the Phidget Spatial accelerometer" OFF)
OPTION (PLUS_USE_3dConnexion_TRACKER "Provide support for the 3dConnexion 3d mouse" OFF)
OPTION (PLUS_USE_ULTRASONIX_VIDEO "Provide support for Ultrasonix ultrasound scanners" OFF)
OPTION (PLUS_USE_BKPROFOCUS_VIDEO "Provide support for BK ProFocus ultrasound scanners through the OEM (TCP/IP) interface" OFF)
OPTION (PLUS_USE_BKPROFOCUS_CAMERALINK "Provide support for BK ProFocus ultrasound scanners through CameraLink interface" OFF)
OPTION (PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION (PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)
OPTION (PLUS_USE_EPIPHAN "Provide support for the Epiphan" OFF)
OPTION (PLUS_USE_INTERSON_VIDEO "Provide support Interson USB ultrasound probes" OFF)
OPTION(PLUS_USE_STEALTHLINK "Provide support for the Medtronick StealthLink Server" OFF)

OPTION (PLUS_TEST_BKPROFOCUS "Enable testing of acquisition from BK ProFocus ultrasound systems. Enable this only if a BK ProFocus device is connected to this computer." OFF)
OPTION (PLUS_TEST_ULTRASONIX "Enable testing of acquisition from Ultrasonix ultrasound systems. Enable this only if an Ultrasonix device accessible from this computer." OFF)
IF (PLUS_TEST_ULTRASONIX)
  SET (PLUS_TEST_ULTRASONIX_IP_ADDRESS "127.0.0.1" CACHE STRING "IP address of the Ultrasonix scanner that is used during testing")
ENDIF()

# --------------------------------------------------------------------------
# Sources
SET (DataCollection_SRCS
  vtkDataCollector.cxx 
  vtkDataCollectorSynchronizer.cxx
  vtkPlusDevice.cxx
  vtkPlusChannel.cxx
  vtkPlusDeviceFactory.cxx
  vtkPlusDataSource.cxx 
  FakeTracking/vtkFakeTracker.cxx   
  SavedDataSource/vtkSavedDataSource.cxx 
  VirtualDevices/vtkVirtualMixer.cxx
  VirtualDevices/vtkVirtualSwitcher.cxx
  VirtualDevices/vtkVirtualDiscCapture.cxx 
  VirtualDevices/vtkVirtualVolumeReconstructor.cxx
  SerialLine.cxx
  itkFcsvReader.cxx
  itkFcsvWriter.cxx
  vtkPlusBuffer.cxx 
  UsSimulatorVideo/vtkUsSimulatorVideoSource.cxx
  vtkUsImagingParameters.cxx
  )

SET (DataCollection_HDRS)
if (WIN32)  
  SET (DataCollection_HDRS
    vtkDataCollector.h 
    vtkDataCollectorSynchronizer.h
    vtkPlusDevice.h
    vtkPlusDeviceTypes.h 
    vtkPlusChannel.h
    vtkPlusDeviceFactory.h
    vtkPlusDataSource.h
    FakeTracking/vtkFakeTracker.h
    SavedDataSource/vtkSavedDataSource.h 
    VirtualDevices/vtkVirtualMixer.h
    VirtualDevices/vtkVirtualSwitcher.h
    VirtualDevices/vtkVirtualDiscCapture.h
    VirtualDevices/vtkVirtualVolumeReconstructor.h
    SerialLine.h
    itkFcsvReader.h
    itkFcsvWriter.h
    vtkPlusBuffer.h 
    UsSimulatorVideo/vtkUsSimulatorVideoSource.h
	vtkUsImagingParameters.h
    )
endif (WIN32)  

SET (External_Libraries_Install)
SET (External_Libraries_Install_Minus_Debug)
SET (External_Libraries_Debug)
SET (External_Libraries_Debug_FileName)
SET (DataCollection_INCLUDE_DIRS 
  ${DataCollection_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/FakeTracking 
  ${CMAKE_CURRENT_SOURCE_DIR}/SavedDataSource 
  ${CMAKE_CURRENT_SOURCE_DIR}/UsSimulatorVideo 
  ${CMAKE_CURRENT_SOURCE_DIR}/VirtualDevices  
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# NDI Certus support

IF (PLUS_USE_CERTUS)
    SET (DataCollection_SRCS ${DataCollection_SRCS} 
      NDICertusTracking/vtkNDICertusTracker.cxx
      )
        
    SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} ${NDIOAPI_INCLUDE_DIR} CACHE INTERNAL "" FORCE )
  
    IF (WIN32)  
      SET (DataCollection_HDRS ${DataCollection_HDRS}
        NDICertusTracking/vtkNDICertusTracker.h
        )
    ENDIF (WIN32)
    
    SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/NDICertusTracking 
      CACHE INTERNAL "" FORCE)
            
    SET (DataCollection_LIBS ${DataCollection_LIBS} ${NDIOAPI_LIBRARY} )
    
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${NDIOAPI_BINARY_DIR}/oapi${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
ENDIF (PLUS_USE_CERTUS)

#--------------------------------------------------------------------------
# NDI POLARIS support
IF (PLUS_USE_POLARIS)

  SET (DataCollection_SRCS ${DataCollection_SRCS}
    PolarisTracking/vtkNDITracker.cxx
    )
  
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      PolarisTracking/vtkNDITracker.h
      )
  endif (WIN32)

  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/PolarisTracking
    CACHE INTERNAL "" FORCE)

  SET (DataCollection_LIBS ${DataCollection_LIBS} vtkndicapi )    
              
ENDIF (PLUS_USE_POLARIS)

#--------------------------------------------------------------------------
# MICRONTRACKER support
IF (PLUS_USE_MICRONTRACKER)

  SET (DataCollection_SRCS ${DataCollection_SRCS}
    MicronTracking/vtkMicronTracker.cxx
  )
  
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      MicronTracking/vtkMicronTracker.h
      )
  endif (WIN32)

  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicronTracking 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicronTracking/Utils
    CACHE INTERNAL "" FORCE)
   
  INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
  
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    vtkMicronTrackerInterface
    ${MICRONTRACKER_LIBRARY}
    )   
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${MICRONTRACKER_BINARY_DIR}/MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${MICRONTRACKER_BINARY_DIR}/Windist/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${MICRONTRACKER_BINARY_DIR}/Windist/PGRFlyCapture${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${MICRONTRACKER_BINARY_DIR}/Windist/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  
ENDIF (PLUS_USE_MICRONTRACKER)

#--------------------------------------------------------------------------
# BrachyTracker support
IF (PLUS_USE_BRACHY_TRACKER)

  SET (DataCollection_SRCS ${DataCollection_SRCS}
    BrachyTracking/vtkBrachyTracker.cxx
    BrachyTracking/CmsBrachyStepper.cxx
    BrachyTracking/CivcoBrachyStepper.cxx 
  )
  
  if (WIN32)  
    SET (DataCollection_HDRS
      ${DataCollection_HDRS}
      BrachyTracking/vtkBrachyTracker.h
      BrachyTracking/CmsBrachyStepper.h 
      BrachyTracking/BrachyStepper.h 
      BrachyTracking/CivcoBrachyStepper.h
      )
  endif (WIN32)
    
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/BrachyTracking
    ${USDIGITAL_SEI_INCLUDE_DIR}
    CACHE INTERNAL "" FORCE)    
  
  SET (DataCollection_LIBS ${DataCollection_LIBS}
    ${USDIGITAL_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${USDIGITAL_SEI_BINARY_DIR}/SEISrv32.exe
    )
    
ENDIF (PLUS_USE_BRACHY_TRACKER)

#--------------------------------------------------------------------------
# Ascension 3DG support (trakSTAR)
IF( PLUS_USE_Ascension3DG )
   
  SET (DataCollection_SRCS ${DataCollection_SRCS}
    Ascension3DGTracking/vtkAscension3DGTracker.cxx 
    )
    
  if (WIN32)
    SET (DataCollection_HDRS ${DataCollection_HDRS} 
      Ascension3DGTracking/vtkAscension3DGTracker.h 
      )
  endif (WIN32)

  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${ATC_TRAKSTAR_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    CACHE INTERNAL "" FORCE)

  SET (DataCollection_LIBS ${DataCollection_LIBS}
    ${ATC_TRAKSTAR_LIB_DIR}/ATC3DG${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${ATC_TRAKSTAR_BINARY_DIR}/ATC3DG${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
        
ENDIF( PLUS_USE_Ascension3DG )

#--------------------------------------------------------------------------
# Ascension 3DGm support (medSAFE)
IF( PLUS_USE_Ascension3DGm )
        
  SET (DataCollection_SRCS ${DataCollection_SRCS} Ascension3DGTracking/vtkAscension3DGmTracker.cxx )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS} Ascension3DGTracking/vtkAscension3DGmTracker.h )
  endif (WIN32)
  
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${ATC_MEDSAFE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    CACHE INTERNAL "" FORCE)

  SET (DataCollection_LIBS ${DataCollection_LIBS}
    ${ATC_MEDSAFE_LIB_DIR}/ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${ATC_MEDSAFE_BINARY_DIR}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
        
ENDIF( PLUS_USE_Ascension3DGm )

#--------------------------------------------------------------------------
# PhidgetSpatial 3/3/3 MARG sensor (magnetometer, accelerometer, gyroscope)
IF( PLUS_USE_PHIDGET_SPATIAL_TRACKER )

  IF( ${CMAKE_GENERATOR} MATCHES Win64 )
    SET( PHIDGET_SPATIAL_LIB_DIR ${PHIDGET_SPATIAL_WIN64_LIB_DIR} )
    SET( PHIDGET_SPATIAL_BINARY_DIR ${PHIDGET_SPATIAL_WIN64_BINARY_DIR} ) 
  ELSE()
    SET( PHIDGET_SPATIAL_LIB_DIR ${PHIDGET_SPATIAL_WIN32_LIB_DIR} )
    SET( PHIDGET_SPATIAL_BINARY_DIR ${PHIDGET_SPATIAL_WIN32_BINARY_DIR} ) 
  ENDIF()
  
  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    PhidgetSpatialTracking/vtkPhidgetSpatialTracker.cxx 
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS} PhidgetSpatialTracking/vtkPhidgetSpatialTracker.h )
  endif (WIN32)
  
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${PHIDGET_SPATIAL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/PhidgetSpatialTracking
    CACHE INTERNAL "" FORCE)

  SET (DataCollection_LIBS ${DataCollection_LIBS}
    ${PHIDGET_SPATIAL_LIB_DIR}/phidget21${CMAKE_STATIC_LIBRARY_SUFFIX}
    vtkxio
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${PHIDGET_SPATIAL_BINARY_DIR}/phidget21${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
          
ENDIF( PLUS_USE_PHIDGET_SPATIAL_TRACKER )

#--------------------------------------------------------------------------
# 3dConnexion SpaceNavigator and similar 3D mouse devices

IF( PLUS_USE_3dConnexion_TRACKER )

  if (WIN32)  # the 3D mouse is currently supported on Windows only
    SET (DataCollection_SRCS ${DataCollection_SRCS} 
      3dConnexionTracking/vtk3dConnexionTracker.cxx
      )
    if (WIN32)  
      SET (DataCollection_HDRS ${DataCollection_HDRS} 3dConnexionTracking/vtk3dConnexionTracker.h )
    endif (WIN32)  
    SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
      ${CMAKE_CURRENT_SOURCE_DIR}/3dConnexionTracking
      CACHE INTERNAL "" FORCE)
  endif (WIN32)
  
ENDIF( PLUS_USE_3dConnexion_TRACKER )

#--------------------------------------------------------------------------
# CH Robotics UM6 MARG sensor (magnetometer, accelerometer, gyroscope)
        
  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    ChRobotics/vtkChRoboticsTracker.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS} ChRobotics/vtkChRoboticsTracker.h )
  endif (WIN32)
  
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ChRobotics
    CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------
# OpenIGTLink 

IF( PLUS_USE_OpenIGTLink )
  SET (DataCollection_SRCS ${DataCollection_SRCS}
    OpenIGTLinkTracking/vtkOpenIGTLinkTracker.cxx 
    OpenIGTLinkVideo/vtkOpenIGTLinkVideoSource.cxx
    )
    
  IF (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      OpenIGTLinkTracking/vtkOpenIGTLinkTracker.h 
      OpenIGTLinkVideo/vtkOpenIGTLinkVideoSource.h
      )
  ENDIF (WIN32)
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLinkTracking 
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLinkVideo
    CACHE INTERNAL "" FORCE)  
  
  SET (DataCollection_LIBS ${DataCollection_LIBS} OpenIGTLink vtkPlusOpenIGTLink )

ENDIF( PLUS_USE_OpenIGTLink )

# --------------------------------------------------------------------------
# Ultrasonix support

IF (PLUS_USE_ULTRASONIX_VIDEO)

  SET (DataCollection_SRCS
    ${DataCollection_SRCS}
    SonixVideo/vtkSonixVideoSource.cxx
    SonixVideo/vtkSonixVolumeReader.cxx
    SonixVideo/itkUlteriusImageIO.cxx
    SonixVideo/itkUlteriusImageIOFactory.cxx   
    SonixVideo/vtkSonixPortaVideoSource.cxx
    )
    
  IF (WIN32)  
    SET (DataCollection_HDRS
      ${DataCollection_HDRS}
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
    SET (DataCollection_LIBS ${DataCollection_LIBS} 
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
    SET (DataCollection_LIBS ${DataCollection_LIBS}
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

    SET (DataCollection_LIBS ${DataCollection_LIBS} 
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
    
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}
    ${ULTERIUS_INCLUDE_DIR}        
    ${PORTA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SonixVideo 
    )
      
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

# --------------------------------------------------------------------------
# BK ProFocus support

# OEM interface
IF (PLUS_USE_BKPROFOCUS_VIDEO)  
  SET (DataCollection_SRCS ${DataCollection_SRCS}
    BkProFocus/vtkBkProFocusOemVideoSource.cxx 
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      BkProFocus/vtkBkProFocusOemVideoSource.h
      )
  endif (WIN32)
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${GRABBIELIB_SOURCE_DIR}/AcquisitionLibrary
    ${GRABBIELIB_SOURCE_DIR}/BkmDataFileLib
    ${GRABBIELIB_SOURCE_DIR}/cfuff
    ${GRABBIELIB_SOURCE_DIR}/CommandAndControl
    ${GRABBIELIB_SOURCE_DIR}/LogLibrary
    ${GRABBIELIB_SOURCE_DIR}/OemParams
    ${GRABBIELIB_SOURCE_DIR}/ParametersLibrary        
    ${CMAKE_CURRENT_SOURCE_DIR}/BkProFocus
    )
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    GrabbieLib
    ${VTK_PNG_LIBRARIES}
    )  

  # CameraLink interface
  IF (PLUS_USE_BKPROFOCUS_CAMERALINK)  
  SET (DataCollection_SRCS ${DataCollection_SRCS}
    BkProFocus/vtkBkProFocusCameraLinkVideoSource.cxx
    BkProFocus/PlusBkProFocusCameraLinkReceiver.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      BkProFocus/vtkBkProFocusCameraLinkVideoSource.h
      BkProFocus/PlusBkProFocusCameraLinkReceiver.h
      )
  endif (WIN32)  
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ${DALSASAPERA_LIB_DIR}/SapClassBasic${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  ENDIF (PLUS_USE_BKPROFOCUS_CAMERALINK)

ENDIF (PLUS_USE_BKPROFOCUS_VIDEO)

# --------------------------------------------------------------------------
# ICCapturing support
IF (PLUS_USE_ICCAPTURING_VIDEO)

  SET (DataCollection_SRCS
    ${DataCollection_SRCS}
    ICCapturing/vtkICCapturingSource.cxx
    ICCapturing/ICCapturingListener.cxx
    )  
  if (WIN32)  
    SET (DataCollection_HDRS
      ${DataCollection_HDRS}
      ICCapturing/vtkICCapturingSource.h
      ICCapturing/ICCapturingListener.h 
      )
  endif (WIN32)  
  # Look for ICCapturing libraries   
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${ICCAPTURING_INCLUDE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ICCapturing
    )
  
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
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
# StealthLink2.0 suppport
IF (PLUS_USE_STEALTHLINK)
  SET (DataCollection_SRCS
    ${DataCollection_SRCS}
    StealthLink/vtkStealthLinkTracker.cxx
    )  
  if (WIN32)  
    SET (DataCollection_HDRS
      ${DataCollection_HDRS}
      StealthLink/vtkStealthLinkTracker.h
      )
  endif (WIN32)  
  
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${STEALTHLINK2_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/StealthLink
    )
  
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    optimized ${STEALTHLINK2_STEALTHLINK_STATIC_LIBRARY}
    debug ${STEALTHLINK2_STEALTHLINKD_STATIC_LIBRARY}
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${STEALTHLINK2_STEALTHLINK_SHARED_LIBRARY}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug} 
    ${STEALTHLINK2_STEALTHLINKD_SHARED_LIBRARY}
    )
ENDIF (PLUS_USE_STEALTHLINK)

    
# --------------------------------------------------------------------------
# Video-for-Windows video digitizer support

IF (PLUS_USE_VFW_VIDEO)

  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    VideoForWindows/vtkWin32VideoSource2.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      VideoForWindows/vtkWin32VideoSource2.h
      )
  endif (WIN32)  
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ws2_32
    )    
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoForWindows   
    )      
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    Vfw32.lib
    )
ENDIF (PLUS_USE_VFW_VIDEO)

# --------------------------------------------------------------------------
# Microsoft Media Foundation video digitizer support

IF (PLUS_USE_MMF_VIDEO)

  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    MicrosoftMediaFoundation/vtkMmfVideoSource.cxx 
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      MicrosoftMediaFoundation/vtkMmfVideoSource.h 
      )
  endif (WIN32)
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicrosoftMediaFoundation
    ${CMAKE_CURRENT_SOURCE_DIR}/MicrosoftMediaFoundation/InputCaptureLibrary
    ${WINDOWSSDK_LATEST_DIR}/Include
    )
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    MfVideoCaptureLibrary
    )
ENDIF (PLUS_USE_MMF_VIDEO)

# --------------------------------------------------------------------------
# Epiphan support

IF (PLUS_USE_EPIPHAN)

  IF( ${CMAKE_GENERATOR} MATCHES Win64 )
    SET (EPIPHAN_BINARY_DIR ${EPIPHAN_WIN64_BINARY_DIR})
    SET (EPIPHAN_LIB_DIR ${EPIPHAN_WIN64_LIB_DIR})
  ELSE()
    SET (EPIPHAN_BINARY_DIR ${EPIPHAN_WIN32_BINARY_DIR})
    SET (EPIPHAN_LIB_DIR ${EPIPHAN_WIN32_LIB_DIR})
  ENDIF()

  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    Epiphan/vtkEpiphanVideoSource.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      Epiphan/vtkEpiphanVideoSource.h
      )
  endif (WIN32)  
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ${EPIPHAN_LIB_DIR}/frmgrab${CMAKE_STATIC_LIBRARY_SUFFIX}
    )    
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}     
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
# Interson support

IF (PLUS_USE_INTERSON_VIDEO)
  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    Interson/vtkIntersonVideoSource.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      Interson/vtkIntersonVideoSource.h
      )
  endif (WIN32)
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ${INTERSON_LIBRARY_DIR}/BmodeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${INTERSON_LIBRARY_DIR}/USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}     
    ${INTERSON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Interson
    )          
  IF( ${CMAKE_GENERATOR} MATCHES Win64 )
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${INTERSON_WIN64_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    SET ( External_Libraries_Debug ${External_Libraries_Debug}
      ${INTERSON_WIN64_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
      SET ( External_Libraries_Install ${External_Libraries_Install}
      ${INTERSON_WIN32_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    SET ( External_Libraries_Debug ${External_Libraries_Debug}
      ${INTERSON_WIN32_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
ENDIF (PLUS_USE_INTERSON_VIDEO)

# --------------------------------------------------------------------------
# Build the library

SET (DataCollection_LIBS ${DataCollection_LIBS}
  vtkCommon 
  vtkHybrid 
  vtkFiltering 
  vtkPlusCommon  
  vtkSignaling
  ${ITK_LIBRARIES}
  vtkUsSimulatorAlgo
  vtkVolumeReconstruction
  vtkRfProcessingAlgo
  ) 
  
SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
  ${UsSimulatorAlgo_INCLUDE_DIRS}
  )

INCLUDE_DIRECTORIES( ${DataCollection_INCLUDE_DIRS} )
ADD_LIBRARY(vtkDataCollection ${DataCollection_SRCS} ${DataCollection_HDRS})
TARGET_LINK_LIBRARIES(vtkDataCollection ${DataCollection_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkDataCollection CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkDataCollection vtkDataCollection;"${PlusLib_BINARY_DIR}/src/DataCollection/vtkDataCollection.vcproj";vtkPlusCommon;vtkUsSimulatorAlgo;vtkRfProcessingAlgo CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
	MESSAGE( " External_Libraries_Install " ${External_Libraries_Install})
    FILE(COPY ${External_Libraries_Install}
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
        )
	#Find the file names of the debug shared libraries
	FOREACH (DEBUG ${External_Libraries_Debug})
		get_filename_component(FileName ${DEBUG} NAME)
		SET(External_Libraries_Debug_FileName ${External_Libraries_Debug_FileName} ${FileName})
	ENDFOREACH(DEBUG)	
	#Extract the release shared libraries that have the same name as debug shared libraries
	FOREACH (RELEASE ${External_Libraries_Install})
		get_filename_component(FileName ${RELEASE} NAME)
		list(FIND External_Libraries_Debug_FileName ${FileName} HasTheFileName)
	IF(HasTheFileName EQUAL -1)
		SET(External_Libraries_Install_Minus_Debug ${External_Libraries_Install_Minus_Debug} ${RELEASE})
	ENDIF()
	ENDFOREACH(RELEASE)
    FILE(COPY ${External_Libraries_Install_Minus_Debug}
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
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
#
INSTALL(FILES ${External_Libraries_Install}
  DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )
  
INSTALL(TARGETS vtkDataCollection
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )
  
# --------------------------------------------------------------------------
# Testing
#

IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

# --------------------------------------------------------------------------
# Options
OPTION (PLUS_USE_POLARIS "Provide support for the NDI Polaris and Aurora" OFF)
OPTION (PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
OPTION (PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION (PLUS_USE_BRACHY_TRACKER "Provide support for the brachy stepper tracking device" OFF)
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
OPTION (PLUS_USE_CAPISTRANO_VIDEO "Provide support Capistrano Labs USB ultrasound probes" OFF)
OPTION (PLUS_USE_INTERSON_VIDEO "Provide support Interson USB ultrasound probes" OFF)
OPTION (PLUS_USE_INTERSONSDKCXX_VIDEO "Provide support Interson SDK 1.X with C++ Wrapper USB ultrasound probes" OFF)
OPTION (PLUS_USE_TELEMED_VIDEO "Provide support for the Telemed ultrasound probes" OFF)
OPTION (PLUS_USE_THORLABS_VIDEO "Provide support for the ThorLabs compact spectrometers" OFF)
OPTION(PLUS_USE_STEALTHLINK "Provide support for the Medtronick StealthLink Server" OFF)
OPTION(PLUS_USE_IntuitiveDaVinci "Enable the tracking of the da Vinci Surgical System. Enable this only if you have a da Vinci that is accessible from this computer." OFF)
OPTION(PLUS_USE_PHILIPS_3D_ULTRASOUND "Provide support for the Philips ie33 3D ultrasound probe system." OFF)
OPTION (PLUS_TEST_BKPROFOCUS "Enable testing of acquisition from BK ProFocus ultrasound systems. Enable this only if a BK ProFocus device is connected to this computer." OFF)
OPTION (PLUS_TEST_ULTRASONIX "Enable testing of acquisition from Ultrasonix ultrasound systems. Enable this only if an Ultrasonix device accessible from this computer." OFF)
IF (PLUS_TEST_ULTRASONIX)
  SET (PLUS_TEST_ULTRASONIX_IP_ADDRESS "127.0.0.1" CACHE STRING "IP address of the Ultrasonix scanner that is used during testing")
ENDIF()
OPTION (PLUS_TEST_MMF_VIDEO "Enable testing of acquisition from the first available Microsoft Media Foundation compatible device. Turn off if no such device is available or the first device does not have any Plus-compatible video mode." ON)

SET (DataCollection_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Sources
SET (DataCollection_SRCS
  vtkDataCollector.cxx 
  vtkPlusDevice.cxx
  vtkPlusChannel.cxx
  vtkPlusDeviceFactory.cxx
  vtkPlusDataSource.cxx
  vtkTimestampedCircularBuffer.cxx
  StreamBufferItem.cxx
  vtkGenericSerialDevice.cxx
  FakeTracking/vtkFakeTracker.cxx   
  SavedDataSource/vtkSavedDataSource.cxx 
  VirtualDevices/vtkVirtualMixer.cxx
  VirtualDevices/vtkVirtualSwitcher.cxx
  VirtualDevices/vtkVirtualDiscCapture.cxx 
  VirtualDevices/vtkVirtualVolumeReconstructor.cxx
  ImageProcessor/vtkImageProcessorVideoSource.cxx
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
    vtkPlusDevice.h
    vtkPlusChannel.h
    vtkPlusDeviceFactory.h
    vtkPlusDataSource.h
    vtkTimestampedCircularBuffer.h 
    StreamBufferItem.h
    vtkGenericSerialDevice.h
    FakeTracking/vtkFakeTracker.h
    SavedDataSource/vtkSavedDataSource.h 
    VirtualDevices/vtkVirtualMixer.h
    VirtualDevices/vtkVirtualSwitcher.h
    VirtualDevices/vtkVirtualDiscCapture.h
    VirtualDevices/vtkVirtualVolumeReconstructor.h
    ImageProcessor/vtkImageProcessorVideoSource.h
    SerialLine.h
    itkFcsvReader.h
    itkFcsvWriter.h
    vtkPlusBuffer.h 
    UsSimulatorVideo/vtkUsSimulatorVideoSource.h
    vtkUsImagingParameters.h
    )
endif (WIN32)  

SET (External_Libraries_Install)
SET (External_Libraries_Debug)
SET (DataCollection_INCLUDE_DIRS 
  ${DataCollection_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/FakeTracking 
  ${CMAKE_CURRENT_SOURCE_DIR}/ImageProcessor
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
            
    SET (DataCollection_LIBS ${DataCollection_LIBS}
      ${NDIOAPI_LIBRARY}
      vtkndicapi # uses ndicapi's math functions
      )
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

  IF (EXISTS "${MICRONTRACKER_BINARY_DIR}/FlyCapture2${CMAKE_SHARED_LIBRARY_SUFFIX}")
    # MicronTracker 3.7.x
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${MICRONTRACKER_BINARY_DIR}/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/FlyCapture2${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/FlyCapture2_C${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/libiomp5md${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/PGRFlyCapture${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
    # MicronTracker 3.6.x
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${MICRONTRACKER_BINARY_DIR}/MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/Windist/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/Windist/PGRFlyCapture${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MICRONTRACKER_BINARY_DIR}/Windist/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
  
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
# Microchip MARG sensor (magnetometer, accelerometer, gyroscope)
        
  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    MicrochipTracking/vtkMicrochipTracker.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS} MicrochipTracking/vtkMicrochipTracker.h )
  endif (WIN32)
  
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicrochipTracking
    CACHE INTERNAL "" FORCE)
    
#--------------------------------------------------------------------------
# OpenIGTLink 

IF( PLUS_USE_OpenIGTLink )
  SET (DataCollection_SRCS ${DataCollection_SRCS}
    OpenIGTLink/vtkOpenIGTLinkDevice.cxx 
    OpenIGTLink/vtkOpenIGTLinkTracker.cxx 
    OpenIGTLink/vtkOpenIGTLinkVideoSource.cxx
    )
    
  IF (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      OpenIGTLink/vtkOpenIGTLinkDevice.h
      OpenIGTLink/vtkOpenIGTLinkTracker.h 
      OpenIGTLink/vtkOpenIGTLinkVideoSource.h
      )
  ENDIF (WIN32)
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLink
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
      ${PORTA_LIB_DIR}/porta.lib
      )
    IF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 6)

      IF (PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 0)
        MESSAGE( FATAL_ERROR "Plus does not support Ultrasonix SDK version 6.0.x") 
      ENDIF()

      SET (DataCollection_LIBS ${DataCollection_LIBS} ${ULTERIUS_LIB_DIR}/ulterius_old.lib)
    ELSE()
      SET (DataCollection_LIBS ${DataCollection_LIBS} ${ULTERIUS_LIB_DIR}/ulterius.lib)
    ENDIF()

    FILE(GLOB ULTRASONIX_SDK_SHARED_LIBRARIES "${ULTRASONIX_SDK_BINARY_DIR}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}")

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
    ${STEALTHLINK_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/StealthLink
    )
  
  IF (WIN32)
    SET (DataCollection_LIBS ${DataCollection_LIBS} 
      optimized ${STEALTHLINK_STEALTHLINK_STATIC_LIBRARY}
      debug ${STEALTHLINK_STEALTHLINKD_STATIC_LIBRARY}
      )
    SET ( External_Libraries_Debug ${External_Libraries_Debug} 
      ${STEALTHLINK_STEALTHLINKD_SHARED_LIBRARY}
      )
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${STEALTHLINK_STEALTHLINK_SHARED_LIBRARY}
      )
  ELSE()
    SET (DataCollection_LIBS ${DataCollection_LIBS} 
      ${STEALTHLINK_STEALTHLINK_STATIC_LIBRARY}
      )
  ENDIF()  

ENDIF (PLUS_USE_STEALTHLINK)

#--------------------------------------------------------------------------
# da Vinci Surgical System support
IF(PLUS_USE_IntuitiveDaVinci)

  SET (DataCollection_SRCS ${DataCollection_SRCS}
    IntuitiveDaVinci/vtkIntuitiveDaVinciTracker.cxx
    IntuitiveDaVinci/IntuitiveDaVinci.cxx
    )

  SET (DataCollection_HDRS ${DataCollection_HDRS}
    IntuitiveDaVinci/vtkIntuitiveDaVinciTracker.h 
    IntuitiveDaVinci/IntuitiveDaVinci.h
    )

  SET(DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}
    ${IntuitiveDaVinci_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/IntuitiveDaVinci
    CACHE INTERNAL "" FORCE)
      
  SET(DataCollection_LIBS ${DataCollection_LIBS}
    ${IntuitiveDaVinci_LIBRARY}
    ws2_32
    iphlpapi
    )

ENDIF(PLUS_USE_IntuitiveDaVinci)

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
# Capistrano Labs support

IF (PLUS_USE_CAPISTRANO_VIDEO)
  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    Capistrano/vtkCapistranoVideoSource.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      Capistrano/vtkCapistranoVideoSource.h
      )
  endif (WIN32)
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ${CAPISTRANO_LIBRARY_DIR}/BmodeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CAPISTRANO_LIBRARY_DIR}/USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}     
    ${CAPISTRANO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Capistrano
    )          
  IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    message("using 64 bit")
    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${CAPISTRANO_WIN64_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${CAPISTRANO_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    SET ( External_Libraries_Debug ${External_Libraries_Debug}
      ${CAPISTRANO_WIN64_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${CAPISTRANO_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
      # Make a temporary copy and rename this file so it can be found as the
      # expected name when the executable starts.
      SET(_usbprobe_dll_renamed ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX})
      FILE(COPY ${CAPISTRANO_WIN32_BINARY_DIR}/USBprobe32${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY})
      FILE(RENAME
        ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/USBprobe32${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${_usbprobe_dll_renamed})
      
      SET ( External_Libraries_Install ${External_Libraries_Install}
      ${_usbprobe_dll_renamed}
      ${CAPISTRANO_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    SET ( External_Libraries_Debug ${External_Libraries_Debug}
      ${_usbprobe_dll_renamed}
      ${CAPISTRANO_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
ENDIF (PLUS_USE_CAPISTRANO_VIDEO)

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
# Interson SDK C++ support

IF (PLUS_USE_INTERSONSDKCXX_VIDEO)
  FIND_PACKAGE( IntersonSDKCxx REQUIRED )
  SET (DataCollection_SRCS ${DataCollection_SRCS} 
    IntersonSDKCxx/vtkIntersonSDKCxxVideoSource.cxx
    )
  if (WIN32)  
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      IntersonSDKCxx/vtkIntersonSDKCxxVideoSource.h
      )
  endif (WIN32)
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ${IntersonSDKCxx_LIBRARIES}
    )
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}     
    ${IntersonSDKCxx_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/IntersonSDKCxx
    )          
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${IntersonSDK_DIR}/Libraries/Interson${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonSDK_DIR}/Libraries/IntersonTools${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug}
    ${IntersonSDK_DIR}/Libraries/Interson${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonSDK_DIR}/Libraries/IntersonTools${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF (PLUS_USE_INTERSONSDKCXX_VIDEO)

# --------------------------------------------------------------------------
# Telemed support

IF (PLUS_USE_TELEMED_VIDEO)
  # ADD_CUSTOM_COMMAND(OUTPUT 
	# Telemed/Usgfw2_h.h
	# Telemed/dlldata.c
	# Telemed/Usgfw2_i.c
	# Telemed/Usgfw2_p.c
    # COMMAND midl.exe /out ./Telemed /header Usgfw2_h.h /no_robust ${TELEMED_INCLUDE_DIR}/Usgfw2.idl
  # )
  
  SET (DataCollection_SRCS ${DataCollection_SRCS}
    Telemed/vtkTelemedVideoSource.cxx
    Telemed/TelemedUltrasound.cxx
    Telemed/Usgfw2_i.c
    Telemed/Usgfw2_p.c
    Telemed/dlldata.c
    )
  IF (WIN32)
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      Telemed/vtkTelemedVideoSource.h
      Telemed/TelemedUltrasound.h
      Telemed/Usgfw2_h.h
      )
  ENDIF (WIN32)

  # Find Telemed SDK include directory
  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}
    ${TELEMED_INCLUDE_DIR}
    ${WINDOWSSDK_LATEST_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Telemed
    )

ENDIF (PLUS_USE_TELEMED_VIDEO)

# --------------------------------------------------------------------------
# Thorlabs support

IF (PLUS_USE_THORLABS_VIDEO)

  SET (DataCollection_SRCS ${DataCollection_SRCS}
    ThorLabs/vtkThorLabsVideoSource.cxx
    )
  IF (WIN32)
    SET (DataCollection_HDRS ${DataCollection_HDRS}
      ThorLabs/vtkThorLabsVideoSource.h
      )
  ENDIF (WIN32)

  SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThorLabs  
    ${THORLABS_INCLUDE_DIR}
    )
  SET (DataCollection_LIBS ${DataCollection_LIBS} 
    ${THORLABS_LIBRARY_DIR}/TLCCS_32${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${THORLABS_LIBRARY_DIR}/visa32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

ENDIF (PLUS_USE_THORLABS_VIDEO)

# --------------------------------------------------------------------------
# Philips ie33 3d ultrasound support

IF (PLUS_USE_PHILIPS_3D_ULTRASOUND)
  IF(PLUS_USE_ULTRASONIX_VIDEO)
	# Ultrasonix contains its own libmmd.dll which does not support all the functionality needed for the philips probe
	# They both cannot be enabled at the same time.
	MESSAGE( FATAL_ERROR "Plus cannot enable both Ultrasonix and Philips devices due to .dll conflicts in their respective SDK packages.")
  ELSE(PLUS_USE_ULTRASONIX_VIDEO)
    SET (DataCollection_SRCS ${DataCollection_SRCS}
      Philips3D/vtkPhilips3DProbeVideoSource.cxx 
      Philips3D/vtkIEEListener.cxx 
      )

    IF (WIN32)  
      SET (DataCollection_HDRS ${DataCollection_HDRS}
        Philips3D/vtkPhilips3DProbeVideoSource.h
        Philips3D/vtkIEEListener.h
        )
    ENDIF (WIN32)

    SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
      ${CMAKE_CURRENT_SOURCE_DIR}/Philips3D
      ${Philips_INCLUDE_DIRS}
      CACHE INTERNAL "" FORCE)

    SET( DataCollection_LIBS ${DataCollection_LIBS}
      ${Philips_LIBRARY_DIR}/PipelineComDll${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${Philips_LIBRARY_DIR}/3DData${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${Philips_LIBRARY_DIR}/3DUtil${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  
    FILE(GLOB PHILIPS_SDK_SHARED_LIBRARIES "${Philips_BINARY_DIRS}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}")

    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${PHILIPS_SDK_SHARED_LIBRARIES}
      )
  ENDIF(PLUS_USE_ULTRASONIX_VIDEO)
ENDIF(PLUS_USE_PHILIPS_3D_ULTRASOUND)

# --------------------------------------------------------------------------
# Build the library

SET (DataCollection_LIBS ${DataCollection_LIBS}
  ${VTK_LIBRARIES}
  vtkPlusCommon  
  ${ITK_LIBRARIES}
  vtkUsSimulatorAlgo
  vtkVolumeReconstruction
  vtkImageProcessingAlgo
  ) 
  
SET (DataCollection_INCLUDE_DIRS ${DataCollection_INCLUDE_DIRS} 
  ${UsSimulatorAlgo_INCLUDE_DIRS}
  )

INCLUDE_DIRECTORIES( ${DataCollection_INCLUDE_DIRS} )
GENERATE_EXPORT_DIRECTIVE_FILE(vtkDataCollection)
ADD_LIBRARY(vtkDataCollection ${DataCollection_SRCS} ${DataCollection_HDRS})
TARGET_LINK_LIBRARIES(vtkDataCollection ${DataCollection_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkDataCollection CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkDataCollection vtkDataCollection;"${PlusLib_BINARY_DIR}/src/DataCollection/vtkDataCollection.vcproj";vtkPlusCommon;vtkUsSimulatorAlgo;vtkImageProcessingAlgo CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY ${External_Libraries_Install}
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
        )
    # If debug and release dll has the same name then the debug dll would be
    # overwritten by the file copy. To prevent this, copy only those release dlls
    # that has no corresponding debug dll with the same filename.
    SET (External_Libraries_Install_Minus_Debug)
    SET (External_Libraries_Debug_FileName)
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

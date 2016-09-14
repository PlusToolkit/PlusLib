# --------------------------------------------------------------------------
# Options
OPTION (PLUS_USE_OPTITRACK "Provide support for the OptiTrack tracking system" OFF)
OPTION (PLUS_USE_OPTIMET_CONOPROBE "Provide support for the Optimet ConoProbe" OFF)

OPTION(PLUS_USE_NDI "Provide support for the NDI POLARIS and AURORA" OFF)
OPTION(PLUS_USE_NDI_CERTUS "Provide support for the NDI Certus" OFF)

OPTION(PLUS_USE_POLARIS "Provide support for the NDI POLARIS and AURORA" OFF)
MARK_AS_ADVANCED(PLUS_USE_POLARIS)
OPTION(PLUS_USE_CERTUS "Provide support for the NDI Certus" OFF)
MARK_AS_ADVANCED(PLUS_USE_CERTUS)

IF(PLUS_USE_POLARIS)
  MESSAGE("PLUS_USE_POLARIS has been deprecated. Changing to PLUS_USE_NDI intead.")
  SET(PLUS_USE_POLARIS OFF CACHE BOOL "Provide support for the NDI POLARIS and AURORA" FORCE)
  SET(PLUS_USE_NDI ON CACHE BOOL "Provide support for the NDI POLARIS and AURORA" FORCE)
ENDIF()
IF(PLUS_USE_CERTUS)
  MESSAGE("PLUS_USE_CERTUS has been deprecated. Changing to PLUS_USE_NDI_CERTUS intead.")
  SET(PLUS_USE_CERTUS OFF CACHE BOOL "Provide support for the NDI Certus" FORCE)
  SET(PLUS_USE_NDI_CERTUS ON CACHE BOOL "Provide support for the NDI Certus" FORCE)
ENDIF()

OPTION (PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION (PLUS_USE_BRACHY_TRACKER "Provide support for the brachy stepper tracking device" OFF)
OPTION (PLUS_USE_USDIGITALENCODERS_TRACKER "Provide support for multiple USDigital encoders tracking device" OFF)
OPTION (PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" ${ENABLED_BY_DEFAULT_ON_WINDOWS_ONLY})
OPTION (PLUS_USE_Ascension3DGm "Provide support for the Ascension 3DGm MedSafe Tracker" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
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
OPTION (PLUS_USE_STEALTHLINK "Provide support for the Medtronick StealthLink Server" OFF)
OPTION (PLUS_USE_IntuitiveDaVinci "Enable the tracking of the da Vinci Surgical System. Enable this only if you have a da Vinci that is accessible from this computer." OFF)
OPTION (PLUS_USE_PHILIPS_3D_ULTRASOUND "Provide support for the Philips ie33 3D ultrasound probe system." OFF)
OPTION (PLUS_TEST_BKPROFOCUS "Enable testing of acquisition from BK ProFocus ultrasound systems. Enable this only if a BK ProFocus device is connected to this computer." OFF)

OPTION (PLUS_TEST_ULTRASONIX "Enable testing of acquisition from Ultrasonix ultrasound systems. Enable this only if an Ultrasonix device accessible from this computer." OFF)
IF (PLUS_TEST_ULTRASONIX)
  SET (PLUS_TEST_ULTRASONIX_IP_ADDRESS "127.0.0.1" CACHE STRING "IP address of the Ultrasonix scanner that is used during testing")
ENDIF()

OPTION(PLUS_USE_MMF_VIDEO "Provide support for the Microsoft Media Foundation video digitizers (requires installation of Windows Platform SDK 7.1 or later)" OFF)
IF(PLUS_USE_MMF_VIDEO)
  OPTION(PLUS_TEST_MMF_VIDEO "Enable testing of acquisition from MMF video device (webcam). Enable this only if an MMF device is connected to this computer." OFF)
ENDIF(PLUS_USE_MMF_VIDEO)

OPTION (PLUS_USE_tesseract "Provide support for optical character recognition virtual device via tesseract OCR library." OFF)
IF(PLUS_USE_tesseract)
  OPTION(PLUS_TEST_tesseract "Enable testing functionality for tesseract virtual recognizer device." ON)
ENDIF()

OPTION(PLUS_USE_NVIDIA_DVP "Provide support for the NVidia Digital Video Pipeline" OFF)
IF (PLUS_USE_NVIDIA_DVP)
  FIND_PACKAGE(QuadroSDI REQUIRED)
  FIND_PACKAGE(OpenGL REQUIRED)
ENDIF()

OPTION(PLUS_USE_OvrvisionPro "Provide support for the OvrvisionPro Stereo Camera" OFF)
IF (PLUS_USE_OvrvisionPro)
  FIND_PACKAGE(OvrvisionPro REQUIRED NO_MODULE)
  OPTION(PLUS_TEST_OvrvisionPro "Enable testing of acquisition of stereo images from the OvrvisionPro" OFF)
ENDIF()

# --------------------------------------------------------------------------
# Haptics
ADD_SUBDIRECTORY(Haptics)

SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${vtkPlusHaptics_INCLUDE_DIRS} CACHE INTERNAL "" FORCE)
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusHaptics CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Sources
SET (PlusDataCollection_SRCS
  vtkPlusDataCollector.cxx 
  vtkPlusDevice.cxx
  vtkPlusUsDevice.cxx
  vtkPlusChannel.cxx
  vtkPlusDeviceFactory.cxx
  vtkPlusDataSource.cxx
  vtkPlusTimestampedCircularBuffer.cxx
  PlusStreamBufferItem.cxx
  vtkPlusGenericSerialDevice.cxx
  FakeTracking/vtkPlusFakeTracker.cxx   
  SavedDataSource/vtkPlusSavedDataSource.cxx 
  VirtualDevices/vtkPlusVirtualMixer.cxx
  VirtualDevices/vtkPlusVirtualSwitcher.cxx
  VirtualDevices/vtkPlusVirtualDiscCapture.cxx 
  VirtualDevices/vtkPlusVirtualVolumeReconstructor.cxx
  ImageProcessor/vtkPlusImageProcessorVideoSource.cxx
  PlusSerialLine.cxx
  itkFcsvReader.cxx
  itkFcsvWriter.cxx
  vtkPlusBuffer.cxx 
  UsSimulatorVideo/vtkPlusUsSimulatorVideoSource.cxx
  vtkPlusUsImagingParameters.cxx
  )

IF(PLUS_USE_tesseract)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} VirtualDevices/vtkPlusVirtualTextRecognizer.cxx)
ENDIF()

SET (PlusDataCollection_HDRS)
IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET (PlusDataCollection_HDRS
    vtkPlusDataCollector.h 
    vtkPlusDevice.h
    vtkPlusUsDevice.h
    vtkPlusChannel.h
    vtkPlusDeviceFactory.h
    vtkPlusDataSource.h
    vtkPlusTimestampedCircularBuffer.h 
    PlusStreamBufferItem.h
    vtkPlusGenericSerialDevice.h
    FakeTracking/vtkPlusFakeTracker.h
    SavedDataSource/vtkPlusSavedDataSource.h 
    VirtualDevices/vtkPlusVirtualMixer.h
    VirtualDevices/vtkPlusVirtualSwitcher.h
    VirtualDevices/vtkPlusVirtualDiscCapture.h
    VirtualDevices/vtkPlusVirtualTextRecognizer.h
    VirtualDevices/vtkPlusVirtualVolumeReconstructor.h
    ImageProcessor/vtkPlusImageProcessorVideoSource.h
    PlusSerialLine.h
    itkFcsvReader.h
    itkFcsvWriter.h
    vtkPlusBuffer.h 
    UsSimulatorVideo/vtkPlusUsSimulatorVideoSource.h
    vtkPlusUsImagingParameters.h
    )
  IF(PLUS_USE_tesseract)
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} VirtualDevices/vtkPlusVirtualTextRecognizer.h)
  ENDIF()
ENDIF()

SET (External_Libraries_Install)
SET (External_Libraries_Debug)

SET (PlusDataCollection_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/FakeTracking 
  ${CMAKE_CURRENT_SOURCE_DIR}/ImageProcessor
  ${CMAKE_CURRENT_SOURCE_DIR}/SavedDataSource 
  ${CMAKE_CURRENT_SOURCE_DIR}/UsSimulatorVideo 
  ${CMAKE_CURRENT_SOURCE_DIR}/VirtualDevices  
  CACHE INTERNAL "" FORCE)
  
# --------------------------------------------------------------------------
# OptiTrack support
IF (PLUS_USE_OPTITRACK)
  IF (WIN32)
    # PlusLib Files
    SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
      OptiTrack/vtkPlusOptiTrackTracker.cxx
      )

    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      OptiTrack/vtkPlusOptiTrackTracker.h
      )

    SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
         ${CMAKE_CURRENT_SOURCE_DIR}/OptiTrack 
         CACHE INTERNAL "" FORCE)

    # Find BiiGOptitrackControl Library
    find_package (BiiGOptitrackControl REQUIRED)
    set(External_Libraries_Install ${External_Libraries_Install} ${BiiGOptitrackControl_INSTALL_DIR}/bin/BiiGOptitrackControl${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(External_Libraries_Install ${External_Libraries_Install} ${BiiGOptitrackControl_INSTALL_DIR}/lib/BiiGOptitrackControl${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} ${BiiGOptitrackControl_INSTALL_DIR}/inc CACHE INTERNAL "" FORCE )

    # Find NPTrackingTools Library
    find_package(NPTrackingTools REQUIRED)
    set(External_Libraries_Install ${External_Libraries_Install} ${NPTrackingTools_INSTALL_DIR}/bin/${NPTrackingTools_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(External_Libraries_Install ${External_Libraries_Install} ${NPTrackingTools_INSTALL_DIR}/lib/${NPTrackingTools_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} ${NPTrackingTools_INCLUDE_DIR} CACHE INTERNAL "" FORCE )

    set(PlusDataCollection_LIBS ${PlusDataCollection_LIBS} BiiGOptitrackControl ${NPTrackingTools_LIBRARY})
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Optimet ConoProbe support
IF (PLUS_USE_OPTIMET_CONOPROBE)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    Optimet/vtkPlusOptimetConoProbeMeasurer.cxx
    )

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} ${OPTIMETSMART32SDK_INCLUDE_DIR} CACHE INTERNAL "" FORCE )

  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      Optimet/vtkPlusOptimetConoProbeMeasurer.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Optimet 
    CACHE INTERNAL "" FORCE)
    
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
  ${OPTIMETSMART32SDK_LIBRARY}
  )

  SET ( External_Libraries_Install ${External_Libraries_Install}
  ${OPTIMETSMART32SDK_BINARY_DIR}/Smart32${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
  SET ( External_Libraries_Install ${External_Libraries_Install}
  ${OPTIMETSMART32SDK_BINARY_DIR}/NewProbeDialog${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
ENDIF()

# --------------------------------------------------------------------------
# NDI Certus support
IF (PLUS_USE_NDI_CERTUS)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    NDICertusTracking/vtkPlusNDICertusTracker.cxx
    )

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} ${NDIOAPI_INCLUDE_DIR} CACHE INTERNAL "" FORCE )

  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      NDICertusTracking/vtkPlusNDICertusTracker.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/NDICertusTracking 
    CACHE INTERNAL "" FORCE)
          
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    ${NDIOAPI_LIBRARY}
    vtkndicapi # uses ndicapi's math functions
    )
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${NDIOAPI_BINARY_DIR}/oapi${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

#--------------------------------------------------------------------------
# NDI COMMON API support
IF (PLUS_USE_NDI)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    PolarisTracking/vtkPlusNDITracker.cxx
    )

  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      PolarisTracking/vtkPlusNDITracker.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/PolarisTracking
    CACHE INTERNAL "" FORCE)

  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} vtkndicapi )
ENDIF()

#--------------------------------------------------------------------------
# MICRONTRACKER support
IF (PLUS_USE_MICRONTRACKER)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    MicronTracking/vtkPlusMicronTracker.cxx
  )
  
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      MicronTracking/vtkPlusMicronTracker.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicronTracking 
    CACHE INTERNAL "" FORCE)
   
  INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
  
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    vtkPlusMicronTrackerInterface
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
ENDIF()

#--------------------------------------------------------------------------
# INTELREALSENSE support
IF (PLUS_USE_INTELREALSENSE)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    IntelRealSense/vtkPlusIntelRealSenseTracker.cxx
  )
  
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      IntelRealSense/vtkPlusIntelRealSenseTracker.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/IntelRealSense
    ${INTELREALSENSE_INCLUDE_DIR}
    CACHE INTERNAL "" FORCE)
  
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    optimized ${INTELREALSENSE_RELEASE_LIB}
    debug ${INTELREALSENSE_DEBUG_LIB}
    )

  # TODO: may need to install DLLs
ENDIF()

#--------------------------------------------------------------------------
# BrachyTracker support
IF (PLUS_USE_BRACHY_TRACKER)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    BrachyTracking/vtkPlusBrachyTracker.cxx
    BrachyTracking/PlusCmsBrachyStepper.cxx
    BrachyTracking/PlusCivcoBrachyStepper.cxx
  )
  
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS
      ${PlusDataCollection_HDRS}
      BrachyTracking/vtkPlusBrachyTracker.h
      BrachyTracking/PlusCmsBrachyStepper.h 
      BrachyTracking/PlusBrachyStepper.h 
      BrachyTracking/PlusCivcoBrachyStepper.h
      )
  ENDIF()
    
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/BrachyTracking
    ${USDIGITAL_SEI_INCLUDE_DIR}
    CACHE INTERNAL "" FORCE)
  
  ADD_LIBRARY(USDigitalEncoders SHARED IMPORTED)
  SET_PROPERTY(TARGET USDigitalEncoders PROPERTY IMPORTED_IMPLIB ${USDIGITAL_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET USDigitalEncoders PROPERTY IMPORTED_LOCATION ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX})

  SET ( PlusDataCollection_LIBS ${PlusDataCollection_LIBS} USDigitalEncoders )
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${USDIGITAL_SEI_BINARY_DIR}/SEISrv32.exe
    )
ENDIF()

#--------------------------------------------------------------------------
# USDigitalEncoders tracking support
IF (PLUS_USE_USDIGITALENCODERS_TRACKER)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    USDigitalEncodersTracking/vtkPlusUSDigitalEncodersTracker.cxx
  )
  
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS
      ${PlusDataCollection_HDRS}
      USDigitalEncodersTracking/vtkPlusUSDigitalEncodersTracker.h
      )
  ENDIF()
    
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/USDigitalEncodersTracking
    ${USDIGITAL_SEI_INCLUDE_DIR}
    CACHE INTERNAL "" FORCE)
  
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    ${USDIGITAL_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${USDIGITAL_SEI_BINARY_DIR}/SEISrv32.exe
    )
ENDIF()

#--------------------------------------------------------------------------
# Ascension 3DG support (trakSTAR)
IF( PLUS_USE_Ascension3DG )
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    Ascension3DGTracking/vtkPlusAscension3DGTracker.cxx 
    )
    
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} 
      Ascension3DGTracking/vtkPlusAscension3DGTracker.h 
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${ATC_TRAKSTAR_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    CACHE INTERNAL "" FORCE)

  ADD_LIBRARY(Ascension3DG SHARED IMPORTED)
  IF ( (MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64") OR MINGW )
    SET(Ascension3DG_LIB_FILE ${ATC_TRAKSTAR_LIB_DIR}/ATC3DG64${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "Lib file for Ascension3DG.")
    SET(Ascension3DG_SHARED_LIB_FILE ${ATC_TRAKSTAR_BINARY_DIR}/ATC3DG64${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "DLL file for Ascension3DG.")
  ELSE()
    SET(Ascension3DG_LIB_FILE ${ATC_TRAKSTAR_LIB_DIR}/ATC3DG${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "Lib file for Ascension3DG.")
    SET(Ascension3DG_SHARED_LIB_FILE ${ATC_TRAKSTAR_BINARY_DIR}/ATC3DG${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "DLL file for Ascension3DG.")
  ENDIF()
  
  SET(PlusDataCollection_LIBS ${PlusDataCollection_LIBS} Ascension3DG)
  SET(External_Libraries_Install ${External_Libraries_Install} ${Ascension3DG_SHARED_LIB_FILE})
  SET_PROPERTY(TARGET Ascension3DG PROPERTY IMPORTED_IMPLIB ${Ascension3DG_LIB_FILE})
  SET_PROPERTY(TARGET Ascension3DG PROPERTY IMPORTED_LOCATION ${Ascension3DG_SHARED_LIB_FILE})
ENDIF()

#--------------------------------------------------------------------------
# Ascension 3DGm support (medSAFE)
IF( PLUS_USE_Ascension3DGm )
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} Ascension3DGTracking/vtkAscension3DGmTracker.cxx )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} Ascension3DGTracking/vtkAscension3DGmTracker.h )
  ENDIF()
  
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${ATC_MEDSAFE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    CACHE INTERNAL "" FORCE)

  ADD_LIBRARY(Ascension3DGm SHARED IMPORTED)
  SET_PROPERTY(TARGET Ascension3DG PROPERTY IMPORTED_IMPLIB ${ATC_MEDSAFE_LIB_DIR}/ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET Ascension3DG PROPERTY IMPORTED_LOCATION ${ATC_MEDSAFE_BINARY_DIR}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX} )

  SET ( PlusDataCollection_LIBS ${PlusDataCollection_LIBS} Ascension3DGm )
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${ATC_MEDSAFE_BINARY_DIR}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
ENDIF()

#--------------------------------------------------------------------------
# PhidgetSpatial 3/3/3 MARG sensor (magnetometer, accelerometer, gyroscope)
IF( PLUS_USE_PHIDGET_SPATIAL_TRACKER )
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    PhidgetSpatialTracking/vtkPlusPhidgetSpatialTracker.cxx 
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} PhidgetSpatialTracking/vtkPlusPhidgetSpatialTracker.h )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${PHIDGET_SPATIAL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/PhidgetSpatialTracking
    CACHE INTERNAL "" FORCE)

  ADD_LIBRARY(phidget SHARED IMPORTED)
  SET_PROPERTY(TARGET phidget PROPERTY IMPORTED_IMPLIB ${PHIDGET_SPATIAL_LIB_DIR}/phidget21${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET phidget PROPERTY IMPORTED_LOCATION ${PHIDGET_SPATIAL_BINARY_DIR}/phidget21${CMAKE_SHARED_LIBRARY_SUFFIX})

  SET ( PlusDataCollection_LIBS ${PlusDataCollection_LIBS} phidget vtkxio )
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${PHIDGET_SPATIAL_BINARY_DIR}/phidget21${CMAKE_SHARED_LIBRARY_SUFFIX} 
    )
ENDIF()

#--------------------------------------------------------------------------
# 3dConnexion SpaceNavigator and similar 3D mouse devices
IF( PLUS_USE_3dConnexion_TRACKER )
  IF (WIN32)  # the 3D mouse is currently supported on Windows only
    SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
      3dConnexionTracking/vtkPlus3dConnexionTracker.cxx
      )
    IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} 3dConnexionTracking/vtkPlus3dConnexionTracker.h )
    ENDIF()
    SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
      ${CMAKE_CURRENT_SOURCE_DIR}/3dConnexionTracking
      CACHE INTERNAL "" FORCE)
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# CH Robotics UM6 MARG sensor (magnetometer, accelerometer, gyroscope)
SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
  ChRobotics/vtkPlusChRoboticsTracker.cxx
  )
IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} ChRobotics/vtkPlusChRoboticsTracker.h )
ENDIF()

SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}/ChRobotics
  CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------
# Microchip MARG sensor (magnetometer, accelerometer, gyroscope)
SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
  MicrochipTracking/vtkPlusMicrochipTracker.cxx
  )
IF (WIN32)
  SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS} MicrochipTracking/vtkPlusMicrochipTracker.h )
ENDIF()

SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}/MicrochipTracking
  CACHE INTERNAL "" FORCE)

#--------------------------------------------------------------------------
# OpenIGTLink 
IF( PLUS_USE_OpenIGTLink )
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    OpenIGTLink/vtkPlusOpenIGTLinkDevice.cxx 
    OpenIGTLink/vtkPlusOpenIGTLinkTracker.cxx 
    OpenIGTLink/vtkPlusOpenIGTLinkVideoSource.cxx
    )
    
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      OpenIGTLink/vtkPlusOpenIGTLinkDevice.h
      OpenIGTLink/vtkPlusOpenIGTLinkTracker.h 
      OpenIGTLink/vtkPlusOpenIGTLinkVideoSource.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLink
    ${OpenIGTLink_INCLUDE_DIRS}
    CACHE INTERNAL "" FORCE)
  
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} OpenIGTLink vtkPlusOpenIGTLink )
ENDIF()

# --------------------------------------------------------------------------
# Ultrasonix support
IF (PLUS_USE_ULTRASONIX_VIDEO)
  SET (PlusDataCollection_SRCS
    ${PlusDataCollection_SRCS}
    SonixVideo/vtkPlusSonixVideoSource.cxx
    SonixVideo/vtkPlusSonixVolumeReader.cxx
    SonixVideo/itkUlteriusImageIO.cxx
    SonixVideo/itkUlteriusImageIOFactory.cxx   
    SonixVideo/vtkPlusSonixPortaVideoSource.cxx
    )
    
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS
      ${PlusDataCollection_HDRS}
      SonixVideo/vtkPlusSonixVideoSource.h
      SonixVideo/vtkPlusSonixVolumeReader.h
      SonixVideo/itkUlteriusImageIO.h
      SonixVideo/itkUlteriusImageIOFactory.h
      SonixVideo/vtkPlusSonixPortaVideoSource.h
      )
  ENDIF()

  MESSAGE(STATUS "Ultrasonix SDK directory: ${ULTRASONIX_SDK_DIR}")
  IF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 1)
    # Use Ultrasonix SDK 1.2 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
    SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)
    SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
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
    SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
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
    IF(PLUS_ULTRASONIX_SDK_MAJOR_VERSION GREATER 5)
      SET(PORTA_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/porta/inc ${ULTRASONIX_SDK_DIR}/porta/wrapper CACHE PATH "Path to the Ultrasonix SDK Porta include directory." FORCE)
    ELSE()
      SET(PORTA_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/porta/inc CACHE PATH "Path to the Ultrasonix SDK Porta include directory." FORCE)
    ENDIF()
    SET(PORTA_LIB_DIR ${ULTRASONIX_SDK_DIR}/porta/lib CACHE PATH "Path to the Ultrasonix SDK Porta library directory." FORCE)

    SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
      ${PORTA_LIB_DIR}/porta.lib
      )
    IF (PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 6)

      IF (PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 0)
        MESSAGE( FATAL_ERROR "Plus does not support Ultrasonix SDK version 6.0.x")
      ENDIF()

      SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} ${ULTERIUS_LIB_DIR}/ulterius_old.lib)
    ELSE()
      SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} ${ULTERIUS_LIB_DIR}/ulterius.lib)
    ENDIF()

    FILE(GLOB ULTRASONIX_SDK_SHARED_LIBRARIES "${ULTRASONIX_SDK_BINARY_DIR}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}")

    SET ( External_Libraries_Install ${External_Libraries_Install} ${ULTRASONIX_SDK_SHARED_LIBRARIES} )
    SET ( External_Libraries_Debug ${External_Libraries_Debug} ${ULTRASONIX_SDK_SHARED_LIBRARIES} )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${ULTERIUS_INCLUDE_DIR}        
    ${PORTA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SonixVideo 
    )
ENDIF()

# --------------------------------------------------------------------------
# BK ProFocus support

# OEM interface
IF (PLUS_USE_BKPROFOCUS_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    BkProFocus/vtkPlusBkProFocusOemVideoSource.cxx 
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      BkProFocus/vtkPlusBkProFocusOemVideoSource.h
      )
  ENDIF()
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${GRABBIELIB_SOURCE_DIR}/AcquisitionLibrary
    ${GRABBIELIB_SOURCE_DIR}/BkmDataFileLib
    ${GRABBIELIB_SOURCE_DIR}/cfuff
    ${GRABBIELIB_SOURCE_DIR}/CommandAndControl
    ${GRABBIELIB_SOURCE_DIR}/LogLibrary
    ${GRABBIELIB_SOURCE_DIR}/OemParams
    ${GRABBIELIB_SOURCE_DIR}/ParametersLibrary        
    ${CMAKE_CURRENT_SOURCE_DIR}/BkProFocus
    )
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    GrabbieLib
    ${VTK_PNG_LIBRARIES}
    )

  # CameraLink interface
  IF (PLUS_USE_BKPROFOCUS_CAMERALINK)
    SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
      BkProFocus/vtkPlusBkProFocusCameraLinkVideoSource.cxx
      BkProFocus/PlusBkProFocusCameraLinkReceiver.cxx
      )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      BkProFocus/vtkPlusBkProFocusCameraLinkVideoSource.h
      BkProFocus/PlusBkProFocusCameraLinkReceiver.h
      )
  ENDIF()
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    ${DALSASAPERA_LIB_DIR}/SapClassBasic${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# ICCapturing support
IF (PLUS_USE_ICCAPTURING_VIDEO)
  SET (PlusDataCollection_SRCS
    ${PlusDataCollection_SRCS}
    ICCapturing/vtkPlusICCapturingSource.cxx
    ICCapturing/ICCapturingListener.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS
      ${PlusDataCollection_HDRS}
      ICCapturing/vtkPlusICCapturingSource.h
      ICCapturing/ICCapturingListener.h 
      )
  ENDIF()
  # Look for ICCapturing libraries   
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${ICCAPTURING_INCLUDE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ICCapturing
    )
  
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    optimized ${ICCAPTURING_TIS_UDSHL_STATIC_LIB}
    debug ${ICCAPTURING_TIS_UDSHLD_STATIC_LIB}
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${ICCAPTURING_TIS_UDSHL_SHARED_LIB}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug} 
    ${ICCAPTURING_TIS_UDSHLD_SHARED_LIB}
    )
ENDIF()

# --------------------------------------------------------------------------
# StealthLink2.0 suppport
IF (PLUS_USE_STEALTHLINK)
  SET (PlusDataCollection_SRCS
    ${PlusDataCollection_SRCS}
    StealthLink/vtkPlusStealthLinkTracker.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS
      ${PlusDataCollection_HDRS}
      StealthLink/vtkPlusStealthLinkTracker.h
      )
  ENDIF()
  
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${STEALTHLINK_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/StealthLink
    )
  
  IF (WIN32)
    SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
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
    SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
      ${STEALTHLINK_STEALTHLINK_STATIC_LIBRARY}
      )
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# da Vinci Surgical System support
IF(PLUS_USE_IntuitiveDaVinci)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    IntuitiveDaVinci/vtkPlusIntuitiveDaVinciTracker.cxx
    IntuitiveDaVinci/IntuitiveDaVinci.cxx
    )

  SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
    IntuitiveDaVinci/vtkPlusIntuitiveDaVinciTracker.h 
    IntuitiveDaVinci/IntuitiveDaVinci.h
    )

  SET(PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${IntuitiveDaVinci_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/IntuitiveDaVinci
    CACHE INTERNAL "" FORCE)
      
  SET(PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    ${IntuitiveDaVinci_LIBRARY}
    ws2_32
    iphlpapi
    )
ENDIF()

# --------------------------------------------------------------------------
# Video-for-Windows video digitizer support
IF (PLUS_USE_VFW_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    VideoForWindows/vtkPlusWin32VideoSource2.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      VideoForWindows/vtkPlusWin32VideoSource2.h
      )
  ENDIF()
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    ws2_32
    )
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoForWindows   
    )
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    Vfw32.lib
    )
ENDIF()

# --------------------------------------------------------------------------
# Microsoft Media Foundation video digitizer support
IF (PLUS_USE_MMF_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    MicrosoftMediaFoundation/vtkPlusMmfVideoSource.cxx 
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      MicrosoftMediaFoundation/vtkPlusMmfVideoSource.h 
      )
  ENDIF()
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/MicrosoftMediaFoundation
    ${WINDOWS_SDK_INCLUDE_DIRS}
    )
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    MfVideoCaptureLibrary
    )
ENDIF()

# --------------------------------------------------------------------------
# Epiphan support
IF (PLUS_USE_EPIPHAN)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    Epiphan/vtkPlusEpiphanVideoSource.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      Epiphan/vtkPlusEpiphanVideoSource.h
      )
  ENDIF()
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}     
    ${EPIPHAN_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Epiphan
    )
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug} 
    ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

  # Create a target for proper CMake infrastructure
  ADD_LIBRARY(Epiphan SHARED IMPORTED)
  SET_PROPERTY(TARGET Epiphan PROPERTY IMPORTED_IMPLIB ${EPIPHAN_LIB_DIR}/frmgrab${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET Epiphan PROPERTY IMPORTED_LOCATION ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX})
  
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    Epiphan
  )
ENDIF()

# --------------------------------------------------------------------------
# Capistrano Labs support
IF (PLUS_USE_CAPISTRANO_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    Capistrano/vtkPlusCapistranoVideoSource.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      Capistrano/vtkPlusCapistranoVideoSource.h
      )
  ENDIF()
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    ${CAPISTRANO_LIBRARY_DIR}/BmodeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CAPISTRANO_LIBRARY_DIR}/USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}     
    ${CAPISTRANO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Capistrano
    )
  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${CAPISTRANO_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${CAPISTRANO_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug}
    ${CAPISTRANO_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${CAPISTRANO_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# Interson support
IF (PLUS_USE_INTERSON_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    Interson/vtkPlusIntersonVideoSource.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      Interson/vtkPlusIntersonVideoSource.h
      )
  ENDIF()
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    ${INTERSON_LIBRARY_DIR}/BmodeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${INTERSON_LIBRARY_DIR}/USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}     
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
ENDIF()

# --------------------------------------------------------------------------
# Interson SDK C++ support
IF (PLUS_USE_INTERSONSDKCXX_VIDEO)
  FIND_PACKAGE( IntersonSDKCxx REQUIRED )
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    IntersonSDKCxx/vtkPlusIntersonSDKCxxVideoSource.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      IntersonSDKCxx/vtkPlusIntersonSDKCxxVideoSource.h
      )
  ENDIF()
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    ${IntersonSDKCxx_LIBRARIES}
    )
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}     
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
ENDIF()

# --------------------------------------------------------------------------
# Telemed support
IF (PLUS_USE_TELEMED_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    Telemed/vtkPlusTelemedVideoSource.cxx
    Telemed/TelemedUltrasound.cxx
    Telemed/Usgfw2_i.c
    Telemed/Usgfw2_p.c
    Telemed/dlldata.c
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      Telemed/vtkPlusTelemedVideoSource.h
      Telemed/TelemedUltrasound.h
      Telemed/Usgfw2_h.h
      )
  ENDIF()

  # Find Telemed SDK include directory
  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${TELEMED_INCLUDE_DIR}
    ${WINDOWS_SDK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Telemed
    )
ENDIF()

# --------------------------------------------------------------------------
# Thorlabs support
IF (PLUS_USE_THORLABS_VIDEO)
  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    ThorLabs/vtkPlusThorLabsVideoSource.cxx
    )
  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      ThorLabs/vtkPlusThorLabsVideoSource.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThorLabs  
    ${THORLABS_INCLUDE_DIR}
    )
  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS} 
    ${THORLABS_LIBRARY_DIR}/TLCCS_32${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${THORLABS_LIBRARY_DIR}/visa32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# Philips ie33 3d ultrasound support
IF (PLUS_USE_PHILIPS_3D_ULTRASOUND)
  IF(PLUS_USE_ULTRASONIX_VIDEO)
    # Ultrasonix contains its own libmmd.dll which does not support all the functionality needed for the philips probe
    # They both cannot be enabled at the same time.
    MESSAGE( FATAL_ERROR "Plus cannot enable both Ultrasonix and Philips devices due to .dll conflicts in their respective SDK packages.")
  ELSE(PLUS_USE_ULTRASONIX_VIDEO)
    SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
      Philips3D/vtkPlusPhilips3DProbeVideoSource.cxx 
      Philips3D/vtkPlusIEEListener.cxx 
      )

    IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
        Philips3D/vtkPlusPhilips3DProbeVideoSource.h
        Philips3D/vtkPlusIEEListener.h
        )
    ENDIF()

    SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS} 
      ${CMAKE_CURRENT_SOURCE_DIR}/Philips3D
      ${Philips_INCLUDE_DIRS}
      CACHE INTERNAL "" FORCE)

    SET( PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
      ${Philips_LIBRARY_DIR}/PipelineComDll${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${Philips_LIBRARY_DIR}/3DData${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${Philips_LIBRARY_DIR}/3DUtil${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  
    FILE(GLOB PHILIPS_SDK_SHARED_LIBRARIES "${Philips_BINARY_DIRS}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}")

    SET ( External_Libraries_Install ${External_Libraries_Install}
      ${PHILIPS_SDK_SHARED_LIBRARIES}
      )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Try to find tesseract and include its settings (otherwise complain)
IF(PLUS_USE_tesseract)
  FIND_PACKAGE(tesseract NO_MODULE REQUIRED PATHS ${tesseract_DIR})

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${tesseract_INCLUDE_DIRS}
    )

  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    ${tesseract_LIBRARIES}
    vtkzlib vtkpng
    )
ENDIF()

# --------------------------------------------------------------------------
# Include the NVidia DVP classes
IF(PLUS_USE_NVIDIA_DVP)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/NVidiaDVP/nvConfigure.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/nvConfigure.h
    )

  SET(NVidiaDVP_SRCS)
  SET(NVidiaDVP_HDRS ${CMAKE_CURRENT_BINARY_DIR}/nvConfigure.h)

  IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET (NVidiaDVP_SRCS ${NVidiaDVP_SRCS} 
      NVidiaDVP/vtkPlusNVidiaDVPVideoSourceWin32.cxx
      )

    IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      SET (NVidiaDVP_HDRS ${NVidiaDVP_HDRS}
        NVidiaDVP/vtkPlusNVidiaDVPVideoSourceWin32.h
        )
    ENDIF()
  ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET (NVidiaDVP_SRCS ${NVidiaDVP_SRCS} 
      NVidiaDVP/vtkPlusNVidiaDVPVideoSourceLinux.cxx
      )
  ENDIF()
  
  SET (NVidiaDVP_SRCS ${NVidiaDVP_SRCS} 
    NVidiaDVP/glExtensions.cpp
    NVidiaDVP/nvGPUutil.cpp
    NVidiaDVP/nvSDIin.cpp
    NVidiaDVP/nvSDIout.cpp
    NVidiaDVP/nvSDIutil.cpp
    )

  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (NVidiaDVP_HDRS ${NVidiaDVP_HDRS}
      NVidiaDVP/nvGPUutil.h
      NVidiaDVP/nvSDIin.h
      NVidiaDVP/nvSDIout.h
      NVidiaDVP/glExtensions.h
      NVidiaDVP/nvSDIutil.h
      NVidiaDVP/nvCommon.h
      )
  ENDIF()
  
  SET(PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
    ${NVidiaDVP_HDRS}
    )

  SET(PlusDataCollection_SRCS ${PlusDataCollection_SRCS}
    ${NVidiaDVP_SRCS}
    )

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/NVidiaDVP
    )

  SET ( External_Libraries_Install ${External_Libraries_Install}
    ${NVIDIA_DVP_BINARY_DIR}/dvp${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  SET ( External_Libraries_Debug ${External_Libraries_Debug}
    ${NVIDIA_DVP_BINARY_DIR}/dvp${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

  IF( BUILD_SHARED_LIBS )
    # Create a target for proper CMake infrastructure
    ADD_LIBRARY(NVidiaDVP SHARED IMPORTED)
    SET_PROPERTY(TARGET NVidiaDVP PROPERTY IMPORTED_IMPLIB ${NVIDIA_DVP_LIB_DIR}/dvp${CMAKE_STATIC_LIBRARY_SUFFIX})
    SET_PROPERTY(TARGET NVidiaDVP PROPERTY IMPORTED_LOCATION ${NVIDIA_DVP_BINARY_DIR}/dvp${CMAKE_SHARED_LIBRARY_SUFFIX})
    SET_PROPERTY(TARGET NVidiaDVP PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${NVIDIA_DVP_INCLUDE_DIR};${OPENGL_INCLUDE_DIR};${PlusCommon_INCLUDE_DIRS}")
  ELSE()
    # What to do here?
  ENDIF()

  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    NVidiaDVP
    QuadroSDI
    ${OPENGL_LIBRARIES}
    )
ENDIF()

# --------------------------------------------------------------------------
# OvrvisionPro
IF (PLUS_USE_OvrvisionPro)
  FIND_PACKAGE(OvrvisionPro REQUIRED NO_MODULE)

  SET (PlusDataCollection_SRCS ${PlusDataCollection_SRCS} 
    OvrvisionPro/vtkPlusOvrvisionProVideoSource.cxx
    )

  IF (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET (PlusDataCollection_HDRS ${PlusDataCollection_HDRS}
      OvrvisionPro/vtkPlusOvrvisionProVideoSource.h
      )
  ENDIF()

  SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
    ${OvrvisionPro_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/OvrvisionPro
    )

  SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
    OvrvisionPro
    )
ENDIF()

# --------------------------------------------------------------------------
# Build the library
SET (PlusDataCollection_LIBS ${PlusDataCollection_LIBS}
  ${VTK_LIBRARIES}
  vtkPlusCommon  
  ${ITK_LIBRARIES}
  vtkPlusUsSimulator
  vtkPlusVolumeReconstruction
  vtkPlusImageProcessing
  )

SET (PlusDataCollection_INCLUDE_DIRS ${PlusDataCollection_INCLUDE_DIRS}
  ${PlusUsSimulator_INCLUDE_DIRS}
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtkPlusDataCollection)
ADD_LIBRARY(vtkPlusDataCollection ${PlusDataCollection_SRCS} ${PlusDataCollection_HDRS})
target_include_directories(vtkPlusDataCollection PUBLIC ${PlusDataCollection_INCLUDE_DIRS} )

IF(MSVC)
  # enable parallel in-target build as data collection is the largest project by far
  # do not add to interface defines as downstream targets don't necessarily need MP
  target_compile_options(vtkPlusDataCollection PRIVATE /MP)
ENDIF()

TARGET_LINK_LIBRARIES(vtkPlusDataCollection ${PlusDataCollection_LIBS})

# --------------------------------------------------------------------------
# Source groups
source_group("NVidiaDVP" FILES ${NVidiaDVP_HDRS} ${NVidiaDVP_SRCS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusDataCollection CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusDataCollection vtkPlusDataCollection;"${PlusLib_BINARY_DIR}/src/PlusDataCollection/vtkPlusDataCollection.vcproj";vtkPlusCommon;vtkPlusUsSimulator;vtkPlusImageProcessing CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
  FILE(COPY ${External_Libraries_Install}
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      )

  # If debug and release dll has the same name then the debug dll would be
  # overwritten by the file copy. To prevent this, copy only those release dlls
  # that has no corresponding debug dll with the same filename.
  SET (External_Libraries_Install_Minus_Debug)
  SET (External_Libraries_Debug_FileName)

  # Find the file names of the debug shared libraries
  FOREACH (DEBUG ${External_Libraries_Debug})
    get_filename_component(FileName ${DEBUG} NAME)
    SET(External_Libraries_Debug_FileName ${External_Libraries_Debug_FileName} ${FileName})
  ENDFOREACH()

  # Extract the release shared libraries that have the same name as debug shared libraries
  FOREACH (RELEASE ${External_Libraries_Install})
    get_filename_component(FileName ${RELEASE} NAME)
    list(FIND External_Libraries_Debug_FileName ${FileName} HasTheFileName)
    IF(HasTheFileName EQUAL -1)
        SET(External_Libraries_Install_Minus_Debug ${External_Libraries_Install_Minus_Debug} ${RELEASE})
    ENDIF()
  ENDFOREACH()

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
# Tools
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_EXECUTABLE(ViewSequenceFile Tools/ViewSequenceFile.cxx )
  TARGET_LINK_LIBRARIES(ViewSequenceFile vtkPlusDataCollection )
  GENERATE_HELP_DOC(ViewSequenceFile)

  ADD_EXECUTABLE(PlusVersion Tools/PlusVersion.cxx )
  TARGET_LINK_LIBRARIES(PlusVersion vtkPlusCommon vtkPlusDataCollection)
  GENERATE_HELP_DOC(PlusVersion)
  
  INSTALL(TARGETS PlusVersion ViewSequenceFile
    RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )
ENDIF()

# --------------------------------------------------------------------------
# Install
INSTALL(FILES ${External_Libraries_Install}
  DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  )
  
INSTALL(TARGETS vtkPlusDataCollection
  RUNTIME DESTINATION bin CONFIGURATIONS Release COMPONENT RuntimeLibraries
  LIBRARY DESTINATION lib CONFIGURATIONS Release COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION lib CONFIGURATIONS Release COMPONENT Development
  )
  
# --------------------------------------------------------------------------
# Testing
IF (BUILD_TESTING)
  SUBDIRS (Testing)
ENDIF()
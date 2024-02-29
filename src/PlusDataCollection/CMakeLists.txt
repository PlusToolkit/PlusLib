PROJECT(PlusDataCollection)

# --------------------------------------------------------------------------
# Options
OPTION(PLUS_USE_OPTITRACK "Provide support for the OptiTrack tracking system" OFF)
OPTION(PLUS_USE_OPTIMET_CONOPROBE "Provide support for the Optimet ConoProbe" OFF)

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

OPTION(PLUS_USE_MICRONTRACKER "Provide support for the Claron MicronTracker" OFF)
OPTION(PLUS_USE_BRACHY_TRACKER "Provide support for the brachy stepper tracking device" OFF)
OPTION(PLUS_USE_USDIGITALENCODERS_TRACKER "Provide support for multiple USDigital encoders tracking device" OFF)
OPTION(PLUS_USE_Ascension3DG "Provide support for the Ascension 3DG Tracker" ${ENABLED_BY_DEFAULT_ON_WINDOWS_ONLY})
OPTION(PLUS_USE_Ascension3DGm "Provide support for the Ascension 3DGm MedSafe Tracker" ${ENABLED_BY_DEFAULT_ON_WINDOWS32_ONLY})
OPTION(PLUS_USE_PHIDGET_SPATIAL_TRACKER "Provide support for the Phidget Spatial accelerometer" OFF)
OPTION(PLUS_USE_3dConnexion_TRACKER "Provide support for the 3dConnexion 3d mouse" OFF)
OPTION(PLUS_USE_ULTRASONIX_VIDEO "Provide support for Ultrasonix ultrasound scanners" OFF)
OPTION(PLUS_USE_BKPROFOCUS_VIDEO "Provide support for BK ProFocus ultrasound scanners through the OEM (TCP/IP) interface" OFF)
OPTION(PLUS_USE_BKPROFOCUS_CAMERALINK "Provide support for BK ProFocus ultrasound scanners through CameraLink interface" OFF)
OPTION(PLUS_USE_ICCAPTURING_VIDEO "Provide support for the IC framegrabber device" OFF)
OPTION(PLUS_USE_VFW_VIDEO "Provide support for the Video-for-Windows video digitizer" OFF)
OPTION(PLUS_USE_EPIPHAN "Provide support for the Epiphan" OFF)
OPTION(PLUS_USE_CAPISTRANO_VIDEO "Provide support Capistrano Labs USB ultrasound probes" OFF)
OPTION(PLUS_USE_WINPROBE_VIDEO "Provide support WinProbe ultrasound systems" OFF)
OPTION(PLUS_USE_INTERSON_VIDEO "Provide support Interson USB ultrasound probes" OFF)
OPTION(PLUS_USE_INTERSONSDKCXX_VIDEO "Provide support Interson SDK 1.X with C++ Wrapper USB ultrasound probes" OFF)
OPTION(PLUS_USE_INTERSONARRAYSDKCXX_VIDEO "Provide support Interson Array SDK 2.X with C++ Wrapper USB ultrasound probes" OFF)
OPTION(PLUS_USE_TELEMED_VIDEO "Provide support for the Telemed ultrasound probes" OFF)
OPTION(PLUS_USE_SPINNAKER_VIDEO "Provide support for Point Grey Spinnaker API compatible imaging devices" OFF)
OPTION(PLUS_USE_THORLABS_VIDEO "Provide support for the ThorLabs compact spectrometers" OFF)
OPTION(PLUS_USE_STEALTHLINK "Provide support for the Medtronick StealthLink Server" OFF)
OPTION(PLUS_USE_IntuitiveDaVinci "Enable the tracking of the da Vinci Surgical System. Enable this only if you have a da Vinci that is accessible from this computer." OFF)
OPTION(PLUS_USE_PHILIPS_3D_ULTRASOUND "Provide support for the Philips ie33 3D ultrasound probe system." OFF)
OPTION(PLUS_TEST_BKPROFOCUS "Enable testing of acquisition from BK ProFocus ultrasound systems. Enable this only if a BK ProFocus device is connected to this computer." OFF)
OPTION(PLUS_USE_AGILENT "Provide support for the Agilent PCI digitizer." OFF)
OPTION(PLUS_USE_OPTICAL_MARKER_TRACKER "Provide support of fiducial marker tracking. Requires PLUS_USE_OpenCV." OFF)
OPTION(PLUS_USE_OPENHAPTICS "Provide support for OpenHaptics devices." OFF)
OPTION(PLUS_USE_BLACKMAGIC_DECKLINK "Provide support for BlackMagic DeckLink capture cards." OFF)
OPTION(PLUS_USE_INFRARED_SEEK_CAM "Provide support for capturing an Infrared Seek Cam" OFF)
OPTION(PLUS_USE_ANDOR_CAMERA "Provide support for capturing images from Andor Camera. Requires PLUS_USE_OpenCV." OFF)
OPTION(PLUS_USE_INFRARED_TEQ1_CAM "Provide support for capturing an Infrared Therma Expert Q1 Camera capture stream" OFF)
OPTION(PLUS_USE_INFRARED_TEEV2_CAM "Provide support for capturing an Infrared Thermal Expert EV2 Camera capture stream" OFF)
OPTION(PLUS_USE_ULTRAVIOLET_PCOUV_CAM "Provide support for capturing an PCO Ultraviolet Camera capture stream" OFF)
OPTION(PLUS_USE_DAQVIDEOSOURCE "Provide support for capturing an CameraLink Camera using DAQ System USB3-FRM13-B Grabber"  OFF)
OPTION(PLUS_USE_DAQMMFCOMBINED_VIDEO "Provide support for capturing an CameraLink Camera using DAQ System USB3-FRM13-B Grabber and MMF Video"  OFF)
OPTION(PLUS_USE_FLIRSPINNAKER_CAM "Provide support for capturing an FLIR Spinnaker Camera capture stream" OFF)
OPTION(PLUS_USE_WITMOTIONTRACKER "Provide support for the WitMotion class of inertial measurement units" OFF)

OPTION(PLUS_USE_LEAPMOTION "Provide support for the LeapMotion hand tracker" OFF)
IF(PLUS_USE_LEAPMOTION)
  OPTION(PLUS_TEST_LEAPMOTION "Enable testing for LeapMotion device" OFF)
ENDIF()
OPTION(PLUS_USE_CLARIUS "Provide support for Clarius ultrasound probes" OFF)
OPTION(PLUS_USE_CLARIUS_OEM "Provide support for Clarius OEM SDK interface to Clarius Ultrasound devices." OFF)
OPTION(PLUS_USE_STEAMVR "Provide support for the SteamVR tracking system" OFF)
OPTION(PLUS_USE_PICOSCOPE "Provide support for PicoScope 2xxx series oscilloscopes" OFF)
OPTION(PLUS_USE_AZUREKINECT "Provide support the Kinect Azure RGB-D camera" OFF)
OPTION(PLUS_USE_REVOPOINT3DCAMERA "Provide support Revopoint 3D cameras" OFF)
OPTION(PLUS_USE_GENERIC_SENSOR_TRACKER "Provide support for generic sensor data collection" OFF)

OPTION(PLUS_TEST_ULTRASONIX "Enable testing of acquisition from Ultrasonix ultrasound systems. Enable this only if an Ultrasonix device accessible from this computer." OFF)
IF(PLUS_TEST_ULTRASONIX)
  SET(PLUS_TEST_ULTRASONIX_IP_ADDRESS "127.0.0.1" CACHE STRING "IP address of the Ultrasonix scanner that is used during testing")
ENDIF()

OPTION(PLUS_USE_MMF_VIDEO "Provide support for the Microsoft Media Foundation video digitizers (requires installation of Windows Platform SDK 7.1 or later)" OFF)
IF(PLUS_USE_MMF_VIDEO)
  OPTION(PLUS_TEST_MMF_VIDEO "Enable testing of acquisition from MMF video device (webcam). Enable this only if an MMF device is connected to this computer." OFF)
ENDIF()

OPTION(PLUS_USE_TextRecognizer "Provide support for optical character recognition virtual device via tesseract OCR library." OFF)
IF(PLUS_USE_TextRecognizer)
  OPTION(PLUS_TEST_TextRecognizer "Enable testing functionality for tesseract virtual recognizer device." ON)
ENDIF()

OPTION(PLUS_USE_OpenCV "Provide support for optical marker tracking and other features using OpenCV." OFF)
OPTION(PLUS_USE_aruco "Provide support for optical marker tracking using aruco." OFF)
OPTION(PLUS_USE_OpenCV_VIDEO "Provide support for capturing an OpenCV capture stream" OFF)

OPTION(PLUS_USE_NVIDIA_DVP "Provide support for the NVidia Digital Video Pipeline" OFF)
IF(PLUS_USE_NVIDIA_DVP)
  FIND_PACKAGE(QuadroSDI REQUIRED)
  FIND_PACKAGE(OpenGL REQUIRED)
ENDIF()

OPTION(PLUS_USE_OvrvisionPro "Provide support for the OvrvisionPro Stereo Camera" OFF)
IF(PLUS_USE_OvrvisionPro)
  OPTION(PLUS_TEST_OvrvisionPro "Enable testing of acquisition of stereo images from the OvrvisionPro" OFF)
ENDIF()

IF(UNIX AND NOT APPLE)
  OPTION(PLUS_USE_V4L2 "Provide support for video4linux devices" OFF)
ENDIF()

# --------------------------------------------------------------------------
# Haptics
ADD_SUBDIRECTORY(Haptics)

SET(PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${vtkPlusHaptics_INCLUDE_DIRS} CACHE INTERNAL "" FORCE)
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusHaptics CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Sources
SET(Common_SRCS
  vtkPlusDataCollector.cxx
  vtkPlusDevice.cxx
  vtkPlusUsDevice.cxx
  vtkPlusChannel.cxx
  vtkPlusDeviceFactory.cxx
  vtkPlusDataSource.cxx
  vtkPlusTimestampedCircularBuffer.cxx
  PlusStreamBufferItem.cxx
  vtkPlusGenericSerialDevice.cxx
  PlusSerialLine.cxx
  vtkFcsvReader.cxx
  vtkFcsvWriter.cxx
  vtkPlusBuffer.cxx
  vtkPlusUsImagingParameters.cxx
  )
SET(Virtual_SRCS
  VirtualDevices/vtkPlusVirtualMixer.cxx
  VirtualDevices/vtkPlusVirtualSwitcher.cxx
  VirtualDevices/vtkPlusVirtualCapture.cxx
  VirtualDevices/vtkPlusVirtualVolumeReconstructor.cxx
  VirtualDevices/vtkPlusVirtualDeinterlacer.cxx
  )
SET(Miscellaneous_SRCS
  FakeTracking/vtkPlusFakeTracker.cxx
  SavedDataSource/vtkPlusSavedDataSource.cxx
  ImageProcessor/vtkPlusImageProcessorVideoSource.cxx
  UsSimulatorVideo/vtkPlusUsSimulatorVideoSource.cxx
  )

IF(PLUS_USE_TextRecognizer)
  SET(Virtual_SRCS ${Virtual_SRCS}
    VirtualDevices/vtkPlusVirtualTextRecognizer.cxx
    )
ENDIF()

LIST(APPEND ${PROJECT_NAME}_SRCS
  ${Common_SRCS}
  ${Virtual_SRCS}
  ${Miscellaneous_SRCS}
  )

SET(Common_HDRS
  vtkPlusDataCollector.h
  vtkPlusDevice.h
  vtkPlusUsDevice.h
  vtkPlusChannel.h
  vtkPlusDeviceFactory.h
  vtkPlusDataSource.h
  vtkPlusTimestampedCircularBuffer.h
  PlusStreamBufferItem.h
  vtkPlusGenericSerialDevice.h
  PlusSerialLine.h
  vtkFcsvReader.h
  vtkFcsvWriter.h
  vtkPlusBuffer.h
  vtkPlusUsImagingParameters.h
  )
SET(Miscellaneous_HDRS
  FakeTracking/vtkPlusFakeTracker.h
  SavedDataSource/vtkPlusSavedDataSource.h
  ImageProcessor/vtkPlusImageProcessorVideoSource.h
  UsSimulatorVideo/vtkPlusUsSimulatorVideoSource.h
  )
SET(Virtual_HDRS
  VirtualDevices/vtkPlusVirtualMixer.h
  VirtualDevices/vtkPlusVirtualSwitcher.h
  VirtualDevices/vtkPlusVirtualCapture.h
  VirtualDevices/vtkPlusVirtualVolumeReconstructor.h
  VirtualDevices/vtkPlusVirtualDeinterlacer.h
  )
IF(PLUS_USE_TextRecognizer)
  LIST(APPEND Virtual_HDRS VirtualDevices/vtkPlusVirtualTextRecognizer.h)
ENDIF()

LIST(APPEND ${PROJECT_NAME}_HDRS
  ${Common_HDRS}
  ${Virtual_HDRS}
  ${Miscellaneous_HDRS}
  )

SET(${PROJECT_NAME}_INSTALL_INCLUDE_FILES)
SET(External_Libraries_Install)
SET(External_Libraries_Debug)

SET(${PROJECT_NAME}_INCLUDE_DIRS
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
IF(PLUS_USE_OPTITRACK)
  SET(OptiTrack_SRCS
    OptiTrack/vtkPlusOptiTrack.cxx
    )

  STRING(REPLACE "." ";" VERSION_LIST ${OPTITRACK_MOTIVE_VERSION})
  LIST(GET VERSION_LIST 0 MOTIVE_VERSION_MAJOR)
  LIST(GET VERSION_LIST 1 MOTIVE_VERSION_MINOR)
  LIST(GET VERSION_LIST 2 MOTIVE_VERSION_PATCH)
  SET(MOTIVE_VERSION_MAJOR ${MOTIVE_VERSION_MAJOR} CACHE INTERNAL "" FORCE)
  SET(MOTIVE_VERSION_MINOR ${MOTIVE_VERSION_MINOR} CACHE INTERNAL "" FORCE)
  SET(MOTIVE_VERSION_PATCH ${MOTIVE_VERSION_PATCH} CACHE INTERNAL "" FORCE)

  SET(OptiTrack_HDRS
    OptiTrack/vtkPlusOptiTrack.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${OptiTrack_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${OptiTrack_SRCS}
    )

  # Collect all the MotiveAPI libraries
  if (OPTITRACK_MOTIVE_VERSION VERSION_LESS "3.0.0")
    set(PLATFORM_NAME_SUFFIX "")
    set(LIBRARIES_PATH_SUFFIX "lib32/")
    if (CMAKE_HOST_WIN32 AND CMAKE_CL_64 )
      set(PLATFORM_NAME_SUFFIX "x64")
      set(LIBRARIES_PATH_SUFFIX "")
    endif ()
  endif ()

  if (OPTITRACK_MOTIVE_VERSION VERSION_LESS "3.0.0")
    LIST(APPEND OPTITRACK_MOTIVE_API_STATIC_LIBS
      ${OPTITRACK_MOTIVE_DIR}/lib/NPTrackingTools${PLATFORM_NAME_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
    LIST(APPEND OPTITRACK_MOTIVE_API_SHARED_LIBS
      ${OPTITRACK_MOTIVE_DIR}/lib/NPTrackingTools${PLATFORM_NAME_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  else()
    LIST(APPEND OPTITRACK_MOTIVE_API_STATIC_LIBS
      ${OPTITRACK_MOTIVE_DIR}/lib/MotiveAPI${PLATFORM_NAME_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
    LIST(APPEND OPTITRACK_MOTIVE_API_SHARED_LIBS
      ${OPTITRACK_MOTIVE_DIR}/lib/MotiveAPI${PLATFORM_NAME_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${OPTITRACK_MOTIVE_DIR}/opencv_world454${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  endif()

  LIST(APPEND OPTITRACK_MOTIVE_API_SHARED_LIBS
    ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}libiomp5md${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

  IF (OPTITRACK_MOTIVE_VERSION VERSION_GREATER_EQUAL "2.0.0")

    IF(OPTITRACK_MOTIVE_VERSION VERSION_LESS "2.1.1")
      LIST(APPEND OPTITRACK_MOTIVE_API_SHARED_LIBS
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avcodec-56${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avformat-56${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avutil-54${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}swscale-3${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ELSE()
      LIST(APPEND OPTITRACK_MOTIVE_API_SHARED_LIBS
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avcodec-57${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avfilter-6${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avformat-57${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avresample-3${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}avutil-55${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}python36${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}shiboken2.cp36-win_amd64${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}swscale-4${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${OPTITRACK_MOTIVE_DIR}/${LIBRARIES_PATH_SUFFIX}tbb${CMAKE_SHARED_LIBRARY_SUFFIX}
        )

      IF(OPTITRACK_MOTIVE_VERSION VERSION_LESS "3.0.0")
        LIST(APPEND OPTITRACK_MOTIVE_API_PLATFORM_SHARED_LIBS
          ${OPTITRACK_MOTIVE_DIR}/plugins/platforms/qwindows${CMAKE_SHARED_LIBRARY_SUFFIX}
          )
      ENDIF()

      FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Multimedia)
      SET(PLUSLIB_QT_COMPONENTS ${PLUSLIB_QT_COMPONENTS} Multimedia CACHE INTERNAL "" FORCE)
        LIST(APPEND ${PROJECT_NAME}_LIBS
        Qt5::Multimedia
        )
    ENDIF()
  ELSE()
    IF (OPTITRACK_MSVC80_OPENMP_DIR)
    LIST(APPEND External_Libraries_Install
      ${OPTITRACK_MSVC80_OPENMP_DIR}/Microsoft.VC80.OpenMP.manifest
      ${OPTITRACK_MSVC80_OPENMP_DIR}/vcomp.dll
      )
    ENDIF()
  ENDIF()

  mark_as_advanced(MOTIVE_API_STATIC_LIBS)
  mark_as_advanced(MOTIVE_API_SHARED_LIBS)

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/OptiTrack
    ${OPTITRACK_MOTIVE_INCLUDE_DIR}
    ${OPTITRACK_NATNET_INCLUDE_DIR}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${OPTITRACK_MOTIVE_API_STATIC_LIBS}
    ${OPTITRACK_NATNET_LIBRARY_DIR}/NatNetLib${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

  LIST(APPEND External_Libraries_Install
    ${OPTITRACK_MOTIVE_API_SHARED_LIBS}
    ${OPTITRACK_NATNET_BINARY_DIR}/NatNetLib${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

ENDIF()

# --------------------------------------------------------------------------
# Optimet ConoProbe support
IF(PLUS_USE_OPTIMET_CONOPROBE)
  SET(Optimet_SRCS
    Optimet/vtkPlusOptimetConoProbeMeasurer.cxx
    )

  SET(Optimet_HDRS
    Optimet/vtkPlusOptimetConoProbeMeasurer.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Optimet_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Optimet_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${OPTIMETSMART32SDK_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Optimet
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
  ${OPTIMETSMART32SDK_LIBRARY}
  )

  LIST(APPEND External_Libraries_Install
    ${OPTIMETSMART32SDK_BINARY_DIR}/Smart32${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${OPTIMETSMART32SDK_BINARY_DIR}/NewProbeDialog${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# NDI Certus support
IF(PLUS_USE_NDI_CERTUS)
  FIND_PACKAGE(ndicapi REQUIRED NO_MODULE)

  SET(NDICertus_SRCS
    NDICertusTracking/vtkPlusNDICertusTracker.cxx
    )

  SET(NDICertus_HDRS
    NDICertusTracking/vtkPlusNDICertusTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${NDICertus_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${NDICertus_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/NDICertusTracking
    ${NDIOAPI_INCLUDE_DIR}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${NDIOAPI_LIBRARY}
    ndicapi
    )

  GET_TARGET_PROPERTY(_lib_release ndicapi IMPORTED_LOCATION_RELEASE)
  GET_TARGET_PROPERTY(_lib_debug ndicapi IMPORTED_LOCATION_DEBUG)
  IF(EXISTS ${_lib_release})
    LIST(APPEND External_Libraries_Install
      ${_lib_release}
      )
  ELSEIF(EXISTS ${_lib_debug})
    LIST(APPEND External_Libraries_Install
      ${_lib_debug}
      )
  ELSE()
    MESSAGE(FATAL_ERROR "Wanted to add ndicapi library to install, but dependent files don't exist. Do you need to build it?")
  ENDIF()
  LIST(APPEND External_Libraries_Install
    ${NDIOAPI_BINARY_DIR}/oapi${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

#--------------------------------------------------------------------------
# NDI COMMON API support
IF(PLUS_USE_NDI)
  FIND_PACKAGE(ndicapi REQUIRED NO_MODULE)

  SET(NDICAPI_SRCS
    NDICAPITracking/vtkPlusNDITracker.cxx
    )

  SET(NDICAPI_HDRS
    NDICAPITracking/vtkPlusNDITracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${NDICAPI_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${NDICAPI_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/NDICAPITracking
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ndicapi
    )

  SET(_entries)
  GET_TARGET_PROPERTY(_configs ndicapi IMPORTED_CONFIGURATIONS)
  FOREACH(_config IN LISTS _configs)
    GET_TARGET_PROPERTY(_location ndicapi IMPORTED_LOCATION_${_config})
    IF(${_config} STREQUAL RELEASE OR ${_config} STREQUAL NOCONFIG AND EXISTS ${_location})
      LIST(APPEND _entries ${_config})
      LIST(APPEND External_Libraries_Install ${_location})
    ELSEIF(${_config} STREQUAL DEBUG AND EXISTS ${_location})
      LIST(APPEND _entries ${_config})
      LIST(APPEND External_Libraries_Debug ${_location})
    ELSEIF(${_config} STREQUAL RELWITHDEBINFO AND EXISTS ${_location})
      LIST(APPEND _entries ${_config})
      LIST(APPEND External_Libraries_Debug ${_location})
    ENDIF()
  ENDFOREACH()

  LIST(LENGTH _entries _size)
  IF(_size EQUAL 0)
    MESSAGE(FATAL_ERROR "Wanted to add ndicapi library to install, but dependent files don't exist. Do you need to build it?")
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# MicronTracker support
IF(PLUS_USE_MICRONTRACKER)
  SET(MicronTracking_SRCS
    MicronTracking/vtkPlusMicronTracker.cxx
  )
  SET(MicronTracking_HDRS
    MicronTracking/vtkPlusMicronTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${MicronTracking_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${MicronTracking_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/MicronTracking
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    MicronTrackerInterface
    )

  IF(EXISTS "${MicronTracker_BINARY_DIR}/FlyCapture2${CMAKE_SHARED_LIBRARY_SUFFIX}")
    # MicronTracker 3.7.x
    LIST(APPEND External_Libraries_Install
      ${MicronTracker_BINARY_DIR}/FlyCapture2${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MicronTracker_BINARY_DIR}/FlyCapture2_C${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MicronTracker_BINARY_DIR}/libiomp5md${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MicronTracker_BINARY_DIR}/MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MicronTracker_BINARY_DIR}/PGRFlyCapture${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MicronTracker_BINARY_DIR}/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    IF(EXISTS ${MicronTracker_BINARY_DIR}/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX})
      LIST(APPEND External_Libraries_Install
        ${MicronTracker_BINARY_DIR}/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
    IF(EXISTS ${MicronTracker_BINARY_DIR}/triclops${CMAKE_SHARED_LIBRARY_SUFFIX})
      LIST(APPEND External_Libraries_Install
        ${MicronTracker_BINARY_DIR}/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
  ELSE()
    # MicronTracker 3.6.x
    LIST(APPEND External_Libraries_Install
      ${MicronTracker_BINARY_DIR}/MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${MicronTracker_BINARY_DIR}/Windist/PGRFlyCapture${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    IF(EXISTS ${MicronTracker_BINARY_DIR}/Windist/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX})
      LIST(APPEND External_Libraries_Install
        ${MicronTracker_BINARY_DIR}/Windist/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
    IF(EXISTS ${MicronTracker_BINARY_DIR}/Windist/triclops${CMAKE_SHARED_LIBRARY_SUFFIX})
      LIST(APPEND External_Libraries_Install
        ${MicronTracker_BINARY_DIR}/Windist/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# OpenHaptics support
IF(PLUS_USE_OPENHAPTICS)
  FIND_PACKAGE(OpenHaptics REQUIRED)

  SET(OpenHaptics_SRCS
    OpenHaptics/vtkPlusOpenHapticsDevice.cxx
  )

  SET(OpenHaptics_HDRS
    OpenHaptics/vtkPlusOpenHapticsDevice.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${OpenHaptics_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${OpenHaptics_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${OpenHaptics_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenHaptics
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    optimized ${HDAPI_LIBRARY_RELEASE}
    optimized ${HLAPI_LIBRARY_RELEASE}
    optimized ${HDAPI_HDU_LIBRARY_RELEASE}
    optimized ${HLAPI_HLU_LIBRARY_RELEASE}
    debug ${HDAPI_LIBRARY_DEBUG}
    debug ${HLAPI_LIBRARY_DEBUG}
    debug ${HDAPI_HDU_LIBRARY_DEBUG}
    debug ${HLAPI_HLU_LIBRARY_DEBUG}
    )

  LIST(APPEND External_Libraries_Debug
    ${OpenHaptics_BINARY_DIR}/Debug/hd${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${OpenHaptics_BINARY_DIR}/Debug/hl${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${OpenHaptics_UTILITIES_BINARY_DIR}/Debug/glut32${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

  LIST(APPEND External_Libraries_Install
    ${OpenHaptics_BINARY_DIR}/Release/hd${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${OpenHaptics_BINARY_DIR}/Release/hl${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${OpenHaptics_UTILITIES_BINARY_DIR}/Release/glut32${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

#--------------------------------------------------------------------------
# BlackMagic DeckLink support
IF(PLUS_USE_BLACKMAGIC_DECKLINK)
  FIND_PACKAGE(DeckLinkSDK REQUIRED)
  SET(BlackMagicDeckLink_HDRS
    BlackMagic/vtkPlusDeckLinkVideoSource.h
    BlackMagic/DeckLinkAPIWrapper.h
    BlackMagic/PlusOutputVideoFrame.h
    ${DeckLinkSDK_INCLUDE_DIR}/DeckLinkAPIVersion.h
  )

  SET(BlackMagicDeckLink_SRCS
    BlackMagic/vtkPlusDeckLinkVideoSource.cxx
    BlackMagic/DeckLinkAPIWrapper.cxx
    BlackMagic/PlusOutputVideoFrame.cxx
    )

  IF(UNIX)
    LIST(APPEND BlackMagicDeckLink_SRCS
      ${DeckLinkSDK_INCLUDE_DIR}/DeckLinkAPIDispatch.cpp
      )
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${BlackMagicDeckLink_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${BlackMagicDeckLink_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    DeckLinkSDK
    )
ENDIF()

#--------------------------------------------------------------------------
# INTELREALSENSE support
IF(PLUS_USE_INTELREALSENSE)
  SET(IntelRealSense_SRCS
    IntelRealSense/vtkPlusIntelRealSense.cxx
  )

  SET(IntelRealSense_HDRS
    IntelRealSense/vtkPlusIntelRealSense.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${IntelRealSense_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${IntelRealSense_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/IntelRealSense
    ${RSSDK_INCLUDE_DIR}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${RSSDK_LIB}
    )

  LIST(APPEND External_Libraries_Install
    ${RSSDK_BIN}
    )

  LIST(APPEND External_Libraries_Debug
    ${RSSDK_BIN}
    )
ENDIF()

# --------------------------------------------------------------------------
# Optical Marker Tracking

# --------------------------------------------------------------------------
# Try to find OpenCV and include its settings (otherwise complain)
IF(PLUS_USE_OpenCV)
  FIND_PACKAGE(OpenCV REQUIRED PATHS ${OpenCV_DIR})
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${OpenCV_INCLUDE_DIRS}
    )
  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${OpenCV_LIBRARIES}
    )
  # Installation of shared libraries is managed in PlusApp
ENDIF()

IF(PLUS_USE_OPTICAL_MARKER_TRACKER)
  IF(NOT PLUS_USE_OpenCV)
    MESSAGE(ERROR "PLUS_USE_OPTICAL_MARKER_TRACKER requires PLUS_USE_OpenCV option enabled")
  ENDIF()
  IF(NOT PLUS_USE_aruco OR NOT aruco_DIR)
    MESSAGE(FATAL_ERROR "PLUS_USE_OPTICAL_MARKER_TRACKER requires PLUS_USE_aruco option enabled")
  ENDIF()
  FIND_PACKAGE(aruco REQUIRED NO_MODULE)

  SET(OpticalMarkerTracking_SRCS
    OpticalMarkerTracking/vtkPlusOpticalMarkerTracker.cxx
    )

  SET(OpticalMarkerTracking_HDRS
    OpticalMarkerTracking/vtkPlusOpticalMarkerTracker.h
    )

  LIST(APPEND PlusDataCollection_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/OpticalMarkerTracking
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${OpticalMarkerTracking_HDRS}
    )

  LIST (APPEND ${PROJECT_NAME}_SRCS
    ${OpticalMarkerTracking_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    aruco
    )

  SET(_entries)
  GET_TARGET_PROPERTY(_configs aruco IMPORTED_CONFIGURATIONS)
  FOREACH(_config IN LISTS _configs)
    GET_TARGET_PROPERTY(_location aruco IMPORTED_LOCATION_${_config})
    IF(${_config} STREQUAL RELEASE OR ${_config} STREQUAL NOCONFIG AND EXISTS ${_location})
      LIST(APPEND _entries ${_config})
      LIST(APPEND External_Libraries_Install ${_location})
    ELSEIF(${_config} STREQUAL DEBUG AND EXISTS ${_location})
      LIST(APPEND _entries ${_config})
      LIST(APPEND External_Libraries_Debug ${_location})
    ENDIF()
  ENDFOREACH()

  LIST(LENGTH _entries _size)
  IF(_size EQUAL 0)
    MESSAGE(FATAL_ERROR "Wanted to add aruco library to install, but dependent files don't exist. Do you need to build it?")
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# Atracsys support
IF(PLUS_USE_ATRACSYS)
  SET(Atracsys_SRCS
    Atracsys/vtkPlusAtracsysTracker.cxx
    Atracsys/AtracsysTracker.cxx
  )

  SET(Atracsys_HDRS
    Atracsys/vtkPlusAtracsysTracker.h
    Atracsys/AtracsysTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Atracsys_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Atracsys_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Atracsys
    ${AtracsysSDK_INCLUDE_DIR}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${AtracsysSDK_LIBRARY}
    )

  IF(WIN32)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
      SET(ATRACSYS_BITNESS_SUFFIX "64")
    ELSE()
      SET(ATRACSYS_BITNESS_SUFFIX "32")
    ENDIF()

    LIST(APPEND External_Libraries_Install
      ${AtracsysSDK_BINARY_DIR}/fusionTrack${ATRACSYS_BITNESS_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${AtracsysSDK_BINARY_DIR}/device${ATRACSYS_BITNESS_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${AtracsysSDK_BINARY_DIR}/libfusionTrack64${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${AtracsysSDK_BINARY_DIR}/libdevice64${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    LIST(APPEND External_Libraries_Install
      ${AtracsysSDK_BINARY_DIR}/libfusionTrack64${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${AtracsysSDK_BINARY_DIR}/libdevice64${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
  IF(ATRACSYS_DEVICE_TYPE STREQUAL "stk")
    LIST(APPEND External_Libraries_Install
      ${AtracsysSDK_BINARY_DIR}/libusb-1.0${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    IF(UNIX)
      LIST(APPEND ${PROJECT_NAME}_LIBS
        ${AtracsysSDK_BINARY_DIR}/libusb-1.0${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
  ENDIF()

ENDIF()

#--------------------------------------------------------------------------
# PicoScope 2xxx series oscilloscope support
IF(PLUS_USE_PICOSCOPE)
  SET(PicoScope_SRCS
    PicoScope/vtkPlusPicoScopeDataSource.cxx
  )

  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET(PicoScope_HDRS
      PicoScope/vtkPlusPicoScopeDataSource.cxx
      )
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_SRCS
  ${PicoScope_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${PicoScope_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/PicoScope
    ${PicoScopeSDK_INCLUDE_DIR}
    )

  IF(WIN32)
    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${PicoScopeSDK_LIBRARY_DIR}/ps2000${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
  ELSE()
    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${PicoScopeSDK_LIBRARY_DIR}/ps2000${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()

  LIST(APPEND External_Libraries_Install
    ${PicoScopeSDK_BINARY_DIR}/ps2000${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

ENDIF()

#--------------------------------------------------------------------------
# BrachyTracker support
IF(PLUS_USE_BRACHY_TRACKER)
  SET(BrachyTracker_SRCS
    BrachyTracking/vtkPlusBrachyTracker.cxx
    BrachyTracking/PlusCmsBrachyStepper.cxx
    BrachyTracking/PlusCivcoBrachyStepper.cxx
  )

  SET(BrachyTracker_HDRS
    BrachyTracking/vtkPlusBrachyTracker.h
    BrachyTracking/PlusCmsBrachyStepper.h
    BrachyTracking/PlusBrachyStepper.h
    BrachyTracking/PlusCivcoBrachyStepper.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${BrachyTracker_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${BrachyTracker_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/BrachyTracking
    ${USDIGITAL_SEI_INCLUDE_DIR}
    )

  ADD_LIBRARY(USDigitalEncoders SHARED IMPORTED)
  SET_PROPERTY(TARGET USDigitalEncoders PROPERTY IMPORTED_IMPLIB ${USDIGITAL_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET USDigitalEncoders PROPERTY IMPORTED_LOCATION ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX})

  SET( ${PROJECT_NAME}_LIBS ${${PROJECT_NAME}_LIBS} USDigitalEncoders )
  LIST(APPEND External_Libraries_Install
    ${USDIGITAL_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${USDIGITAL_SEI_BINARY_DIR}/SEISrv32.exe
    )
  LIST(APPEND External_Libraries_Static_Install
    ${USDIGITAL_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# CLARIUS Support
IF(PLUS_USE_CLARIUS)
  FIND_PACKAGE(Threads REQUIRED)
  FIND_PACKAGE(CLARIUS REQUIRED)

  SET(CLARIUS_SRCS
    Clarius/vtkPlusClarius.cxx
    )
  SET(CLARIUS_HDRS
    Clarius/vtkPlusClarius.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${CLARIUS_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${CLARIUS_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Clarius
    ${CLARIUS_INCLUDE_DIR}
    )

  LIST(APPEND External_Libraries_Debug
    ${CLARIUS_LIB_DIR}/cast${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CLARIUS_LIB_DIR}/cast${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

  LIST(APPEND External_Libraries_Install
    ${CLARIUS_LIB_DIR}/cast${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CLARIUS_LIB_DIR}/cast${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${CLARIUS_LIB_DIR}/cast${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# CLARIUS OEM SDK Support
IF(PLUS_USE_CLARIUS_OEM)

  FIND_PACKAGE(ClariusOEM REQUIRED)

  SET(CLARIUS_OEM_SRCS
    Clarius/ClariusBLE.cxx
    Clarius/ClariusWifi.cxx
    Clarius/vtkPlusClariusOEM.cxx
    )
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    SET(CLARIUS_OEM_HDRS
      Clarius/ClariusBLE.h
      Clarius/ClariusWifi.h
      Clarius/vtkPlusClariusOEM.h
      )
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${CLARIUS_OEM_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${CLARIUS_OEM_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Clarius
    )

  LIST(APPEND ${PROJECT_NAME}_PRIVATE_LIBS
    ClariusOEM
    )

  LIST(APPEND External_Libraries_Install
    ${ClariusOEM_BINARY_PATH}
  )

ENDIF()

#--------------------------------------------------------------------------
# USDigitalEncoders tracking support
IF(PLUS_USE_USDIGITALENCODERS_TRACKER)
  SET(USDigitalEncoders_SRCS
    USDigitalEncodersTracking/vtkPlusUSDigitalEncodersTracker.cxx
  )

  SET(USDigitalEncoders_HDRS
    USDigitalEncodersTracking/vtkPlusUSDigitalEncodersTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${USDigitalEncoders_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${USDigitalEncoders_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/USDigitalEncodersTracking
    )

  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(Bitness "x64")
  ELSE()
    SET(Bitness "x86")
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_LIBS
    vtkSEIDrv
    )
  SET(vtkSEIDrv_vcProj vtkSEIDrv)

  LIST(APPEND External_Libraries_Install
    ${CMAKE_CURRENT_SOURCE_DIR}/../SEIDrv/${Bitness}/USDigital${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}/../SEIDrv/${Bitness}/USDSEI${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

#--------------------------------------------------------------------------
# Ascension 3DG support (trakSTAR)
IF( PLUS_USE_Ascension3DG )
  SET(Ascension_SRCS
    Ascension3DGTracking/vtkPlusAscension3DGTracker.cxx
    )

  SET(Ascension_HDRS
    Ascension3DGTracking/vtkPlusAscension3DGTracker.h
    )
  # vtkPlusAscension3DGTrackerBase has to be installed as well
  LIST(APPEND ${PROJECT_NAME}_INSTALL_INCLUDE_FILES
    Ascension3DGTracking/vtkPlusAscension3DGTrackerBase.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Ascension_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Ascension_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${ATC_TRAKSTAR_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    )

  ADD_LIBRARY(Ascension3DG SHARED IMPORTED)
  IF( (MSVC AND ${CMAKE_VS_PLATFORM_NAME} MATCHES "x64") OR MINGW )
    SET(Ascension3DG_LIB_FILE ${ATC_TRAKSTAR_LIB_DIR}/ATC3DG64${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "Lib file for Ascension3DG.")
    SET(Ascension3DG_SHARED_LIB_FILE ${ATC_TRAKSTAR_BINARY_DIR}/ATC3DG64${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "DLL file for Ascension3DG.")
  ELSE()
    SET(Ascension3DG_LIB_FILE ${ATC_TRAKSTAR_LIB_DIR}/ATC3DG${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "Lib file for Ascension3DG.")
    SET(Ascension3DG_SHARED_LIB_FILE ${ATC_TRAKSTAR_BINARY_DIR}/ATC3DG${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "DLL file for Ascension3DG.")
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_LIBS Ascension3DG)
  LIST(APPEND External_Libraries_Install ${Ascension3DG_SHARED_LIB_FILE})
  LIST(APPEND External_Libraries_Static_Install
    ${Ascension3DG_LIB_FILE}
    )
  SET_PROPERTY(TARGET Ascension3DG PROPERTY IMPORTED_IMPLIB ${Ascension3DG_LIB_FILE})
  SET_PROPERTY(TARGET Ascension3DG PROPERTY IMPORTED_LOCATION ${Ascension3DG_SHARED_LIB_FILE})
ENDIF()

#--------------------------------------------------------------------------
# Ascension 3DGm support (medSAFE)
IF( PLUS_USE_Ascension3DGm )
  SET(Ascension3DGm_SRCS
    Ascension3DGTracking/vtkAscension3DGmTracker.cxx
    )
  SET(Ascension3DGm_HDRS
    Ascension3DGTracking/vtkAscension3DGmTracker.h
    )
  # vtkPlusAscension3DGTrackerBase has to be installed as well
  LIST(APPEND ${PROJECT_NAME}_INSTALL_INCLUDE_FILES
    Ascension3DGTracking/vtkPlusAscension3DGTrackerBase.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Ascension3DGm_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Ascension3DGm_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${ATC_MEDSAFE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Ascension3DGTracking
    )

  ADD_LIBRARY(Ascension3DGm SHARED IMPORTED)
  SET_PROPERTY(TARGET Ascension3DGm PROPERTY IMPORTED_IMPLIB ${ATC_MEDSAFE_LIB_DIR}/ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET Ascension3DGm PROPERTY IMPORTED_LOCATION ${ATC_MEDSAFE_BINARY_DIR}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX})

  SET( ${PROJECT_NAME}_LIBS ${${PROJECT_NAME}_LIBS} Ascension3DGm)
  LIST(APPEND External_Libraries_Install
    ${ATC_MEDSAFE_BINARY_DIR}/ATC3DGm${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Static_Install
    ${ATC_MEDSAFE_LIB_DIR}/ATC3DGm${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
ENDIF()

#--------------------------------------------------------------------------
# PhidgetSpatial 3/3/3 MARG sensor (magnetometer, accelerometer, gyroscope)
IF( PLUS_USE_PHIDGET_SPATIAL_TRACKER )
  SET(PhidgetSpatial_SRCS
    PhidgetSpatialTracking/vtkPlusPhidgetSpatialTracker.cxx
    )
  SET(PhidgetSpatial_HDRS
    PhidgetSpatialTracking/vtkPlusPhidgetSpatialTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${PhidgetSpatial_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${PhidgetSpatial_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${PHIDGET_SPATIAL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/PhidgetSpatialTracking
    )

  ADD_LIBRARY(phidget SHARED IMPORTED)
  SET_PROPERTY(TARGET phidget PROPERTY IMPORTED_IMPLIB ${PHIDGET_SPATIAL_LIB_DIR}/phidget22${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET phidget PROPERTY IMPORTED_LOCATION ${PHIDGET_SPATIAL_BINARY_DIR}/phidget22${CMAKE_SHARED_LIBRARY_SUFFIX})

  LIST(APPEND ${PROJECT_NAME}_LIBS
    phidget
    )
  LIST(APPEND External_Libraries_Install
    ${PHIDGET_SPATIAL_BINARY_DIR}/phidget22${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Static_Install
    ${PHIDGET_SPATIAL_BINARY_DIR}/phidget22${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

#--------------------------------------------------------------------------
# vkxio algortihms required (magnetometer, accelerometer, gyroscope)
IF( PLUS_USE_PHIDGET_SPATIAL_TRACKER OR PLUS_USE_CLARIUS OR PLUS_USE_CLARIUS_OEM)
  LIST(APPEND ${PROJECT_NAME}_LIBS
    vtkxio
    )
ENDIF()

#--------------------------------------------------------------------------
# 3dConnexion SpaceNavigator and similar 3D mouse devices
IF( PLUS_USE_3dConnexion_TRACKER )
  IF(WIN32)  # the 3D mouse is currently supported on Windows only
    SET(3dConnexion_SRCS
      3dConnexionTracking/vtkPlus3dConnexionTracker.cxx
      )
    SET(3dConnexion_HDRS
      3dConnexionTracking/vtkPlus3dConnexionTracker.h
      )

    LIST(APPEND ${PROJECT_NAME}_HDRS
      ${3dConnexion_HDRS}
      )

    LIST(APPEND ${PROJECT_NAME}_SRCS
      ${3dConnexion_SRCS}
      )

    LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/3dConnexionTracking
      )
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# CH Robotics UM6 MARG sensor (magnetometer, accelerometer, gyroscope)
SET(ChRobotics_SRCS
  ChRobotics/vtkPlusChRoboticsTracker.cxx
  )
SET(ChRobotics_HDRS
  ChRobotics/vtkPlusChRoboticsTracker.h
  )

LIST(APPEND ${PROJECT_NAME}_HDRS
  ${ChRobotics_HDRS}
  )

LIST(APPEND ${PROJECT_NAME}_SRCS
  ${ChRobotics_SRCS}
  )

LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/ChRobotics
  )

#--------------------------------------------------------------------------
# Microchip MARG sensor (magnetometer, accelerometer, gyroscope)
SET(MicrochipMARG_SRCS
  MicrochipTracking/vtkPlusMicrochipTracker.cxx
  )
SET(MicrochipMARG_HDRS
  MicrochipTracking/vtkPlusMicrochipTracker.h
  )

LIST(APPEND ${PROJECT_NAME}_HDRS
  ${MicrochipMARG_HDRS}
  )

LIST(APPEND ${PROJECT_NAME}_SRCS
  ${MicrochipMARG_SRCS}
  )

LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/MicrochipTracking
  )

#--------------------------------------------------------------------------
# OpenIGTLink
IF( PLUS_USE_OpenIGTLink )
  SET(OpenIGTLink_SRCS
    OpenIGTLink/vtkPlusOpenIGTLinkDevice.cxx
    OpenIGTLink/vtkPlusOpenIGTLinkTracker.cxx
    OpenIGTLink/vtkPlusOpenIGTLinkVideoSource.cxx
    )

  SET(OpenIGTLink_HDRS
    OpenIGTLink/vtkPlusOpenIGTLinkDevice.h
    OpenIGTLink/vtkPlusOpenIGTLinkTracker.h
    OpenIGTLink/vtkPlusOpenIGTLinkVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${OpenIGTLink_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${OpenIGTLink_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenIGTLink
    ${OpenIGTLink_INCLUDE_DIRS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS OpenIGTLink vtkPlusOpenIGTLink )
ENDIF()

# --------------------------------------------------------------------------
# Ultrasonix support
IF(PLUS_USE_ULTRASONIX_VIDEO)
  SET(Ultrasonix_SRCS
    SonixVideo/vtkPlusSonixVideoSource.cxx
    SonixVideo/vtkPlusSonixVolumeReader.cxx
    SonixVideo/itkUlteriusImageIO.cxx
    SonixVideo/itkUlteriusImageIOFactory.cxx
    SonixVideo/vtkPlusSonixPortaVideoSource.cxx
    )

  SET(Ultrasonix_HDRS
    SonixVideo/vtkPlusSonixVideoSource.h
    SonixVideo/vtkPlusSonixVolumeReader.h
    SonixVideo/itkUlteriusImageIO.h
    SonixVideo/itkUlteriusImageIOFactory.h
    SonixVideo/vtkPlusSonixPortaVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Ultrasonix_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Ultrasonix_SRCS}
    )

  MESSAGE(STATUS "Ultrasonix SDK directory: ${ULTRASONIX_SDK_DIR}")
  IF(PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 1)
    # Use Ultrasonix SDK 1.2 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
    SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)
    LIST(APPEND ${PROJECT_NAME}_LIBS
      optimized ${ULTERIUS_LIB_DIR}/r/ulterius.lib
      debug ${ULTERIUS_LIB_DIR}/d/ulterius.lib
      )
    LIST(APPEND External_Libraries_Install
      ${ULTRASONIX_SDK_BINARY_DIR}/r/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/r/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
   LIST(APPEND External_Libraries_Debug
      ${ULTRASONIX_SDK_BINARY_DIR}/d/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/d/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSEIF(PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 2)
    # Use Ultrasonix SDK 2.0 (only Ulterius is supported)
    SET(ULTRASONIX_SDK_BINARY_DIR ${ULTRASONIX_SDK_DIR}/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
    SET(ULTERIUS_INCLUDE_DIR ${ULTRASONIX_SDK_DIR}/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
    SET(ULTERIUS_LIB_DIR ${ULTRASONIX_SDK_DIR}/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)
    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${ULTERIUS_LIB_DIR}/ulterius.lib
      )
    SET(Ultrasonix_External_Libs
      ${ULTRASONIX_SDK_BINARY_DIR}/ulterius${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/utx_utils${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_string_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_unit_convert${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/umc_xml${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${ULTRASONIX_SDK_BINARY_DIR}/xerces-c_2_7_u_vc6${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    LIST(APPEND External_Libraries_Install ${Ultrasonix_External_Libs})
    LIST(APPEND External_Libraries_Debug ${Ultrasonix_External_Libs})
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

    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${PORTA_LIB_DIR}/porta.lib
      )
    IF(PLUS_ULTRASONIX_SDK_MAJOR_VERSION EQUAL 6)
      IF(PLUS_ULTRASONIX_SDK_MINOR_VERSION EQUAL 0)
        MESSAGE( FATAL_ERROR "Plus does not support Ultrasonix SDK version 6.0.x")
      ENDIF()

      LIST(APPEND ${PROJECT_NAME}_LIBS ${ULTERIUS_LIB_DIR}/ulterius_old.lib)
    ELSE()
      LIST(APPEND ${PROJECT_NAME}_LIBS ${ULTERIUS_LIB_DIR}/ulterius.lib)
    ENDIF()

    FILE(GLOB ULTRASONIX_SDK_SHARED_LIBRARIES
      "${ULTRASONIX_SDK_BINARY_DIR}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}")

    LIST(APPEND External_Libraries_Install ${ULTRASONIX_SDK_SHARED_LIBRARIES})
    LIST(APPEND External_Libraries_Debug ${ULTRASONIX_SDK_SHARED_LIBRARIES})
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ITKCommon
    ITKIOImageBase
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${ULTERIUS_INCLUDE_DIR}
    ${PORTA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SonixVideo
    )
ENDIF()

# --------------------------------------------------------------------------
# BK ProFocus support

# OEM interface
IF(PLUS_USE_BKPROFOCUS_VIDEO)
  SET(BKProFocus_SRCS
    BkProFocus/vtkPlusBkProFocusOemVideoSource.cxx
    )
  SET(BKProFocus_HDRS
    BkProFocus/vtkPlusBkProFocusOemVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${BKProFocus_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${BKProFocus_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/BkProFocus
    )

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/BkProFocus/vtkPNGPrivate.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/vtkPNGPrivate.h
    )

  # CameraLink interface
  IF(PLUS_USE_BKPROFOCUS_CAMERALINK)
    LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
      ${GRABBIELIB_SOURCE_DIR}/AcquisitionLibrary
      ${GRABBIELIB_SOURCE_DIR}/BkmDataFileLib
      ${GRABBIELIB_SOURCE_DIR}/cfuff
      ${GRABBIELIB_SOURCE_DIR}/CommandAndControl
      ${GRABBIELIB_SOURCE_DIR}/LogLibrary
      ${GRABBIELIB_SOURCE_DIR}/OemParams
      ${GRABBIELIB_SOURCE_DIR}/ParametersLibrary
      )
    LIST(APPEND ${PROJECT_NAME}_SRCS
      BkProFocus/vtkPlusBkProFocusCameraLinkVideoSource.cxx
      BkProFocus/PlusBkProFocusCameraLinkReceiver.cxx
      )
    LIST(APPEND ${PROJECT_NAME}_HDRS
      BkProFocus/vtkPlusBkProFocusCameraLinkVideoSource.h
      BkProFocus/PlusBkProFocusCameraLinkReceiver.h
      )
    LIST(APPEND ${PROJECT_NAME}_LIBS
      GrabbieLib
      ${DALSASAPERA_LIB_DIR}/SapClassBasic${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# ICCapturing support
IF(PLUS_USE_ICCAPTURING_VIDEO)
  SET(ICCapturing_SRCS
    ICCapturing/vtkPlusICCapturingSource.cxx
    ICCapturing/ICCapturingListener.cxx
    )
  SET(ICCapturing_HDRS
    ICCapturing/vtkPlusICCapturingSource.h
    ICCapturing/ICCapturingListener.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${ICCapturing_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${ICCapturing_SRCS}
    )

  # Look for ICCapturing libraries
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${ICCAPTURING_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ICCapturing
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    optimized ${ICCAPTURING_TIS_UDSHL_STATIC_LIB}
    debug ${ICCAPTURING_TIS_UDSHLD_STATIC_LIB}
    )

  LIST(APPEND External_Libraries_Install
    ${ICCAPTURING_TIS_UDSHL_SHARED_LIB}
    )
  LIST(APPEND External_Libraries_Debug
    ${ICCAPTURING_TIS_UDSHLD_SHARED_LIB}
    )
ENDIF()

# --------------------------------------------------------------------------
# StealthLink2.0 suppport
IF(PLUS_USE_STEALTHLINK)
  SET(StealthLink_SRCS
    StealthLink/vtkPlusStealthLinkTracker.cxx
    )
  SET(StealthLink_HDRS
    StealthLink/vtkPlusStealthLinkTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${StealthLink_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${StealthLink_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${STEALTHLINK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/StealthLink
    )

  IF(WIN32)
    LIST(APPEND ${PROJECT_NAME}_LIBS
      optimized ${STEALTHLINK_STEALTHLINK_STATIC_LIBRARY}
      debug ${STEALTHLINK_STEALTHLINKD_STATIC_LIBRARY}
      )
    LIST(APPEND External_Libraries_Debug
      ${STEALTHLINK_STEALTHLINKD_SHARED_LIBRARY}
      )
    LIST(APPEND External_Libraries_Install
      ${STEALTHLINK_STEALTHLINK_SHARED_LIBRARY}
      )
  ELSE()
    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${STEALTHLINK_STEALTHLINK_STATIC_LIBRARY}
      )
  ENDIF()
ENDIF()

#--------------------------------------------------------------------------
# da Vinci Surgical System support
IF(PLUS_USE_IntuitiveDaVinci)
  SET(daVinci_SRCS
    IntuitiveDaVinci/vtkPlusIntuitiveDaVinciTracker.cxx
    IntuitiveDaVinci/IntuitiveDaVinci.cxx
    )

  SET(daVinci_HDRS
    IntuitiveDaVinci/vtkPlusIntuitiveDaVinciTracker.h
    IntuitiveDaVinci/IntuitiveDaVinci.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${daVinci_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${daVinci_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${IntuitiveDaVinci_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/IntuitiveDaVinci
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${IntuitiveDaVinci_LIBRARY}
    ws2_32
    iphlpapi
    )
ENDIF()

# --------------------------------------------------------------------------
# Video-for-Windows video digitizer support
IF(PLUS_USE_VFW_VIDEO)
  SET(Vfw_SRCS
    VideoForWindows/vtkPlusWin32VideoSource2.cxx
    )
  SET(Vfw_HDRS
    VideoForWindows/vtkPlusWin32VideoSource2.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Vfw_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Vfw_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ws2_32
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/VideoForWindows
    )
  LIST(APPEND ${PROJECT_NAME}_LIBS
    Vfw32.lib
    )
ENDIF()

# --------------------------------------------------------------------------
# Microsoft Media Foundation video digitizer support
IF(PLUS_USE_MMF_VIDEO)
  SET(Mmf_SRCS
    MicrosoftMediaFoundation/vtkPlusMmfVideoSource.cxx
    )
  SET(Mmf_HDRS
    MicrosoftMediaFoundation/vtkPlusMmfVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Mmf_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Mmf_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/MicrosoftMediaFoundation
    ${WINDOWS_SDK_INCLUDE_DIRS}
    )
  LIST(APPEND ${PROJECT_NAME}_LIBS
    MfVideoCaptureLibrary
    )
ENDIF()

# --------------------------------------------------------------------------
# Epiphan support
IF(PLUS_USE_EPIPHAN)
  SET(Epiphan_SRCS
    Epiphan/vtkPlusEpiphanVideoSource.cxx
    )
  SET(Epiphan_HDRS
    Epiphan/vtkPlusEpiphanVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Epiphan_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Epiphan_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${EPIPHAN_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Epiphan
    )
  LIST(APPEND External_Libraries_Install
    ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Debug
    ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Static_Install
    ${EPIPHAN_LIB_DIR}/frmgrab${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

  # Create a target for proper CMake infrastructure
  ADD_LIBRARY(Epiphan SHARED IMPORTED)
  SET_PROPERTY(TARGET Epiphan PROPERTY IMPORTED_IMPLIB ${EPIPHAN_LIB_DIR}/frmgrab${CMAKE_STATIC_LIBRARY_SUFFIX})
  SET_PROPERTY(TARGET Epiphan PROPERTY IMPORTED_LOCATION ${EPIPHAN_BINARY_DIR}/frmgrab${CMAKE_SHARED_LIBRARY_SUFFIX})

  LIST(APPEND ${PROJECT_NAME}_LIBS
    Epiphan
    )
ENDIF()

# --------------------------------------------------------------------------
# Capistrano Labs support
IF(PLUS_USE_CAPISTRANO_VIDEO)
  IF(${CAPISTRANO_SDK_VERSION} STREQUAL "cSDK2023")
    ADD_DEFINITIONS(-DCAPISTRANO_SDK2023)
  ELSEIF(${CAPISTRANO_SDK_VERSION} STREQUAL "cSDK2019.3")
    ADD_DEFINITIONS(-DCAPISTRANO_SDK2019_3)
  ELSEIF(${CAPISTRANO_SDK_VERSION} STREQUAL "cSDK2019.2")
    ADD_DEFINITIONS(-DCAPISTRANO_SDK2019_2)
  ELSEIF(${CAPISTRANO_SDK_VERSION} STREQUAL "cSDK2019")
    ADD_DEFINITIONS(-DCAPISTRANO_SDK2019)
  ELSEIF(${CAPISTRANO_SDK_VERSION} STREQUAL "cSDK2018")
    ADD_DEFINITIONS(-DCAPISTRANO_SDK2018)
  ENDIF()
  SET(Capistrano_SRCS
    Capistrano/vtkPlusCapistranoVideoSource.cxx
    )
  SET(Capistrano_HDRS
    Capistrano/vtkPlusCapistranoVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Capistrano_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Capistrano_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${CAPISTRANO_LIBRARY_DIR}/${CAPISTRANO_LIBRARY_USBPROBE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CAPISTRANO_LIBRARY_DIR}/${CAPISTRANO_LIBRARY_BMODE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CAPISTRANO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Capistrano
    )
  LIST(APPEND External_Libraries_Install
    ${CAPISTRANO_BINARY_DIR}/${CAPISTRANO_BINARY_USBPROBE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${CAPISTRANO_BINARY_DIR}/${CAPISTRANO_BINARY_BMODE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Debug
    ${CAPISTRANO_BINARY_DIR}/${CAPISTRANO_BINARY_USBPROBE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${CAPISTRANO_BINARY_DIR}/${CAPISTRANO_BINARY_BMODE_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# WinProbe support
IF(PLUS_USE_WINPROBE_VIDEO)
  SET(WinProbe_SRCS
    WinProbe/vtkPlusWinProbeVideoSource.cxx
    )
  SET(WinProbe_HDRS
    WinProbe/vtkPlusWinProbeVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${WinProbe_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${WinProbe_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${WINPROBESDK_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/WinProbe
    )

  FILE(GLOB WinProbeXMLFiles "${WINPROBESDK_DIR}/xml/*.xml")
  FILE(COPY ${WinProbeXMLFiles} DESTINATION ${CMAKE_BINARY_DIR})

  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(Bitness "x64")
  ELSE()
    SET(Bitness "x86")
  ENDIF()

  FILE(GLOB WinProbeAssemblies "${WINPROBESDK_DIR}/${Bitness}/*.dll")
  FILE(COPY ${WinProbeAssemblies} DESTINATION ${CMAKE_BINARY_DIR}) # allows running tests without changing the working directory

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${WINPROBESDK_DIR}/${Bitness}/$<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Debug>>:Release>/UltraVisionManagedDll${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/$<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Debug>>:Release>/UltraVisionManagedDXDll${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Install
    ${WINPROBESDK_DIR}/${Bitness}/Release/UltraVisionManagedDll${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/Release/UltraVisionManagedDXDll${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/Release/UltraVisionWrapper${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/Release/UltraVision.DXWrapper${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WinProbeAssemblies}
    ${WinProbeXMLFiles}
    )
  LIST(APPEND External_Libraries_Debug
    ${WINPROBESDK_DIR}/${Bitness}/Debug/UltraVisionManagedDll${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/Debug/UltraVisionManagedDXDll${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/Debug/UltraVisionWrapper${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WINPROBESDK_DIR}/${Bitness}/Debug/UltraVision.DXWrapper${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${WinProbeAssemblies}
    ${WinProbeXMLFiles}
    )
ENDIF()

# --------------------------------------------------------------------------
# Interson support
IF(PLUS_USE_INTERSON_VIDEO)
  SET(Interson_SRCS
    Interson/vtkPlusIntersonVideoSource.cxx
    )
  SET(Interson_HDRS
    Interson/vtkPlusIntersonVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Interson_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Interson_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${INTERSON_LIBRARY_DIR}/BmodeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${INTERSON_LIBRARY_DIR}/USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${INTERSON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Interson
    )

  IF(${CMAKE_GENERATOR} MATCHES Win64)
    LIST(APPEND External_Libraries_Install
      ${INTERSON_WIN64_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    LIST(APPEND External_Libraries_Debug
      ${INTERSON_WIN64_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
      LIST(APPEND External_Libraries_Install
      ${INTERSON_WIN32_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    LIST(APPEND External_Libraries_Debug
      ${INTERSON_WIN32_BINARY_DIR}/USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${INTERSON_BINARY_DIR}/BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Interson SDK C++ support
IF(PLUS_USE_INTERSONSDKCXX_VIDEO)
  FIND_PACKAGE(IntersonSDKCxx REQUIRED)
  SET(IntersonCXX_SRCS
    IntersonSDKCxx/vtkPlusIntersonSDKCxxVideoSource.cxx
    )
  SET(IntersonCXX_HDRS
    IntersonSDKCxx/vtkPlusIntersonSDKCxxVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${IntersonCXX_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${IntersonCXX_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${IntersonSDKCxx_LIBRARIES}
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${IntersonSDKCxx_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/IntersonSDKCxx
    )

  LIST(APPEND External_Libraries_Install
    ${IntersonSDK_DIR}/Libraries/Interson${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonSDK_DIR}/Libraries/IntersonTools${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Debug
    ${IntersonSDK_DIR}/Libraries/Interson${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonSDK_DIR}/Libraries/IntersonTools${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# Interson Array SDK C++ support
IF(PLUS_USE_INTERSONARRAYSDKCXX_VIDEO)
  FIND_PACKAGE(IntersonArraySDKCxx REQUIRED)
  SET(IntersonArrayCXX_SRCS
    IntersonArraySDKCxx/vtkPlusIntersonArraySDKCxxVideoSource.cxx
    )
  SET(IntersonArrayCXX_HDRS
    IntersonArraySDKCxx/vtkPlusIntersonArraySDKCxxVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${IntersonArrayCXX_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${IntersonArrayCXX_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${IntersonArraySDKCxx_LIBRARIES}
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/IntersonArraySDKCxx
    )

  LIST(APPEND External_Libraries_Install
    ${IntersonArraySDK_DIR}/IntersonArray${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonArraySDK_DIR}/IntersonArrayTools${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonArraySDK_DIR}/MathNet.Numerics${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Debug
    ${IntersonArraySDK_DIR}/IntersonArray${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonArraySDK_DIR}/IntersonArrayTools${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${IntersonArraySDK_DIR}/MathNet.Numerics${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# Point Grey Spinnaker API support

IF(PLUS_USE_SPINNAKER_VIDEO)
  SET(Spinnaker_SRCS
    PointGrey/vtkPlusSpinnakerVideoSource.cxx
    )

  #IF(MSVC)
    SET(Spinnaker_HDRS
      PointGrey/vtkPlusSpinnakerVideoSource.h
      )
  #ENDIF()

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Spinnaker_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Spinnaker_SRCS}
    )

  # Collect all the Spinnaker API libraries
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/PointGrey
    ${SPINNAKER_API_INCLUDE_DIR}
    )

  IF(WIN32)
    # Windows libs
    LIST(APPEND ${PROJECT_NAME}_LIBS
      debug     ${SPINNAKER_API_LIBRARY_DIR}/Spinnakerd_v140${CMAKE_STATIC_LIBRARY_SUFFIX}
      optimized ${SPINNAKER_API_LIBRARY_DIR}/Spinnaker_v140${CMAKE_STATIC_LIBRARY_SUFFIX}
      debug     ${SPINNAKER_API_LIBRARY_DIR}/SpinVideod_v140${CMAKE_STATIC_LIBRARY_SUFFIX}
      optimized ${SPINNAKER_API_LIBRARY_DIR}/SpinVideo_v140${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
    LIST(APPEND External_Libraries_Debug
      ${SPINNAKER_API_BINARY_DIR}/GCBase_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/GenApi_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/GenTLd_v140${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/Log_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/log4cpp_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/MathParser_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/NodeMapData_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/Spinnakerd_v140${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/XMLParser_MDd_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    LIST(APPEND External_Libraries_Install
      ${SPINNAKER_API_BINARY_DIR}/GCBase_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/GenApi_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/GenTL_v140${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/libiomp5md${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/Log_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/log4cpp.spinnaker.property
      ${SPINNAKER_API_BINARY_DIR}/log4cpp_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/MathParser_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/NodeMapData_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/Spinnaker_v140${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${SPINNAKER_API_BINARY_DIR}/XMLParser_MD_VC140_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
      )

  ELSEIF(UNIX AND NOT MAC)
    # Ubuntu libs
    LIST(APPEND ${PROJECT_NAME}_LIBS
    ${SPINNAKER_API_LIBRARY_DIR}/libGCBase_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libGenApi_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/liblog4cpp_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libLog_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libMathParser_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libNodeMapData_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libSpinnaker${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libSpinVideo${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libXmlParser_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
    LIST(APPEND External_Libraries_Install
    ${SPINNAKER_API_LIBRARY_DIR}/libGCBase_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libGenApi_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/liblog4cpp_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libLog_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libMathParser_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libNodeMapData_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libSpinnaker${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libSpinVideo${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${SPINNAKER_API_LIBRARY_DIR}/libXmlParser_gcc540_v3_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  ENDIF()

  # Platform invariant "libraries"
  LIST(APPEND External_Libraries_Install
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLDataStream_GigE_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLDataStream_Usb3_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLDevice_GigE_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLDevice_Reference_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLDevice_Usb3_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLInterface_GigE_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLInterface_Reference_Version_1_0_0_Schema_1_1.xml
    ${SPINNAKER_API_BINARY_DIR}/SFNC_GenTLInterface_Usb3_Version_1_0_0_Schema_1_1.xml
    )
ENDIF()

# --------------------------------------------------------------------------
# Telemed support
IF(PLUS_USE_TELEMED_VIDEO)
  SET(Telemed_SRCS
    Telemed/vtkPlusTelemedVideoSource.cxx
    Telemed/TelemedUltrasound.cxx
    )
  SET(Telemed_HDRS
    Telemed/vtkPlusTelemedVideoSource.h
    Telemed/TelemedUltrasound.h
    )

  IF(MSVC AND ${CMAKE_VS_PLATFORM_NAME} MATCHES "x64")
    LIST(APPEND Telemed_SRCS
      Telemed/x64/Usgfw2_i.c
      Telemed/x64/Usgfw2_p.c
      Telemed/x64/dlldata.c
      )
    LIST(APPEND Telemed_SRCS
      Telemed/x64/Usgfw2.h
      )
  ELSE()
    LIST(APPEND Telemed_SRCS
      Telemed/x86/Usgfw2_i.c
      Telemed/x86/Usgfw2_p.c
      Telemed/x86/dlldata.c
      )
    LIST(APPEND Telemed_SRCS
      Telemed/x86/Usgfw2.h
      )
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Telemed_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Telemed_SRCS}
    )

  # Find Telemed SDK include directory
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${TELEMED_INCLUDE_DIR}
    ${WINDOWS_SDK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Telemed
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    rpcrt4.lib
    )

ENDIF()

# --------------------------------------------------------------------------
# Thorlabs support
IF(PLUS_USE_THORLABS_VIDEO)
  # Linker would return with SAFESEH not supported error
  # if attempting to link Thorlabs libraries with VS2013 or later.
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
  SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
  SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
  SET(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
  SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
  SET(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
  SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
  SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")

  SET(ThorLabs_SRCS
    ThorLabs/vtkPlusThorLabsVideoSource.cxx
    )
  SET(ThorLabs_HDRS
    ThorLabs/vtkPlusThorLabsVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${ThorLabs_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${ThorLabs_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/ThorLabs
    ${THORLABS_INCLUDE_DIR}
    )
  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${THORLABS_LIBRARY_DIR}/TLCCS_32${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${THORLABS_LIBRARY_DIR}/visa32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
ENDIF()

# --------------------------------------------------------------------------
# Philips ie33 3d ultrasound support
IF(PLUS_USE_PHILIPS_3D_ULTRASOUND)
  IF(PLUS_USE_ULTRASONIX_VIDEO)
    # Ultrasonix contains its own libmmd.dll which does not support all the functionality needed for the philips probe
    # They both cannot be enabled at the same time.
    MESSAGE( FATAL_ERROR "Plus cannot enable both Ultrasonix and Philips devices due to .dll conflicts in their respective SDK packages.")
  ELSE()
    SET(Philips_SRCS
      Philips3D/vtkPlusPhilips3DProbeVideoSource.cxx
      Philips3D/vtkPlusIEEListener.cxx
      )

    SET(Philips_HDRS
      Philips3D/vtkPlusPhilips3DProbeVideoSource.h
      Philips3D/vtkPlusIEEListener.h
      )

    LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/Philips3D
      ${Philips_INCLUDE_DIRS}
      )

    LIST(APPEND ${PROJECT_NAME}_LIBS
      ${Philips_LIBRARY_DIR}/PipelineComDll${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${Philips_LIBRARY_DIR}/3DData${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${Philips_LIBRARY_DIR}/3DUtil${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    FILE(GLOB PHILIPS_SDK_SHARED_LIBRARIES "${Philips_BINARY_DIRS}/[^.]*${CMAKE_SHARED_LIBRARY_SUFFIX}")

    LIST(APPEND ${PROJECT_NAME}_HDRS
      ${Philips_HDRS}
      )

    LIST(APPEND ${PROJECT_NAME}_SRCS
      ${Philips_SRCS}
      )

    LIST(APPEND External_Libraries_Install
      ${PHILIPS_SDK_SHARED_LIBRARIES}
      )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Agilent DXYZ/D310 PCI oscillioscope support
IF(PLUS_USE_AGILENT)
  FIND_PACKAGE(AgilentMD1 REQUIRED)

  SET(Agilent_SRCS
    Agilent/vtkPlusAgilentScopeTracker.cxx
    )
  SET(Agilent_HDRS
    Agilent/vtkPlusAgilentScopeTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS ${Agilent_HDRS})
  LIST(APPEND ${PROJECT_NAME}_SRCS ${Agilent_SRCS})

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Agilent)
  GET_TARGET_PROPERTY(_md1_shared_lib AgilentMD1 IMPORTED_LOCATION)
  LIST(APPEND External_Libraries_Install
    ${_md1_shared_lib}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS AgilentMD1)
ENDIF()

# --------------------------------------------------------------------------
# Try to find tesseract
IF(PLUS_USE_TextRecognizer)
  FIND_PACKAGE(tesseract NO_MODULE REQUIRED)

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${tesseract_INCLUDE_DIRS}
    )
  LIST(APPEND ${PROJECT_NAME}_PRIVATE_LIBS
    ${tesseract_LIBRARIES}
    ${PlusZLib}
    ${PLUSLIB_VTK_PREFIX}png
    )
  FILE(GLOB _tessdata_files
    LIST_DIRECTORIES false
    ${tesseract_DATA_DIR}/*
    )
  INSTALL(FILES ${_tessdata_files}
    DESTINATION "${PLUSLIB_DATA_INSTALL}/tessdata" COMPONENT LanguageData
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
    LIST(APPEND NVidiaDVP_SRCS
      NVidiaDVP/vtkPlusNVidiaDVPVideoSourceWin32.cxx
      )

    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND NVidiaDVP_HDRS
        NVidiaDVP/vtkPlusNVidiaDVPVideoSourceWin32.h
        )
    ENDIF()
  ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND NVidiaDVP_SRCS
      NVidiaDVP/vtkPlusNVidiaDVPVideoSourceLinux.cxx
      )
  ENDIF()

  LIST(APPEND NVidiaDVP_SRCS
    NVidiaDVP/glExtensions.cpp
    NVidiaDVP/nvGPUutil.cpp
    NVidiaDVP/nvSDIin.cpp
    NVidiaDVP/nvSDIout.cpp
    NVidiaDVP/nvSDIutil.cpp
    )

  LIST(APPEND NVidiaDVP_HDRS
    NVidiaDVP/nvGPUutil.h
    NVidiaDVP/nvSDIin.h
    NVidiaDVP/nvSDIout.h
    NVidiaDVP/glExtensions.h
    NVidiaDVP/nvSDIutil.h
    NVidiaDVP/nvCommon.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${NVidiaDVP_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${NVidiaDVP_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/NVidiaDVP
    )

  LIST(APPEND External_Libraries_Install
    ${NVIDIA_DVP_BINARY_DIR}/dvp${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Debug
    ${NVIDIA_DVP_BINARY_DIR}/dvp${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

  IF(BUILD_SHARED_LIBS)
    # Create a target for proper CMake infrastructure
    ADD_LIBRARY(NVidiaDVP SHARED IMPORTED)
    SET_PROPERTY(TARGET NVidiaDVP PROPERTY IMPORTED_IMPLIB ${NVIDIA_DVP_LIB_DIR}/dvp${CMAKE_STATIC_LIBRARY_SUFFIX})
    SET_PROPERTY(TARGET NVidiaDVP PROPERTY IMPORTED_LOCATION ${NVIDIA_DVP_BINARY_DIR}/dvp${CMAKE_SHARED_LIBRARY_SUFFIX})
    SET_PROPERTY(TARGET NVidiaDVP PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${NVIDIA_DVP_INCLUDE_DIR};${OPENGL_INCLUDE_DIR};${PlusCommon_INCLUDE_DIRS}")
  ELSE()
    # What to do here?
  ENDIF()

  LIST(APPEND ${PROJECT_NAME}_LIBS
    NVidiaDVP
    QuadroSDI
    ${OPENGL_LIBRARIES}
    )
  LIST(APPEND External_Libraries_Static_Install
    "${QuadroSDI_LIBRARY_DIR}/nvapi64${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${QuadroSDI_LIBRARY_DIR}/nvapi${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${QuadroSDI_LIBRARY_DIR}/ANCapi${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${QuadroSDI_LIBRARY_DIR}/NvCpl${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${QuadroSDI_LIBRARY_DIR}/cutil64${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${QuadroSDI_LIBRARY_DIR}/cutil32${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${NVIDIA_DVP_LIB_DIR}/dvp${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
ENDIF()

# --------------------------------------------------------------------------
# OvrvisionPro
IF(PLUS_USE_OvrvisionPro)
  FIND_PACKAGE(OvrvisionPro REQUIRED NO_MODULE)

  SET(OvrvisionPro_SRCS
    OvrvisionPro/vtkPlusOvrvisionProVideoSource.cxx
    )

  SET(OvrvisionPro_HDRS
    OvrvisionPro/vtkPlusOvrvisionProVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/OvrvisionPro
    ${OpenCV_INCLUDE_DIRS}
    ${OpenCL_INCLUDE_DIRS}
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${OvrvisionPro_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${OvrvisionPro_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    OvrvisionPro
    )
ENDIF()

# --------------------------------------------------------------------------
# OpenCV_VIDEO
IF(PLUS_USE_OpenCV_VIDEO AND NOT PLUS_USE_OpenCV)
  MESSAGE("PLUS_USE_OpenCV_VIDEO requires PLUS_USE_OpenCV option enabled, enabling.")
  SET(PLUS_USE_OpenCV ON CACHE BOOL "Use OpenCV for optical marker tracking and other features." FORCE)
ENDIF()

IF(PLUS_USE_OpenCV_VIDEO)
  SET(OpenCV_Video_SRCS
    OpenCVVideo/vtkPlusOpenCVCaptureVideoSource.cxx
    )
  SET(OpenCV_Video_HDRS
    OpenCVVideo/vtkPlusOpenCVCaptureVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${OpenCV_Video_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${OpenCV_Video_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenCVVideo
    )
ENDIF()

#---------------------------------------------------------------------------
# InfraredSeekCam
IF(PLUS_USE_INFRARED_SEEK_CAM)
  FIND_PACKAGE(SeekCameraLib REQUIRED NO_MODULE)

  # Compile
  SET(Infrared_SeekCam_SRCS
      InfraredSeekCam/vtkInfraredSeekCam.cxx
    )
  SET(Infrared_SeekCam_HDRS
    InfraredSeekCam/vtkInfraredSeekCam.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Infrared_SeekCam_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Infrared_SeekCam_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/InfraredSeekCam
    ${SeekCameraLib_INCLUDE_DIRS}
  )

  get_target_property(seek_static_release seek_static IMPORTED_LOCATION_RELEASE)
  get_target_property(seek_static_debug seek_static  IMPORTED_LOCATION_DEBUG)
  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${LIBUSB_LIBRARY}
    optimized ${seek_static_release}
    debug ${seek_static_debug}
  )
ENDIF()

#---------------------------------------------------------------------------
# InfraredTEQ1Cam
IF(PLUS_USE_INFRARED_TEQ1_CAM)
  FIND_PACKAGE(TEQ1SDK REQUIRED)

  # Compile
  SET(Infrared_TEQ1_SRCS
    InfraredTEQ1Cam/vtkInfraredTEQ1Cam.cxx
    )
  SET(Infrared_TEQ1_HDRS
      InfraredTEQ1Cam/vtkInfraredTEQ1Cam.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Infrared_TEQ1_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Infrared_TEQ1_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/InfraredTEQ1Cam
    ${TEQ1_SDK_LIBRARY}
    ${TEQ1_SDK_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${TEQ1_SDK_LIBRARY}
  )
  LIST(APPEND External_Libraries_Install
    ${TEQ1_SDK_BINARY}/${SDK_FILENAME}
    ${TEQ1_SDK_BINARY}/${SDK_FILENAME_USB}
    ${TEQ1_SDK_BINARY}/${SDK_FILENAME_IMG}
  )
  LIST(APPEND External_Libraries_Debug
    ${TEQ1_SDK_BINARY}/${SDK_FILENAME}
    ${TEQ1_SDK_BINARY}/${SDK_FILENAME_USB}
    ${TEQ1_SDK_BINARY}/${SDK_FILENAME_IMG}
  )

ENDIF()

#---------------------------------------------------------------------------
# InfraredTEEV2Cam
IF(PLUS_USE_INFRARED_TEEV2_CAM)
  FIND_PACKAGE(TEEV2SDK REQUIRED)

  # Compile
  SET(Infrared_TEEV2_SRCS
    InfraredTEEV2Cam/vtkPlusInfraredTEEV2Cam.cxx
    )
  SET(Infrared_TEEV2_HDRS
      InfraredTEEV2Cam/vtkPlusInfraredTEEV2Cam.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Infrared_TEEV2_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Infrared_TEEV2_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/InfraredTEEV2Cam
    ${TEEV2_SDK_LIBRARY}
    ${TEEV2_SDK_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${TEEV2_SDK_LIBRARY}
  )
  LIST(APPEND External_Libraries_Install
    ${TEEV2_SDK_BINARY}/${SDK_FILENAME}
    ${TEEV2_SDK_BINARY}/${SDK_FILENAME_USB}
    ${TEEV2_SDK_BINARY}/${SDK_FILENAME_IMG}
  )
  LIST(APPEND External_Libraries_Debug
    ${TEEV2_SDK_BINARY}/${SDK_FILENAME}
    ${TEEV2_SDK_BINARY}/${SDK_FILENAME_USB}
    ${TEEV2_SDK_BINARY}/${SDK_FILENAME_IMG}
  )

ENDIF()

#---------------------------------------------------------------------------
# UltravioletPCOUVCam
IF(PLUS_USE_ULTRAVIOLET_PCOUV_CAM)
  FIND_PACKAGE(PCOUVSDK REQUIRED)

  # Compile
  SET(Ultraviolet_PCOUV_SRCS
    UltravioletPCOUVCam/vtkPlusUltravioletPCOUVCam.cxx
    )
  SET(Ultraviolet_PCOUV_HDRS
      UltravioletPCOUVCam/vtkPlusUltravioletPCOUVCam.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Ultraviolet_PCOUV_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Ultraviolet_PCOUV_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/UltravioletPCOUVCam
    ${PCOUV_SDK_LIBRARY}
    ${PCOUV_SDK_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${PCOUV_SDK_LIBRARY}
  )
  LIST(APPEND External_Libraries_Install
    ${PCOUV_SDK_BINARY}/${SDK_FILENAME}
  )
  LIST(APPEND External_Libraries_Debug
    ${PCOUV_SDK_BINARY}/${SDK_FILENAME}
  )

ENDIF()

#---------------------------------------------------------------------------
# DAQVIDEOSOURCE
IF(PLUS_USE_DAQVIDEOSOURCE)
  FIND_PACKAGE(DAQVIDEOSOURCESDK REQUIRED)

  # Compile
  SET(DAQVIDEOSOURCE_SRCS
    DAQVideoSource/vtkPlusDAQVideoSource.cxx
    )
  SET(DAQVIDEOSOURCE_HDRS
      DAQVideoSource/vtkPlusDAQVideoSource.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${DAQVIDEOSOURCE_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${DAQVIDEOSOURCE_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/DAQVideoSource
    ${DAQVIDEOSOURCE_SDK_LIBRARY}
    ${DAQVIDEOSOURCE_SDK_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${DAQVIDEOSOURCE_SDK_LIBRARY}
  )
  LIST(APPEND External_Libraries_Install
    ${DAQVIDEOSOURCE_SDK_BINARY}/${SDK_FILENAME}
  )
  LIST(APPEND External_Libraries_Debug
    ${DAQVIDEOSOURCE_SDK_BINARY}/${SDK_FILENAME}
  )

ENDIF()

IF(PLUS_USE_DAQMMFCOMBINED_VIDEO)
  # Compile
  SET(DAQMMFCOMBINED_VIDEO_SRCS
    DAQMMFCombinedVideo/vtkPlusDAQMMFCombinedVideo.cxx
    )
  SET(DAQMMFCOMBINED_VIDEO_HDRS
      DAQMMFCombinedVideo/vtkPlusDAQMMFCombinedVideo.h
  )
  
  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${DAQMMFCOMBINED_VIDEO_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${DAQMMFCOMBINED_VIDEO_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/DAQMMFCombinedVideo
  )
ENDIF()

#---------------------------------------------------------------------------
# FLIRSpinnakerCam
IF(PLUS_USE_FLIRSPINNAKER_CAM)
  FIND_PACKAGE(FLIRSPINNAKER_SDK REQUIRED)

  # Compile
  SET(FLIRSPINNAKER_SRCS
    FLIRSpinnakerCam/vtkPlusFLIRSpinnakerCam.cxx
    )
  SET(FLIRSPINNAKER_HDRS
      FLIRSpinnakerCam/vtkPlusFLIRSpinnakerCam.h
  )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${FLIRSPINNAKER_HDRS}
  )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${FLIRSPINNAKER_SRCS}
  )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/FLIRSpinnakerCam
    ${FLIRSPINNAKER_LIBRARY}
    ${FLIRSPINNAKER_SDK_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${FLIRSPINNAKER_SDK_LIBRARY}
  )
  LIST(APPEND External_Libraries_Install
    ${FLIRSPINNAKER_SDK_BINARY}/${SDK_FILENAME}
  )
  LIST(APPEND External_Libraries_Debug
    ${FLIRSPINNAKER_SDK_BINARY}/${SDK_FILENAME}
  )

ENDIF()





# --------------------------------------------------------------------------
# V4L2
IF(PLUS_USE_V4L2)
  FIND_PACKAGE(V4L2 REQUIRED)

  SET(V4L2_Video_SRCS
    V4L2/vtkPlusV4L2VideoSource.cxx
    )
  SET(V4L2_Video_HDRS
    V4L2/vtkPlusV4L2VideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${V4L2_Video_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${V4L2_Video_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/V4L2
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    v4l2
    )
ENDIF()

# --------------------------------------------------------------------------
# Andor Cam support
IF(PLUS_USE_ANDOR_CAMERA)
  IF(NOT PLUS_USE_OpenCV)
    MESSAGE(ERROR "PLUS_USE_ANDOR_CAMERA requires PLUS_USE_OpenCV option enabled")
  ENDIF()
  SET(Andor_SRCS
    Andor/vtkPlusAndorVideoSource.cxx
    )
  SET(Andor_HDRS
    Andor/vtkPlusAndorVideoSource.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Andor_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Andor_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    ${ANDOR_LIBRARY_DIR}/${ANDOR_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${ANDOR_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Andor
	${OpenCV_INCLUDE_DIRS}
    )
  LIST(APPEND External_Libraries_Install
    ${ANDOR_BINARY_DIR}/${ANDOR_DLL}${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  LIST(APPEND External_Libraries_Debug
    ${ANDOR_BINARY_DIR}/${ANDOR_DLL}${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()


# --------------------------------------------------------------------------
# WitMotion
IF(PLUS_USE_WITMOTIONTRACKER)
  SET(WitMotionTracker_SRCS
    WitMotionTracker/vtkPlusWitMotionTracker.cxx
    )
  SET(WitMotionTracker_HDRS
    WitMotionTracker/vtkPlusWitMotionTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${WitMotionTracker_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${WitMotionTracker_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/WitMotionTracker
    )
ENDIF()

# --------------------------------------------------------------------------
# LeapMotion
IF(PLUS_USE_LEAPMOTION)
  FIND_PACKAGE(LeapSDK 4.0)
  SET(LeapMotion_SRCS
    LeapMotion/vtkPlusLeapMotion.cxx
    )
  SET(LeapMotion_HDRS
    LeapMotion/vtkPlusLeapMotion.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${LeapMotion_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${LeapMotion_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/LeapMotion
    )

  LIST(APPEND ${PROJECT_NAME}_LIBS
    LeapSDK::LeapC
    )

  GET_TARGET_PROPERTY(_install_file LeapSDK::LeapC IMPORTED_LOCATION_RELEASE)
  IF(NOT _install_file)
    MESSAGE(FATAL_ERROR "Unable to locate LeapMotion DLL to install. Check LeapSDK installation.")
  ENDIF()
  LIST(APPEND External_Libraries_Install
    ${_install_file}
    )
ENDIF()

# --------------------------------------------------------------------------
# SteamVR support
IF(PLUS_USE_STEAMVR)
  FIND_PACKAGE(OpenVR REQUIRED)

  SET(STEAMVR_SRCS
    SteamVR/vtkPlusSteamVRTracker.cxx
    )

  SET(STEAMVR_HDRS
    SteamVR/vtkPlusSteamVRTracker.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${STEAMVR_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${STEAMVR_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/SteamVR
    ${OpenVR_INCLUDE_DIRS}
    )

  LIST(APPEND ${PROJECT_NAME}_PRIVATE_LIBS
    OpenVR
    )

  GET_TARGET_PROPERTY(_install_file OpenVR IMPORTED_LOCATION_RELEASE)
  IF(NOT _install_file)
    GET_TARGET_PROPERTY(_install_file OpenVR IMPORTED_IMPLIB)
    IF(NOT _install_file)
      MESSAGE(FATAL_ERROR "Unable to locate OpenVR library to install. Check OpenVR installation.")
    ENDIF()
  ENDIF()
  LIST(APPEND External_Libraries_Install
    ${_install_file}
    )
ENDIF()

# --------------------------------------------------------------------------
# Generic sensor
IF(PLUS_USE_GENERIC_SENSOR_TRACKER)
  IF(WIN32)
    SET(GenericSensorTracker_SRCS
      GenericSensor/vtkPlusGenericSensorTracker.cxx
      )
    SET(GenericSensorTracker_HDRS
      GenericSensor/vtkPlusGenericSensorTracker.h
      )

    LIST(APPEND ${PROJECT_NAME}_HDRS
      ${GenericSensorTracker_HDRS}
      )

    LIST(APPEND ${PROJECT_NAME}_SRCS
      ${GenericSensorTracker_SRCS}
      )

    LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/GenericSensor
    )

    LIST(APPEND ${PROJECT_NAME}_PRIVATE_LIBS
      Sensorsapi.lib
      PortableDeviceGUIDs.lib
    )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Azure Kinect
IF(PLUS_USE_AZUREKINECT)
  SET(AzureKinect_SRCS
    AzureKinect/vtkPlusAzureKinect.cxx
    )
  SET(AzureKinect_HDRS
    AzureKinect/vtkPlusAzureKinect.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${AzureKinect_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${AzureKinect_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/AzureKinect
    ${K4A_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_PRIVATE_LIBS
    ${K4A_LIBRARY}
  )

  IF(WIN32)
    LIST(APPEND External_Libraries_Install
      ${K4A_BINARY_DIR}/k4a${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${K4A_BINARY_DIR}/depthengine_2_0${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
  ENDIF()
ENDIF()

# --------------------------------------------------------------------------
# Revopoint 3D
IF(PLUS_USE_REVOPOINT3DCAMERA)
  SET(Revopoint3DCamera_SRCS
      Revopoint3DCamera/vtkPlusRevopoint3DCamera.cxx
    )
  SET(Revopoint3DCamera_HDRS
      Revopoint3DCamera/vtkPlusRevopoint3DCamera.h
    )

  LIST(APPEND ${PROJECT_NAME}_HDRS
    ${Revopoint3DCamera_HDRS}
    )

  LIST(APPEND ${PROJECT_NAME}_SRCS
    ${Revopoint3DCamera_SRCS}
    )

  LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Revopoint3DCamera
    ${REVOPOINT3DSDK_INCLUDE_DIR}
  )

  LIST(APPEND ${PROJECT_NAME}_PRIVATE_LIBS
    ${REVOPOINT3DSDK_LIBRARIES}
  )

  LIST(APPEND External_Libraries_Install
    ${REVOPOINT3DSDK_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}3DCamera${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${REVOPOINT3DSDK_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}HandEye${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
ENDIF()

# --------------------------------------------------------------------------
# Build the library
LIST(APPEND ${PROJECT_NAME}_LIBS
  vtkPlusCommon
  vtkPlusUsSimulator
  vtkPlusVolumeReconstruction
  vtkPlusImageProcessing
  )
IF(PLUS_RENDERING_ENABLED)
  LIST(APPEND ${PROJECT_NAME}_LIBS
    vtkPlusRendering
    )
ENDIF()

LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRS
  ${PlusUsSimulator_INCLUDE_DIRS}
  )

SET(${PROJECT_NAME}_INCLUDE_DIRS ${${PROJECT_NAME}_INCLUDE_DIRS} CACHE INTERNAL "" FORCE) # Force parent scope update
GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library for interacting with virtual and hardware devices for data collection. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

IF(OpenCL_FOUND)
  # Enable downstream targets to use/link this device
  target_compile_definitions(vtk${PROJECT_NAME} PUBLIC -DPLUS_USE_OPENCL)
  SET(PLUS_USE_OPENCL ON CACHE INTERNAL "Use OpenCL accelerated functions in Plus.")
  MARK_AS_ADVANCED(PLUS_USE_OPENCL)
ENDIF()

IF(MSVC)
  # enable parallel in-target build as data collection is the largest project by far
  # do not add to interface defines as downstream targets don't necessarily need MP
  target_compile_options(vtk${PROJECT_NAME} PRIVATE /MP)
ENDIF()

TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_PRIVATE_LIBS})

IF(PLUS_USE_BLACKMAGIC_DECKLINK)
  DeckLinkGenerateAPIFiles(vtk${PROJECT_NAME})
  target_sources(vtk${PROJECT_NAME} PRIVATE ${MIDL_OUTPUT})
  LIST(APPEND BlackMagicDeckLink_SRCS ${MIDL_OUTPUT})
ENDIF()

# --------------------------------------------------------------------------
# Source groups
source_group("NVidiaDVP" FILES ${NVidiaDVP_HDRS} ${NVidiaDVP_SRCS})
source_group("OvrvisionPro" FILES ${OvrvisionPro_HDRS} ${OvrvisionPro_SRCS})
source_group("Philips-iE33" FILES ${Philips_HDRS} ${Philips_SRCS})
source_group("ThorLabs" FILES ${ThorLabs_HDRS} ${ThorLabs_SRCS})
source_group("Telemed" FILES ${Telemed_HDRS} ${Telemed_SRCS})
source_group("Interson" FILES ${Interson_HDRS} ${Interson_SRCS})
source_group("IntersonCXX" FILES ${IntersonCXX_HDRS} ${IntersonCXX_SRCS})
source_group("IntersonArrayCXX" FILES ${IntersonArrayCXX_HDRS} ${IntersonArrayCXX_SRCS})
source_group("Capistrano" FILES ${Capistrano_HDRS} ${Capistrano_SRCS})
source_group("WinProbe" FILES ${WinProbe_HDRS} ${WinProbe_SRCS})
source_group("Epiphan" FILES ${Epiphan_HDRS} ${Epiphan_SRCS})
source_group("MicrosoftMediaFoundation" FILES ${Mmf_HDRS} ${Mmf_SRCS})
source_group("VideoForWindows" FILES ${Vfw_HDRS} ${Vfw_SRCS})
source_group("daVinci" FILES ${daVinci_HDRS} ${daVinci_SRCS})
source_group("StealthLink" FILES ${StealthLink_HDRS} ${StealthLink_SRCS})
source_group("ICCapturing" FILES ${ICCapturing_HDRS} ${ICCapturing_SRCS})
source_group("BKProFocus" FILES ${BKProFocus_HDRS} ${BKProFocus_SRCS})
source_group("Ultrasonix" FILES ${Ultrasonix_HDRS} ${Ultrasonix_SRCS})
source_group("MicrochipMARG" FILES ${MicrochipMARG_HDRS} ${MicrochipMARG_SRCS})
source_group("Miscellaneous" FILES ${Miscellaneous_HDRS} ${Miscellaneous_SRCS})
source_group("ChRobotics" FILES ${ChRobotics_HDRS} ${ChRobotics_SRCS})
source_group("3dConnexion" FILES ${3dConnexion_HDRS} ${3dConnexion_SRCS})
source_group("PhidgetSpatial" FILES ${PhidgetSpatial_HDRS} ${PhidgetSpatial_SRCS})
source_group("Ascension3DGm" FILES ${Ascension3DGm_HDRS} ${Ascension3DGm_SRCS})
source_group("Ascension" FILES ${Ascension_HDRS} ${Ascension_SRCS})
source_group("USDigitalEncoders" FILES ${USDigitalEncoders_HDRS} ${USDigitalEncoders_SRCS})
source_group("BrachyTracker" FILES ${BrachyTracker_HDRS} ${BrachyTracker_SRCS})
source_group("IntelRealSense" FILES ${IntelRealSense_HDRS} ${IntelRealSense_SRCS})
source_group("MicronTracking" FILES ${MicronTracking_HDRS} ${MicronTracking_SRCS})
source_group("NDICAPI" FILES ${NDICAPI_HDRS} ${NDICAPI_SRCS})
source_group("Atracsys" FILES ${Atracsys_HDRS} ${Atracsys_SRCS})
source_group("PicoScope" FILES ${PicoScope_HDRS} ${PicoScope_SRCS})
source_group("NDICertus" FILES ${NDICertus_HDRS} ${NDICertus_SRCS})
source_group("Optimet" FILES ${Optimet_HDRS} ${Optimet_SRCS})
source_group("OptiTrack" FILES ${OptiTrack_HDRS} ${OptiTrack_SRCS})
source_group("SpinnakerAPI" FILES ${Spinnaker_HDRS} ${Spinnaker_SRCS})
source_group("VirtualDevices" FILES ${Virtual_HDRS} ${Virtual_SRCS})
source_group("OpenIGTLink" FILES ${OpenIGTLink_HDRS} ${OpenIGTLink_SRCS})
source_group("Common" FILES ${Common_HDRS} ${Common_SRCS})
source_group("Agilent" FILES ${Agilent_HDRS} ${Agilent_SRCS})
source_group("OpticalMarkerTracking" FILES ${OpticalMarkerTracking_HDRS} ${OpticalMarkerTracking_SRCS})
source_group("OpenCV" FILES ${OpenCV_Video_HDRS} ${OpenCV_Video_SRCS})
source_group("OpenHaptics" FILES ${OpenHaptics_HDRS} ${OpenHaptics_SRCS})
source_group("V4L2" FILES ${V4L2_Video_HDRS} ${V4L2_Video_SRCS})
source_group("InfraredSeekCam" FILES ${Infrared_SeekCam_HDRS} ${Infrared_SeekCam_SRCS})
source_group("WitMotionTracker" FILES ${WitMotionTracker_HDRS} ${WitMotionTracker_SRCS})
source_group("Andor" FILES ${Andor_HDRS} ${Andor_SRCS})

source_group("LeapMotion" FILES ${LeapMotion_HDRS} ${LeapMotion_SRCS})
source_group("ClariusVideo" FILES ${CLARIUS_HDRS} ${CLARIUS_SRCS})
source_group("ClariusOEM" FILES ${CLARIUS_OEM_HDRS} ${CLARIUS_OEM_SRCS})
source_group("BlackMagic" FILES ${BlackMagicDeckLink_HDRS} ${BlackMagicDeckLink_SRCS})
source_group("SteamVR" FILES ${STEAMVR_HDRS} ${STEAMVR_SRCS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE) # To force parent scope update
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj;vtkPlusCommon;vtkPlusUsSimulator;vtkPlusImageProcessing;${vtkSEIDrv_vcProj} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Copy external libraries to PLUS_EXECUTABLE_OUTPUT_PATH
IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
    )
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/RelWithDebInfo
    )
  FILE(COPY ${External_Libraries_Install}
    DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/MinSizeRel
    )

  # If debug and release dll has the same name then the debug dll would be
  # overwritten by the file copy. To prevent this, copy only those release dlls
  # that has no corresponding debug dll with the same filename.

  # Find the file names of the debug shared libraries
  FOREACH(_debug_lib ${External_Libraries_Debug})
    GET_FILENAME_COMPONENT(_file_name ${_debug_lib} NAME)
    LIST(APPEND External_Libraries_Debug_FileName ${_file_name})
  ENDFOREACH()

  # Extract the release shared libraries that have the same name as debug shared libraries
  FOREACH(_release_lib ${External_Libraries_Install})
    GET_FILENAME_COMPONENT(_file_name ${_release_lib} NAME)
    LIST(FIND External_Libraries_Debug_FileName ${_file_name} _has_file_name)
    IF(_has_file_name EQUAL -1)
      LIST(APPEND External_Libraries_Install_Minus_Debug ${_release_lib})
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
  ADD_EXECUTABLE(ViewSequenceFile Tools/ViewSequenceFile.cxx)
  SET_TARGET_PROPERTIES(ViewSequenceFile PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(ViewSequenceFile vtk${PROJECT_NAME})
  GENERATE_HELP_DOC(ViewSequenceFile)

  ADD_EXECUTABLE(PlusVersion Tools/PlusVersion.cxx)
  SET_TARGET_PROPERTIES(PlusVersion PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(PlusVersion vtkPlusCommon vtk${PROJECT_NAME})
  GENERATE_HELP_DOC(PlusVersion)

  # OpenIGTLink
  IF(PLUS_USE_OpenIGTLink)
    ADD_EXECUTABLE(BrainLabTrackerSim Tools/BrainLabTrackerSim.cxx)
    SET_TARGET_PROPERTIES(BrainLabTrackerSim PROPERTIES FOLDER Tools)
    TARGET_LINK_LIBRARIES(BrainLabTrackerSim OpenIGTLink vtkPlusCommon)
  ENDIF()

  INSTALL(TARGETS PlusVersion ViewSequenceFile EXPORT PlusLib
    RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  )
ENDIF()

IF(PLUS_USE_ATRACSYS)
  # build AtracsysMarkerCreator
  ADD_EXECUTABLE(AtracsysMarkerCreator Atracsys/AtracsysTracker.h Atracsys/AtracsysTracker.cxx Atracsys/AtracsysMarkerCreator.cxx)
  SET_TARGET_PROPERTIES(AtracsysMarkerCreator PROPERTIES FOLDER Tools)
  TARGET_LINK_LIBRARIES(AtracsysMarkerCreator PUBLIC ${${PROJECT_NAME}_LIBS})
  TARGET_LINK_LIBRARIES(AtracsysMarkerCreator PRIVATE ${${PROJECT_NAME}_PRIVATE_LIBS})
  FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(AtracsysMarkerCreator PUBLIC $<BUILD_INTERFACE:${p}>)
  ENDFOREACH()

  INSTALL(TARGETS AtracsysMarkerCreator EXPORT PlusLib
    RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})

# Introduce a CMAKE Build Flag, whether Static libraries should be installed?
INSTALL(FILES ${External_Libraries_Install}
  DESTINATION "${PLUSLIB_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  )
INSTALL(FILES ${External_Libraries_Static_Install}
  DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" COMPONENT RuntimeLibraries
  )

IF(PLUS_USE_OPTITRACK AND OPTITRACK_MOTIVE_VERSION VERSION_GREATER_EQUAL "2.1.1" AND OPTITRACK_MOTIVE_VERSION VERSION_LESS "3.0.0")
  # Copy additional OptiTrack libraries
  ADD_CUSTOM_COMMAND( TARGET vtk${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${OPTITRACK_MOTIVE_DIR}/platforms/qwindows.dll
    $<TARGET_FILE_DIR:vtk${PROJECT_NAME}>/platforms/qwindows.dll
    )
ENDIF()

# --------------------------------------------------------------------------
# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF()

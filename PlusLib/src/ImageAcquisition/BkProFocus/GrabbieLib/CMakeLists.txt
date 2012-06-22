PROJECT(GrabbieLib)

SET (GrabbieLib_INCLUDE_DIRS)
SET (GrabbieLib_LIBS)

SET (GrabbieLib_INCLUDE_DIRS ${GrabbieLib_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/AcquisitionLibrary/
  ${PROJECT_SOURCE_DIR}/LogLibrary/
  ${PROJECT_SOURCE_DIR}/ParametersLibrary/
  ${PROJECT_SOURCE_DIR}/CommandAndControl/
  ${PROJECT_SOURCE_DIR}/cfuff/
  ${PROJECT_SOURCE_DIR}/BkmDataFileLib
  ${PROJECT_SOURCE_DIR}/OemParams/
  )

# AcquisitionLibrary
  
SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  AcquisitionLibrary/Acquisition.cpp
  AcquisitionLibrary/AcquisitionInjector.cpp
  AcquisitionLibrary/BmodeViewDataReceiver.cpp 
  AcquisitionLibrary/libbmode.c
  )

# Sapera-specific acquisition classes
SET (GrabbieLib_INCLUDE_DIRS ${GrabbieLib_INCLUDE_DIRS}
  ${DALSASAPERA_DIR}/Classes/Basic
  ${DALSASAPERA_DIR}/include
  )
SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  AcquisitionLibrary/AcquisitionGrabberSapera.cpp
  AcquisitionLibrary/AcquisitionSettings.cpp
  AcquisitionLibrary/ResearchInterface.cpp 
  AcquisitionLibrary/SaperaProcessor.cpp 
  AcquisitionLibrary/SaperaViewDataReceiver.cpp 
  AcquisitionLibrary/Serial.cpp 
  )
SET(GrabbieLib_LIBS ${GrabbieLib_LIBS}
  ${DALSASAPERA_LIB_DIR}/SapClassBasic${CMAKE_STATIC_LIBRARY_SUFFIX}
  )

# BkmDataFileLib 

SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  BkmDataFileLib/BkmDataFile.cpp
  BkmDataFileLib/BkmDataFileUtil.cpp
  )

# cfuff

SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  cfuff/cfu_scan.c
  )

# CommandAndControl

SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  CommandAndControl/CommandAndControl.cpp
  CommandAndControl/ObservedCmdCtrlState.cpp
  )
  
# LogLibrary

SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  LogLibrary/LogLibrary.cpp
  )  

# OemParams

SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  OemParams/OemParams.cpp 
  )  
  
# ParametersLibrary 
  
SET(GrabbieLib_SRCS ${GrabbieLib_SRCS}
  ParametersLibrary/ParamConnectionSettings.cpp 
  ParametersLibrary/ParamSyncConnection.cpp 
  ParametersLibrary/TCPClient.cpp 
  ParametersLibrary/WSAIF.cpp 
  ParametersLibrary/UseCaseParser.cpp 
  )  
 
INCLUDE_DIRECTORIES( ${GrabbieLib_INCLUDE_DIRS} )
SET (GrabbieLib_INCLUDE_DIRS ${GrabbieLib_INCLUDE_DIRS} CACHE INTERNAL "")

# Disable non-secure CRT call warnings (sptrinf, etc.)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

ADD_LIBRARY(GrabbieLib STATIC ${GrabbieLib_SRCS})
TARGET_LINK_LIBRARIES( GrabbieLib ${GrabbieLib_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} GrabbieLib CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_GrabbieLib GrabbieLib;"${PlusLib_BINARY_DIR}/src/ImageAcquisition/GrabbieLib/GrabbieLib.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib GrabbieLib)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

if(CMAKE_VERSION VERSION_LESS 3.15.1)
  message(WARNING "Beware of C++/CLI bug:\n  https://gitlab.kitware.com/cmake/cmake/issues/19480")
  message(STATUS "The error message has the form:\n  %Temp%\\.NETFramework,Version=v4.0.AssemblyAttributes.cpp : fatal error C1083: Cannot open compiler generated file: 'Debug/.NETFramework,Version=v4.0.AssemblyAttributes.asm': No such file or directory")
  message("If you encounter it, use CMake 3.15.1 or later")
endif()

PROJECT(SEIDrv)

# Sources
SET(${PROJECT_NAME}_SRCS
    SEIDrv.cpp
    )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode") 
  SET(${PROJECT_NAME}_HDRS
    SEIDrv.h
    )
ENDIF()

SET(${PROJECT_NAME}_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
SET(${PROJECT_NAME}_LIBS
  )

GENERATE_EXPORT_DIRECTIVE_FILE(vtk${PROJECT_NAME})
ADD_LIBRARY(vtk${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  TARGET_INCLUDE_DIRECTORIES(vtk${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
TARGET_INCLUDE_DIRECTORIES(vtk${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(vtk${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_LIBS})
PlusLibAddVersionInfo(vtk${PROJECT_NAME} "Library for interfacing with USDigital encoders. Part of the Plus toolkit." vtk${PROJECT_NAME} vtk${PROJECT_NAME})

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(Bitness "x64")
ELSE()
  SET(Bitness "x86")
ENDIF()

SET_TARGET_PROPERTIES(vtk${PROJECT_NAME} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
SET_TARGET_PROPERTIES(vtk${PROJECT_NAME} PROPERTIES VS_DOTNET_REFERENCE_USDSEI "${CMAKE_CURRENT_LIST_DIR}/${Bitness}/USDSEI.dll")
SET_TARGET_PROPERTIES(vtk${PROJECT_NAME} PROPERTIES VS_DOTNET_REFERENCE_USDigital "${CMAKE_CURRENT_LIST_DIR}/${Bitness}/USDigital.dll")

# required to display reference assemblies in Visual Studio GUI
SET_PROPERTY(TARGET vtk${PROJECT_NAME} PROPERTY VS_GLOBAL_CLRSupport "true")

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtk${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtk${PROJECT_NAME} vtk${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/${PROJECT_NAME}/vtk${PROJECT_NAME}.vcxproj;vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(vtk${PROJECT_NAME} ${PROJECT_NAME})

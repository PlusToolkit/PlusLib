# _____________________________________________________________________________
#
#       OemParametersLibrary
# _____________________________________________________________________________
#

# -----------------------------------------------------------------------------
#  List the header files. The get installed together with the Lib files
# -----------------------------------------------------------------------------
# set(HDR OemParams.h)
set(HDR ${HDR} OemParamsList.h)
set(HDR ${HDR} ParamConnectionSettings.h)
set(HDR ${HDR} ParamSyncConnection.h)
set(HDR ${HDR} targetver.h)
set(HDR ${HDR} TCPClient.h)
set(HDR ${HDR} WSAIF.h)
set(HDR ${HDR} stdint.h)
set(HDR ${HDR} UseCaseParser.h)
set(HDR ${HDR} UseCaseStructs.h)


# -----------------------------------------------------------------------------
#   Enumerate the CPP files. Used for building. Installed with the source
# -----------------------------------------------------------------------------
# set(CPP ${CPP} OemParams.cpp)
set(CPP ${CPP} ParamConnectionSettings.cpp)
set(CPP ${CPP} ParamSyncConnection.cpp)
set(CPP ${CPP} TCPClient.cpp)
set(CPP ${CPP} WSAIF.cpp)
set(CPP ${CPP} UseCaseParser.cpp)

# -----------------------------------------------------------------------------
#   Set precompiled headers for MSVC
# -----------------------------------------------------------------------------
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRC_FILE)

set(CPP ${CPP} stdafx.h stdafx.cpp)

# -----------------------------------------------------------------------------
# This is Windows Console Application.
# It must compile using MFC as a shared library
# -----------------------------------------------------------------------------

ADD_DEFINITIONS(-D_AFXDLL -D_CRT_SECURE_NO_WARNINGS -W4 -WX)
SET(CMAKE_MFC_FLAG 2)


# -----------------------------------------------------------------------------
#      Add the target
# -----------------------------------------------------------------------------
add_library(ParametersLibrary STATIC ${CPP} ${HDR})
target_link_libraries(ParametersLibrary OemParams)

# -----------------------------------------------------------------------------
#   Installation instruction
# -----------------------------------------------------------------------------

install (TARGETS ParametersLibrary
         ARCHIVE
         DESTINATION lib
         COMPONENT Libraries)


install (FILES ${HDR}
         DESTINATION include
         COMPONENT Libraries)


install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         DESTINATION SourceCode
         COMPONENT Source)

# ----------------------------------------------------------------------------
# Doxygen documentation
# ----------------------------------------------------------------------------
if (BUILD_DOXYGEN_DOCS)
	add_doxygen_target(ParametersLibrary_Doc)
endif(BUILD_DOXYGEN_DOCS)




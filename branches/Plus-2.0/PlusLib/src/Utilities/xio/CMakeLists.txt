PROJECT(vtkxio)

SET(SRCS
  AhrsAlgo.h
  MadgwickAhrsAlgo.cxx
  MadgwickAhrsAlgo.h
  MahonyAhrsAlgo.cxx
  MahonyAhrsAlgo.h 
  )

ADD_LIBRARY(vtkxio STATIC ${SRCS})
TARGET_LINK_LIBRARIES( vtkxio )

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "" FORCE) 

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkxio CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkxio vtkxio;"${PlusLib_BINARY_DIR}/src/Utilities/xio/vtkxio.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib vtkxio)

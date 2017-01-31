PROJECT(vtkxio)

SET(vtkxio_SRCS
  MadgwickAhrsAlgo.cxx
  MahonyAhrsAlgo.cxx
  )

SET(vtkxio_HDRS
  AhrsAlgo.h
  MadgwickAhrsAlgo.h
  MahonyAhrsAlgo.h 
  )

ADD_LIBRARY(vtkxio STATIC ${vtkxio_SRCS} ${vtkxio_HDRS})
SET_TARGET_PROPERTIES(vtkxio PROPERTIES FOLDER Utilities)
target_include_directories(vtkxio PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
  $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}/vtkxio>
  )
TARGET_LINK_LIBRARIES( vtkxio )

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "" FORCE) 

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkxio CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkxio vtkxio;${PlusLib_BINARY_DIR}/src/Utilities/xio/vtkxio.vcxproj CACHE INTERNAL "" FORCE)

INSTALL(TARGETS vtkxio EXPORT PlusLib 
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}"
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}"
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}"
  )
INSTALL(FILES ${vtkxio_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}/vtkxio"
  )
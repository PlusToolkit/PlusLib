# Developed by MACBIOIDI-ULPGC & IACTEC-IAC group
CMAKE_MINIMUM_REQUIRED(VERSION 3.3.0)

PROJECT(SeekCameraLib)

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

  SET(${PROJECT_NAME}_SRCS
   SeekCam.cpp
   SeekDevice.cpp
   SeekThermal.cpp
   SeekThermalPro.cpp
  )

  SET(${PROJECT_NAME}_HDRS
   seek.h
   SeekCam.h
   SeekDevice.h
   SeekLogging.h
   SeekThermal.h
   SeekThermalPro.h
  )

  
 # libusb-1.0 -----
find_path(LIBUSB_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES "include" "libusb" "libusb-1.0"
)

find_library(LIBUSB_LIBRARY
  NAMES usb-1.0
  PATH_SUFFIXES "lib" "lib32" "lib64"
)

if (NOT LIBUSB_LIBRARY)
    message(FATAL_ERROR "Libusb library not found")
endif()

FIND_PACKAGE(OpenCV REQUIRED PATHS ${OpenCV_DIR})

if (NOT OpenCV_FOUND)
  message(FATAL_ERROR "OpenCv library not found")
endif()

SET(Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${${PROJECT_NAME}_INCLUDE_DIRS} CACHE INTERNAL "")


# create library
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SeekCameraLib_SRCS} ${SeekCameraLib_HDRS})

# add include directories in the build
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${LIBUSB_INCLUDE_DIR}>)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${OpenCV_INCLUDE_DIRS}>)

# link libraries for new library
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES} ${LIBUSB_LIBRARY})

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} ${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)


INSTALL(TARGETS ${PROJECT_NAME} EXPORT PlusLib 
  RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}"
  LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}"
  ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}"
  )
INSTALL(FILES ${${PROJECT_NAME}_HDRS}
  DESTINATION "${PLUSLIB_INCLUDE_INSTALL}/${PROJECT_NAME}"
  )

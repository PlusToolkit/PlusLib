PROJECT(EpiphanIDE)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

SET(EpiphanInterface_SRCS
  v2u_version.h
  frmgrab.h
  v2u_compression.h
  v2u_defs.h
  v2u_dshow.h
  v2u_id.h
  v2u_ioctl.h
  v2u_lib.h
  v2u_save.h
  v2u_sys.h
  v2u_util.h
  )

ADD_CUSTOM_TARGET(EpiphanIDE SOURCES ${EpiphanInterface_SRCS})
SET_PROPERTY(TARGET EpiphanIDE PROPERTY FOLDER Interfaces)
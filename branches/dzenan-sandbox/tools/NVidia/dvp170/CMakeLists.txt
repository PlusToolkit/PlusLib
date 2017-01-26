PROJECT(NVidiaIDE)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

SET(NVidiaInterface_SRCS
  include/dvpapi_gl.h
  include/DVPAPI.h
  include/dvpapi_cuda.h
  include/dvpapi_d3d9.h
  include/dvpapi_d3d10.h
  include/dvpapi_d3d11.h
  )

ADD_CUSTOM_TARGET(NVidiaIDE SOURCES ${NVidiaInterface_SRCS})
SET_PROPERTY(TARGET NVidiaIDE PROPERTY FOLDER Interfaces)
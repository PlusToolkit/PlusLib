cmake_minimum_required(VERSION 3.12)

project(SEIcppCLI)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(Bitness "x64")
else()
  set(Bitness "x86")
endif()

add_library(SEIDrv STATIC SEIDrv.cpp)
set_target_properties(SEIDrv PROPERTIES COMMON_LANGUAGE_RUNTIME "")
set_target_properties(SEIDrv PROPERTIES VS_DOTNET_REFERENCE_USDSEI "${CMAKE_CURRENT_LIST_DIR}/${Bitness}/USDSEI.dll")
set_target_properties(SEIDrv PROPERTIES VS_DOTNET_REFERENCE_USDigital "${CMAKE_CURRENT_LIST_DIR}/${Bitness}/USDigital.dll")

# required to display reference assemblies in Visual Studio GUI
set_property(TARGET SEIDrv PROPERTY VS_GLOBAL_CLRSupport "true")

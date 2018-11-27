MACRO(PlusLibInstallLibrary _target_name _variable_root)
  INSTALL(TARGETS ${_target_name} EXPORT PlusLib
    RUNTIME DESTINATION "${PLUSLIB_BINARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
    LIBRARY DESTINATION "${PLUSLIB_LIBRARY_INSTALL}" CONFIGURATIONS Release COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION "${PLUSLIB_ARCHIVE_INSTALL}" CONFIGURATIONS Release COMPONENT Development
    )
  INSTALL(FILES ${${_variable_root}_HDRS}
    DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" COMPONENT Development
    )
  GET_TARGET_PROPERTY(_library_type ${_target_name} TYPE)
  IF(${_library_type} STREQUAL SHARED_LIBRARY AND MSVC)
    INSTALL(FILES "$<TARGET_PDB_FILE:${_target_name}>" OPTIONAL
      DESTINATION "${PLUSLIB_BINARY_INSTALL}" COMPONENT RuntimeLibraries
      )
  ENDIF()
ENDMACRO()

MACRO(PlusLibAddVersionInfo target_name description internal_name product_name)
  IF(MSVC)
    IF(NOT TARGET ${target_name})
      MESSAGE(FATAL_ERROR PlusLibAddVersionInfo called but target parameter does not exist)
    ENDIF()

    # Configure file does not see these variables unless we re-set them locally
    set(target_name ${target_name})
    set(description ${description})
    set(internal_name ${internal_name})
    set(product_name ${product_name})

    CONFIGURE_FILE(
      ${CMAKE_SOURCE_DIR}/CMake/MSVCVersion.rc.in 
      ${CMAKE_CURRENT_BINARY_DIR}/${target_name}MSVCVersion.rc
      )

    GET_TARGET_PROPERTY(_target_type ${target_name} TYPE)
    IF(${_target_type} STREQUAL "EXECUTABLE")
      TARGET_SOURCES(${target_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/${target_name}MSVCVersion.rc)
    ELSE()
      TARGET_SOURCES(${target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${target_name}MSVCVersion.rc)
    ENDIF()

    # If this macro was called from the target they're currently configuring
    IF(${PROJECT_NAME} STREQUAL ${target_name} OR vtk${PROJECT_NAME} STREQUAL ${target_name})
      SOURCE_GROUP(Resources FILES ${CMAKE_CURRENT_BINARY_DIR}/${target_name}MSVCVersion.rc)
    ENDIF()
  ENDIF()
ENDMACRO()
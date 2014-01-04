find_package( Doxygen QUIET)

if( DOXYGEN_FOUND )
  if( DOXYGEN_DOT_FOUND )

    # Automatically generate documentation at build time
    set(all_arg)
    if(DOCUMENTATION_TARGET_IN_ALL)
      set(all_arg "ALL")
    endif()

    # Build html help (chm) file on Windows
    set(GENERATE_HTMLHELP "NO")
    set(COMPRESSED_HELP_TARGET_FILE_EXT ".tar.gz")
    if(WIN32)
      set(GENERATE_HTMLHELP "YES")
      set(COMPRESSED_HELP_TARGET_FILE_EXT ".chm")
      set(HHC_LOCATION "${PLUSLIB_TOOLS_DIR}/HtmlHelp/hhc.exe")
    endif(WIN32) 
       
    # Parameters for generate_doc.cmake
    set(TARGET_NAME "Documentation")
    set(UNCOMPRESSED_HELP_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME})
    set(DOXYGEN_CONFIG_FILE ${UNCOMPRESSED_HELP_TARGET_DIR}/Doxyfile.txt)      
    set(CREATE_COMPRESSED_HELP "YES")
    set(COMPRESSED_HELP_TARGET_DIR "${PLUS_EXECUTABLE_OUTPUT_PATH}/Doc")
    set(COMPRESSED_HELP_TARGET_FILE "PlusApp-${TARGET_NAME}${COMPRESSED_HELP_TARGET_FILE_EXT}")
    # Other parameters
    set(DOC_OUTPUT_FILE "${COMPRESSED_HELP_TARGET_DIR}/${COMPRESSED_HELP_TARGET_FILE}")
    set(DOC_GENERATION_CMAKE_FILE ${UNCOMPRESSED_HELP_TARGET_DIR}/generate_doc.cmake)
    # Generate doxygen configuration and generator file
    configure_file( Doxyfile.txt.in
      ${DOXYGEN_CONFIG_FILE}
      )
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/generate_doc.cmake.in
      ${DOC_GENERATION_CMAKE_FILE}
      @ONLY
      )
    # Run doxygen as a custom command driven by a custom target.
    add_custom_command(
      OUTPUT ${DOC_OUTPUT_FILE}
      COMMAND ${CMAKE_COMMAND} -P "${DOC_GENERATION_CMAKE_FILE}"
      DEPENDS "${DOC_GENERATION_CMAKE_FILE}"
              "${DOXYGEN_CONFIG_FILE}"
      )
    add_custom_target(${TARGET_NAME} ${all_arg} DEPENDS ${DOC_OUTPUT_FILE})
    set_target_properties(${TARGET_NAME} PROPERTIES LABELS Documentation)

  else( DOXYGEN_DOT_FOUND )
    message( WARNING
      "Warning: Dot not found - Documentation will not be created"
      )
  endif( DOXYGEN_DOT_FOUND )
endif( DOXYGEN_FOUND )

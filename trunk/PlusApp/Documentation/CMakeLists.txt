find_package( Doxygen REQUIRED)

if( DOXYGEN_FOUND )
  if( DOXYGEN_DOT_FOUND )

    # Automatically generate documentation at build time
    set(all_arg)
    if(DOCUMENTATION_TARGET_IN_ALL)
      set(all_arg "ALL")
    endif()

    # Set up help file format
    set(PLUSAPP_DOCUMENTATION_FORMAT "PLAIN_HTML" CACHE STRING "User manual format. PLAIN_HTML: HTML files with limited search. SERVER_INDEXED_HTML: HTML files with server-based full-text search (requires PHP). WINDOWS_HELP: single compressed html (.chm) help file.")
    set_property(CACHE PLUSAPP_DOCUMENTATION_FORMAT PROPERTY STRINGS PLAIN_HTML SERVER_INDEXED_HTML WINDOWS_HELP)
 
    # Default format (plain HTML)
    set(GENERATE_HTMLHELP "NO")
    set(COMPRESSED_HELP_TARGET_FILE_EXT ".tar.gz")
    set(SEARCHENGINE "YES")
    set(SERVER_BASED_SEARCH "NO")    
    IF (PLUSAPP_DOCUMENTATION_FORMAT STREQUAL "WINDOWS_HELP")
      IF(WIN32)
        set(GENERATE_HTMLHELP "YES")
        set(SEARCHENGINE "NO")
        set(COMPRESSED_HELP_TARGET_FILE_EXT ".chm")
        set(HHC_LOCATION "${PLUSLIB_TOOLS_DIR}/HtmlHelp/hhc.exe" CACHE FILEPATH "Path to the Windows HTML Help Compiler tool." FORCE)
      ELSE(WIN32)
        MESSAGE(WARNING "Build html help (chm) file is only supported on Windows")
      ENDIF(WIN32)
    ELSEIF (PLUSAPP_DOCUMENTATION_FORMAT STREQUAL "SERVER_INDEXED_HTML")
      set(SERVER_BASED_SEARCH "YES")
    ELSEIF (PLUSAPP_DOCUMENTATION_FORMAT STREQUAL "PLAIN_HTML")
      # it's the default, no changes are needed
    ELSE()
      MESSAGE(WARNING "Invalid PLUSAPP_DOCUMENTATION_FORMAT: ${PLUSAPP_DOCUMENTATION_FORMAT}")
    ENDIF()  
  
    # Parameters for generate_doc.cmake
    set(TARGET_NAME "UserManual")
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
    add_custom_target(Documentation-PlusApp-${TARGET_NAME} ${all_arg} DEPENDS ${DOC_OUTPUT_FILE})
    set_target_properties(Documentation-PlusApp-${TARGET_NAME} PROPERTIES LABELS Documentation)

  else( DOXYGEN_DOT_FOUND )
    message( WARNING
      "Warning: Dot not found - Documentation will not be created"
      )
  endif( DOXYGEN_DOT_FOUND )
endif( DOXYGEN_FOUND )

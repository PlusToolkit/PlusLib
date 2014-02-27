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
      set(DOXYGEN_HHC_EXECUTABLE "${PLUSLIB_TOOLS_DIR}/HtmlHelp/hhc.exe" CACHE PATH "Path to the Windows HTML Help Compiler tool." FORCE)
    endif(WIN32) 
   
    MACRO(GENERATE_DOC TARGET_NAME)
      # Parameters for generate_doc.cmake
      set(UNCOMPRESSED_HELP_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME})
      set(DOXYGEN_CONFIG_FILE ${UNCOMPRESSED_HELP_TARGET_DIR}/Doxyfile.txt)      
      set(CREATE_COMPRESSED_HELP "YES")
      set(COMPRESSED_HELP_TARGET_DIR "${PLUS_EXECUTABLE_OUTPUT_PATH}/Doc")
      set(COMPRESSED_HELP_TARGET_FILE "PlusLib-${TARGET_NAME}${COMPRESSED_HELP_TARGET_FILE_EXT}")
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
    ENDMACRO(GENERATE_DOC)

    # User documentation
    set( PlusLib_USER_DOCUMENTATION "YES" )
    set( PlusLib_DEV_DOCUMENTATION "NO" )
    set( PlusLib_ADDITIONAL_EXCLUDE_PATTERN "*_p.h  *Private.*  */Testing/*" )
    GENERATE_DOC(doc-usr)

    # Dev documentation
    set( PlusLib_USER_DOCUMENTATION "NO" )
    set( PlusLib_DEV_DOCUMENTATION "YES" )
    set( PlusLib_ADDITIONAL_EXCLUDE_PATTERN "" )
    set( PlusLib_ADDITIONAL_FILE_PATTERN "*.c \  *.cc \  *.cxx \  *.cpp \  *.c++ \  *.hxx \  *.hpp \  *.h++ \  *.txx \  *.C \  *.CC \  *.C++ \  *.moc" )
    GENERATE_DOC(doc-dev)

  else( DOXYGEN_DOT_FOUND )
    message( STATUS
      "Documentation: Graphviz dot tool not found (http://www.graphviz.org/Download.php, required by Doxygen for diagram generation)- Documentation will not be created"
      )
  endif( DOXYGEN_DOT_FOUND )
endif( DOXYGEN_FOUND )

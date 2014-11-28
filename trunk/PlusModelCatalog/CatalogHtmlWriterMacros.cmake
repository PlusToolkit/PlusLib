MACRO(MODEL_TABLE_START MODEL_TABLE_NAME MODEL_TABLE_DESCRIPTION)
  SET(PAGE_BODY "${PAGE_BODY}
    <h2>${MODEL_TABLE_NAME}</h2>
    <p>${MODEL_TABLE_DESCRIPTION}</p>
    <p>
    <table>
      <thead>
        <tr>
          <th>Image</th>
          <th>ID</td>
          <th>Description</th>
          <th>Printable model</th>
        </tr>
      </thead>
      <tbody>")
ENDMACRO(MODEL_TABLE_START)

MACRO(MODEL_TABLE_ROW)
  set(options "" )
  set(oneValueArgs ID IMAGE_FILE DESCRIPTION EDIT_LINK)
  set(multiValueArgs PRINTABLE_FILES)
  cmake_parse_arguments(MODEL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  IF(NOT MODEL_PRINTABLE_FILES)
    SET(MODEL_PRINTABLE_FILES "TrackingFixtures/${MODEL_ID}.stl")
  ENDIF()  
  IF(NOT MODEL_IMAGE_FILE)
    SET(MODEL_IMAGE_FILE "${MODEL_ID}.png")
    SET(MODEL_PRINTABLE_FILES_LIST "${MODEL_PRINTABLE_FILES};")
    LIST(GET MODEL_PRINTABLE_FILES_LIST 0 FIRST_MODEL_PRINTABLE_FILE)
    CREATE_MODEL_IMAGE("${CMAKE_CURRENT_SOURCE_DIR}/${FIRST_MODEL_PRINTABLE_FILE}" "${HTML_OUTPUT_DIR}/rendered/${MODEL_IMAGE_FILE}")
  ELSE()
    FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_IMAGE_FILE} DESTINATION "${HTML_OUTPUT_DIR}/rendered")
  ENDIF()
    
  IF(NOT MODEL_EDIT_LINK)
    SET(MODEL_EDIT_LINK "${CATALOG_URL}/TrackingFixtures")
  ENDIF()
  FOREACH(MODEL_PRINTABLE_FILE ${MODEL_PRINTABLE_FILES})
    FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_PRINTABLE_FILE} DESTINATION "${HTML_OUTPUT_DIR}/printable")
  ENDFOREACH()

  GET_FILENAME_COMPONENT(MODEL_IMAGE_FILE_NAME ${MODEL_IMAGE_FILE} NAME)
  
  SET(PAGE_BODY "${PAGE_BODY}
          <tr>
          <td><img class=\"model\" src=\"rendered/${MODEL_IMAGE_FILE_NAME}\"></td>
          <td>${MODEL_ID}<br><a href=\"${MODEL_EDIT_LINK}\"><img src=\"link.png\"></a></td>
          <td>${MODEL_DESCRIPTION}</td>  
          <td>")
  FOREACH(MODEL_PRINTABLE_FILE ${MODEL_PRINTABLE_FILES})
    GET_FILENAME_COMPONENT(MODEL_PRINTABLE_FILE_NAME ${MODEL_PRINTABLE_FILE} NAME)
    Subversion_WC_INFO("${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_PRINTABLE_FILE}" PrintableModelFile)
    SET(PRINTABLE_MODEL_REV "${PrintableModelFile_WC_LAST_CHANGED_REV}")
    SET(PAGE_BODY "${PAGE_BODY} <a href=\"printable/${MODEL_PRINTABLE_FILE_NAME}\">${MODEL_PRINTABLE_FILE_NAME}</a> (rev.${PRINTABLE_MODEL_REV})<br>")
  ENDFOREACH()
  SET(PAGE_BODY "${PAGE_BODY} </td>
        </tr>")
        
ENDMACRO(MODEL_TABLE_ROW)

MACRO(MODEL_TABLE_END)
  SET(PAGE_BODY "${PAGE_BODY}
      </tbody>
    </table>")
ENDMACRO(MODEL_TABLE_END)

MACRO(PARAGRAPH TEXT)
  SET(PAGE_BODY
    "${PAGE_BODY} <p>${TEXT}</p>")
ENDMACRO(PARAGRAPH)

MACRO (TODAY RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE ".* (..)/(..)/(....).*" "\\1/\\2/\\3" ${RESULT} ${${RESULT}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1/\\2/\\3" ${RESULT} ${${RESULT}})
    ELSE (WIN32)
        MESSAGE(WARNING "date not implemented")
        SET(${RESULT} "unknown")
    ENDIF (WIN32)
ENDMACRO (TODAY)

MACRO(MODEL_CATALOG_START)
  SET(PAGE_BODY "" )
ENDMACRO(MODEL_CATALOG_START)

MACRO(MODEL_CATALOG_END)
  TODAY(CURRENT_DATETIME)
  PARAGRAPH("<br>Generated: ${CURRENT_DATETIME}")
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/CatalogTemplate.html.in
    ${HTML_OUTPUT_DIR}/PlusPrintableModelsCatalog.html
    )
  FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/link.png DESTINATION "${HTML_OUTPUT_DIR}")
ENDMACRO(MODEL_CATALOG_END)

MACRO (CREATE_MODEL_IMAGE INPUT_STL_FILE OUTPUT_PNG_FILE)
  SET(MODEL_RENDERER_EXE "${PLUS_EXECUTABLE_OUTPUT_PATH}/Release/ModelRenderer")
  GET_FILENAME_COMPONENT(BASE_FILENAME ${INPUT_STL_FILE} NAME_WE)
  add_custom_command(
    DEPENDS ${INPUT_STL_FILE} ModelRenderer
    COMMAND ${MODEL_RENDERER_EXE} --model-file=${INPUT_STL_FILE} --output-image-file=${OUTPUT_PNG_FILE}
    OUTPUT ${OUTPUT_PNG_FILE}
    )
  add_custom_target(ModelRenderer-${BASE_FILENAME} DEPENDS ${OUTPUT_PNG_FILE})
  set_target_properties(ModelRenderer-${BASE_FILENAME} PROPERTIES LABELS ModelRenderer)
  set_target_properties(ModelRenderer-${BASE_FILENAME} PROPERTIES EXCLUDE_FROM_ALL FALSE)
ENDMACRO (CREATE_MODEL_IMAGE)

SET(tesseract_DEPENDENCIES leptonica)
LIST(APPEND PlusLib_DEPENDENCIES tesseract)
SET(libpng_DEPENDENCIES zlib)
SET(tesseract_ROOT_DIR ${CMAKE_BINARY_DIR}/tesseract-super)

# --------------------------------------------------------------------------
# zlib for leptonica
SET(leptonica_DEPENDENCIES libpng)
SET (PLUS_zlib_src_DIR ${tesseract_ROOT_DIR}/zlib CACHE INTERNAL "Path to store zlib contents.")
SET (PLUS_zlib_prefix_DIR ${tesseract_ROOT_DIR}/zlib-prefix CACHE INTERNAL "Path to store zlib prefix data.")
SET (PLUS_zlib_DIR "${tesseract_ROOT_DIR}/zlib-bin" CACHE INTERNAL "Path to store zlib binaries")
ExternalProject_Add( zlib
    PREFIX ${PLUS_zlib_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_zlib_src_DIR}"
    BINARY_DIR "${PLUS_zlib_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/zlib.git" 
    GIT_TAG 023318cb972054147677a0d56f502a9d0f8bbd7f 
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags} 
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags} 
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH} 
        -DCMAKE_INSTALL_PREFIX:PATH=${PLUS_zlib_DIR}
    #--Build step-----------------
    #--Install step-----------------
    #--Dependencies-----------------
    DEPENDS "" 
    )

# --------------------------------------------------------------------------
# libpng for leptonica
SET(leptonica_DEPENDENCIES libpng)
SET (PLUS_libpng_src_DIR ${tesseract_ROOT_DIR}/libpng CACHE INTERNAL "Path to store libpng contents.")
SET (PLUS_libpng_prefix_DIR ${tesseract_ROOT_DIR}/libpng-prefix CACHE INTERNAL "Path to store libpng prefix data.")
SET (PLUS_libpng_DIR "${tesseract_ROOT_DIR}/libpng-bin" CACHE INTERNAL "Path to store libpng binaries")
ExternalProject_Add( libpng
    PREFIX ${PLUS_libpng_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_libpng_src_DIR}"
    BINARY_DIR "${PLUS_libpng_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/libpng.git"
    GIT_TAG 9e5ed9232de5e1471104d88dd963e713d090f86d 
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags} 
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags} 
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH} 
        -Dzlib_DIR:PATH=${PLUS_zlib_DIR} 
        -DCMAKE_INSTALL_PREFIX:PATH=${PLUS_libpng_DIR}
    #--Build step-----------------
    #--Install step-----------------
    #--Dependencies-----------------
    DEPENDS ${libpng_DEPENDENCIES} 
    )

# --------------------------------------------------------------------------
# leptonica
SET (PLUS_leptonica_src_DIR ${tesseract_ROOT_DIR}/leptonica CACHE INTERNAL "Path to store leptonica contents.")
SET (PLUS_leptonica_prefix_DIR ${tesseract_ROOT_DIR}/leptonica-prefix CACHE INTERNAL "Path to store leptonica prefix data.")
SET (PLUS_leptonica_DIR "${tesseract_ROOT_DIR}/leptonica-bin" CACHE INTERNAL "Path to store leptonica binaries")
ExternalProject_Add( leptonica
    PREFIX ${PLUS_leptonica_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_leptonica_src_DIR}"
    BINARY_DIR "${PLUS_leptonica_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/leptonica.git"
    GIT_TAG 5d2f1eb7cb023d971dc96062f07baed6f7bb7584
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DPNG_DIR:PATH=${PLUS_libpng_DIR}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND "" #don't install
    #--Dependencies-----------------
    DEPENDS ${leptonica_DEPENDENCIES}
    )

# --------------------------------------------------------------------------
# tessdata
SET (PLUS_tessdata_src_DIR ${tesseract_ROOT_DIR}/tessdata CACHE INTERNAL "Path to store tesseract language data contents.")
SET (PLUS_tessdata_prefix_DIR ${tesseract_ROOT_DIR}/tessdata-prefix CACHE INTERNAL "Path to store tesseract language prefix data.")
ExternalProject_Add( tessdata
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    PREFIX ${PLUS_tessdata_prefix_DIR}
    SOURCE_DIR "${PLUS_tessdata_src_DIR}"
    BINARY_DIR "${PLUS_tessdata_src_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/tessdata.git"
    GIT_TAG 52f304eb1c8b9456adf1c2b8431cc855623a6d26
    #--Configure step-------------
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND ""
    #--Install step-----------------
    #--Dependencies-----------------
    INSTALL_COMMAND ""
    DEPENDS ""
    )
SET( tesseract_DEPENDENCIES ${tesseract_DEPENDENCIES} tessdata )
IF( WIN32 )
  MESSAGE(STATUS "Setting TESSDATA_PREFIX environment variable to enable loading of OCR languages.")
  execute_process(COMMAND setx TESSDATA_PREFIX ${PLUS_tessdata_src_DIR})
ENDIF( WIN32 )
# TODO: else linux, export env var? I don't know if CMake can do that...

# --------------------------------------------------------------------------
# tesseract-ocr-cmake
SET (PLUS_tesseract_src_DIR ${tesseract_ROOT_DIR}/tesseract CACHE INTERNAL "Path to store tesseract contents.")
SET (PLUS_tesseract_prefix_DIR ${tesseract_ROOT_DIR}/tesseract-prefix CACHE INTERNAL "Path to store tesseract prefix data.")
SET (PLUS_tesseract_DIR "${tesseract_ROOT_DIR}/tesseract-bin" CACHE INTERNAL "Path to store tesseract binaries")
ExternalProject_Add( tesseract
    PREFIX ${PLUS_tesseract_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_tesseract_src_DIR}"
    BINARY_DIR "${PLUS_tesseract_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/tesseract-ocr-cmake.git"
    GIT_TAG cd300ab908ed39c739c1047805df22a4d3cae7f8
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DCMAKE_INSTALL_PREFIX:PATH=${PLUS_tesseract_DIR}
        -DLeptonica_DIR:PATH=${PLUS_leptonica_DIR}
    #--Build step-----------------
    #--Install step-----------------
    #--Dependencies-----------------
    DEPENDS ${tesseract_DEPENDENCIES}
    )
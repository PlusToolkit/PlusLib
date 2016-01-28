LIST(APPEND PlusLib_DEPENDENCIES tesseract)

# --------------------------------------------------------------------------
# leptonica
SET (PLUS_leptonica_src_DIR ${CMAKE_BINARY_DIR}/leptonica CACHE INTERNAL "Path to store leptonica contents.")
SET (PLUS_leptonica_DIR "${CMAKE_BINARY_DIR}/leptonica-bin" CACHE INTERNAL "Path to store leptonica binaries")
ExternalProject_Add( leptonica
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_leptonica_src_DIR}"
    BINARY_DIR "${PLUS_leptonica_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/leptonica.git"
    GIT_TAG 45504037248ed25073cd09743d4716205f397288
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS
    )

# --------------------------------------------------------------------------
# tesseract-ocr-cmake
SET (PLUS_tesseract_src_DIR ${CMAKE_BINARY_DIR}/tesseract CACHE INTERNAL "Path to store tesseract contents.")
SET (PLUS_tesseract_DIR "${CMAKE_BINARY_DIR}/tesseract-bin" CACHE INTERNAL "Path to store tesseract binaries")
ExternalProject_Add( tesseract
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_tesseract_src_DIR}"
    BINARY_DIR "${PLUS_tesseract_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/tesseract-ocr-cmake.git"
    GIT_TAG b8082a7879a5d2add333e8ca33c4620168776a70
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DLeptonica_DIR:PATH=${PLUS_leptonica_DIR}
    #--Build step-----------------
    #--Install step-----------------
    DEPENDS ${tesseract_DEPENDENCIES}
    )
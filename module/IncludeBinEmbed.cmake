
#[[
    Binary Embed module

    Defines:
        macro: BinEmbed_AddEmbedding(IDENTIFIER TARGET_FILE)
            args:
                - IDENTIFIER : unique identifier for the embedding, should be a valid c/cxx identifier
                    - used in code generation
                - TARGET_FILE : target file to generate embedding for
                    - absolute or relative to list file that includes this module
            sets:
                - BEMBED_${IDENTIFIER}_SRC : generated sources
                    - contains raw binary data from ${TARGET_FILE}
                - BEMBED_${IDENTIFIER}_INCLUDE_DIRS : include directory, contains generated header
                    - defines a field "extern const unsigned char ${IDENTIFIER}_data[<size>];"
                - BEMBED_${IDENTIFIER}_BUILD : cmake target
                    - should be added as a dependency to anything that uses *_INCLUDE_DIRS or *_SRC
            include header: #include "binembed/${IDENTIFIER}.hxx"
                - contains declaration for the embedded data
]]

cmake_minimum_required(VERSION 3.8)

project(ProjectBinaryEmbedding)

if(NOT TARGET BinEmbedGenerator)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../generator" "${CMAKE_CURRENT_BINARY_DIR}/BinEmbed")
endif()

macro(BinEmbed_AddEmbedding IDENTIFIER TARGET_FILE)
    set(BEMBED_${IDENTIFIER}_DIR "${CMAKE_CURRENT_BINARY_DIR}/binembed.embedding/${IDENTIFIER}.d")
    set(BEMBED_${IDENTIFIER}_INCLUDE_DIRS "${BEMBED_${IDENTIFIER}_DIR}/include")
    set(BEMBED_${IDENTIFIER}_SRC "${BEMBED_${IDENTIFIER}_DIR}/${IDENTIFIER}.cxx")
    set(BEMBED_${IDENTIFIER}_HEADER "${BEMBED_${IDENTIFIER}_INCLUDE_DIRS}/binembed/${IDENTIFIER}.hxx")

    file(MAKE_DIRECTORY "${BEMBED_${IDENTIFIER}_INCLUDE_DIRS}/binembed")

    add_custom_target(
        BEMBED_${IDENTIFIER}_BUILD
        COMMAND "$<TARGET_FILE:BinEmbedGenerator>"
            --target "${TARGET_FILE}"
            --out-src-path "${BEMBED_${IDENTIFIER}_SRC}"
            --out-header-path "${BEMBED_${IDENTIFIER}_HEADER}"
            --identifier "${IDENTIFIER}"
            --data-type "unsigned char"
            --use-c-linkage
        DEPENDS BinEmbedGenerator
        BYPRODUCTS
            "${BEMBED_${IDENTIFIER}_SRC}"
            "${BEMBED_${IDENTIFIER}_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating embedding: ${IDENTIFIER}"
    )

    message(STATUS "Embedding added: ${IDENTIFIER}")
endmacro()

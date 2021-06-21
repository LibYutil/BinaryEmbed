
cmake_minimum_required (VERSION 3.11)

project (ProjectBinaryEmbedGenerator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BINEMBED_TESTS "BINEMBED_TESTS" ON)

include(FetchContent)

FetchContent_Declare(
    cxxcli
    GIT_REPOSITORY https://github.com/LibYutil/CxxCli.git
    UPDATE_DISCONNECTED TRUE
)
FetchContent_GetProperties(cxxcli)
if(NOT cxxcli_POPULATED)
    FetchContent_Populate(cxxcli)
endif()

add_executable(binembed-generator src/main.cxx)
target_include_directories(binembed-generator PRIVATE "${cxxcli_SOURCE_DIR}/include")

#[[
BinEmbed_AddEmbedding(IDENTIFIER TARGET_FILE)
    args:
        - IDENTIFIER : unique identifier for the embedding, should be a valid c/cxx identifier
            - used in code generation
        - TARGET_FILE : target file to generate embedding for
            - absolute or relative to list file that includes this module
    sets:
        - BEMBED_${IDENTIFIER}_SRC : generated sources
            - contains raw binary data from ${TARGET_FILE}
        - BEMBED_${IDENTIFIER}_INCLUDE_DIRS : include directory, contains generated header
        - BEMBED_${IDENTIFIER}_HEADER : the header which is intended to be included
            - defines data field "extern const unsigned char * const ${IDENTIFIER}_data;"
            - defines size field "extern const long long ${IDENTIFIER}_length;"
        - BEMBED_${IDENTIFIER}_BUILD : cmake target
            - should be added as a dependency to anything that uses *_INCLUDE_DIRS or *_SRC
    include header: #include "binembed/${IDENTIFIER}.hxx"
        - contains declaration for the embedded data
]]
macro(BinEmbed_AddEmbedding IDENTIFIER TARGET_FILE)

    # where we store the generated files for this target
    set(BEMBED_${IDENTIFIER}_DIR "${CMAKE_CURRENT_BINARY_DIR}/embedding/${IDENTIFIER}.d")

    # include directories for this target
    # provided as a convention
    set(BEMBED_${IDENTIFIER}_INCLUDE_DIRS "${BEMBED_${IDENTIFIER}_DIR}/include")

    # target header file for this target
    set(BEMBED_${IDENTIFIER}_HEADER "${BEMBED_${IDENTIFIER}_INCLUDE_DIRS}/binembed/${IDENTIFIER}.hxx")

    # sources to build for this target
    set(BEMBED_${IDENTIFIER}_SRC "${BEMBED_${IDENTIFIER}_DIR}/${IDENTIFIER}.cxx")

    # somethng will probably complain if there are no directories
    file(MAKE_DIRECTORY "${BEMBED_${IDENTIFIER}_INCLUDE_DIRS}/binembed")

    get_filename_component(r "${TARGET_FILE}" ABSOLUTE)

    # dependency we should build before building anything that depends on this target
    add_custom_target(
        BEMBED_${IDENTIFIER}_BUILD
        COMMAND "$<TARGET_FILE:binembed-generator>"
            --target "${r}"
            --out-src-path "${BEMBED_${IDENTIFIER}_SRC}"
            --out-header-path "${BEMBED_${IDENTIFIER}_HEADER}"
            --identifier "${IDENTIFIER}"
            --data-type "unsigned char"
            --use-c-linkage
        DEPENDS binembed-generator
        BYPRODUCTS
            "${BEMBED_${IDENTIFIER}_SRC}"
            "${BEMBED_${IDENTIFIER}_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating embedding: ${IDENTIFIER}"
    )

    message(STATUS "Embedding added: ${IDENTIFIER}")
endmacro()

if(BINEMBED_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

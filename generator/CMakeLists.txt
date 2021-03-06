
cmake_minimum_required (VERSION 3.8)

project (ProjectBinaryEmbeddingGenerator)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/CxxCli/CMakeLists.txt")
    find_package(Git REQUIRED)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone "https://github.com/LibYutil/CxxCli.git" "${CMAKE_BINARY_DIR}/CxxCli"
        RESULT_VARIABLE r
    )
    if(NOT ${r} STREQUAL 0)
        message(FATAL_ERROR "failed to retrieve CxxCli")
    endif()
endif()

add_executable(BinEmbedGenerator main.cxx)
target_include_directories(BinEmbedGenerator PRIVATE "${CMAKE_BINARY_DIR}/CxxCli/include")

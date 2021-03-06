
cmake_minimum_required (VERSION 3.8)

project(ProjectBinaryEmbedExample)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../module")

include(IncludeBinEmbed)

BinEmbed_AddEmbedding(some_lib "binary-file")

add_executable(BinEmbed_ex_executable main.cxx ${BEMBED_some_lib_SRC})
target_include_directories(BinEmbed_ex_executable PRIVATE ${BEMBED_some_lib_INCLUDE_DIRS})
add_dependencies(BinEmbed_ex_executable BEMBED_some_lib_BUILD)

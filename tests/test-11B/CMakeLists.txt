
cmake_minimum_required (VERSION 3.0.2)

BinEmbed_AddEmbedding(IDENTIFIER someBinary_file TARGET_FILE "binary-file" APPEND_NULL)

add_executable(be-test-11B test.cxx ${BEMBED_someBinary_file_SRC})
target_include_directories(be-test-11B PRIVATE ${BEMBED_someBinary_file_INCLUDE_DIRS})

add_test(NAME test-be-test-11B COMMAND be-test-11B)

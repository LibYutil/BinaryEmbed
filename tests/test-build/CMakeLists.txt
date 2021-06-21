
cmake_minimum_required (VERSION 3.0.2)

BinEmbed_AddEmbedding(someBinary_file "binary-file")

add_executable(be-test-build main.cxx ${BEMBED_someBinary_file_SRC})
target_include_directories(be-test-build PRIVATE ${BEMBED_someBinary_file_INCLUDE_DIRS})
add_dependencies(be-test-build BEMBED_someBinary_file_BUILD)

add_test(
	NAME test-be-test-build
	COMMAND be-test-build
)

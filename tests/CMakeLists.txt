
cmake_minimum_required (VERSION 3.0)

project (ProjectBinaryEmbedGeneratorTests)

add_executable(
	be-util-rgen
	EXCLUDE_FROM_ALL
	util-rand-generator.cxx
)
target_include_directories(be-util-rgen PRIVATE "${cxxcli_SOURCE_DIR}/include")

add_subdirectory(test-11B)

macro(binembed_add_test byteSize)
	set(tdid testData_${byteSize}B)

	set(generated_file "${CMAKE_CURRENT_BINARY_DIR}/${tdid}")
	add_custom_command(
		OUTPUT "${generated_file}"
		COMMAND be-util-rgen ${byteSize} "${generated_file}"
		DEPENDS be-util-rgen
	)

	BinEmbed_AddEmbedding(IDENTIFIER ${tdid} TARGET_FILE "${generated_file}")

	set(tid be-test-${byteSize}B)
	set(testFileHeader "${BEMBED_${tdid}_HEADER}")
	set(testFilePath "${generated_file}")
	set(testFileDataIdentifier "${tdid}_data")
	set(testFileLengthIdentifier "${tdid}_length")
	configure_file(test.cxx.in "${CMAKE_CURRENT_BINARY_DIR}/test-${byteSize}B.cxx")
	add_executable(be-test-${byteSize}B "${CMAKE_CURRENT_BINARY_DIR}/test-${byteSize}B.cxx" "${BEMBED_${tdid}_SRC}")
	target_include_directories(be-test-${byteSize}B PRIVATE ${BEMBED_${tdid}_INCLUDE_DIRS})

	add_test(NAME test-be-test-${byteSize}B COMMAND be-test-${byteSize}B)
endmacro()

binembed_add_test(100)
binembed_add_test(256)
binembed_add_test(257)
binembed_add_test(1000)
binembed_add_test(2753)

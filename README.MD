# BinaryEmbed

## What is this?
A binary embedding generator.
File in, c/cxx embedding source out.

Generates c/cxx source containing raw binary data of a target file in an `unsigned char` array.

Tested on MSVC 19

## Usage

### Code
See example

### CMake
- Add `BinaryEmbed/module` path to cmake module path
  - `LIST(APPEND CMAKE_MODULE_PATH <BinaryEmbed/module path>)`
- Include BinaryEmbed module
  - `include(IncludeBinEmbed)`
- Add embedding (documentated in module/IncludeBinEmbed.cmake)
  - `BinEmbed_AddEmbedding(some_identifier "./path/to/target.file")`
- Add embedding sources to some dependent library/executable
  - `target_sources(<target> PRIVATE ${BEMBED_some_identifier_SRC})`
- Add embedding headers
  - `target_include_directories(<target> PRIVATE ${BEMBED_some_identifier_INCLUDE_DIRS})`
- Add embedding dependency
  - `add_dependencies(<target> BEMBED_some_lib_BUILD)`
- `#include "binembed/some_identifier.hxx"`

## ToDo
+ example source documentation
+ generator source & cli documentation

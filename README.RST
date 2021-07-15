
BinaryEmbed
===========

What is this?
-------------
- A binary embedding generator
- Binary file in, c/cxx embedding source out
- Generates c/cxx source containing raw binary data of a target file in an ``unsigned char`` array
- C++11 compilant

CMake
-----
#. Start with adding this project into build

   - ``add_subdirectory(./path/to/BinaryEmbed)``

#. Add embedding

   - ``BinEmbed_AddEmbedding(${identifier} "./path/to/target.file")``

#. Add embedding sources to some dependent library/executable

   - ``target_sources(<target> PRIVATE ${BEMBED_${identifier}_SRC})``

#. Add embedding headers

   - ``target_include_directories(<target> PRIVATE ${BEMBED_${identifier}_INCLUDE_DIRS})``

#. Add embedding dependency [optional]

   - ``add_dependencies(<target> BEMBED_${identifier}_BUILD)``
   - only needed in some cases when some target does not depend on neither ``*_SRC`` or ``*_HEADER`` but requires any of them

#. ``#include "binembed/${identifier}.hxx"``

What really happens
*******************
- the project only defines a generator tool (``binembed-generator`` target), which is a cli tool that generates source & header pairs for a given file
- ``BinEmbed_AddEmbedding`` macro uses that tool to create a build target which invokes it to produce the source & header files for a given file
- besides that the macro also sets all necessary variables such as ``*_INCLUDE_DIRS`` and ``*_SRC``
- top level CMakeLists.txt provides a more detailed documentation (and definition) for the macro

CLI
---
Build generator tool: ``mkdir _build && cd _build && cmake .. && cmake --build .``

Command help available with ``binembed-generator --help``

Usage
-----
See tests

ToDo
----
- example source documentation
- generator source & cli documentation

Tested Compilers
----------------
- msvc 19
- g++ 8.3.0

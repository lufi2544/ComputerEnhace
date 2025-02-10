cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(JsonParser VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add platform-specific flags for Debug mode
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")  # Generates PDB files on Windows
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")  # Include PDB during linking
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")  # Generate debug info on macOS/Linux
endif()

# Use file(GLOB ...) to include all .cpp files in the source directory
file(GLOB_RECURSE SOURCES
    "source/*.cpp"
    "source/*.h"
    "../profiling/source/*.h"
    "../profiling/source/*.cpp"
)

# Add the executable with all gathered source files
add_executable(JsonParser ${SOURCES})

# Optionally add include directories
target_include_directories(JsonParser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
	${CMAKE_CURRENT_SOURCE_DIR}/../profiling/source
)

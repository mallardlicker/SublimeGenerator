# SublimeGenerator

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	SublimeGenerator Project
# ==================================================================================================
project(
    SublimeGenerator
    VERSION 0.0.1
    DESCRIPTION "Description here."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generates compile_commands.json, which VSCode uses to determine resolved dependencies in-editor

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# ==================================================================================================
#	Add each folder to project and let their CMakeLists generate separate code
# ==================================================================================================

# Add SublimeGenerator executable directory
add_subdirectory(SublimeGenerator)
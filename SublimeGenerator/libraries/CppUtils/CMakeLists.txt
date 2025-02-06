# CppUtils

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	CppUtils Project
# ==================================================================================================
project(
    CppUtils
    VERSION 0.0.1
    DESCRIPTION "Description here."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# ==================================================================================================
#	Add each folder to project and let their CMakeLists generate separate code
# ==================================================================================================

# CppUtils library
add_subdirectory(CppUtils)

# sgName

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	sgName Project
# ==================================================================================================
project(
    sgName
    VERSION 0.0.1
    DESCRIPTION "Description here."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# ==================================================================================================
#   Collect sgName project files
# ==================================================================================================

file(   GLOB_RECURSE sgName_SOURCE_FILES
    ${sgName_SOURCE_DIR}/cpp/*.cpp
)

file(   GLOB_RECURSE sgName_HEADER_FILES
    ${sgName_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(sgName_INCLUDE_DIRS
    "${sgName_SOURCE_DIR}/include"
    # "${sgName_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Create generated executable
# ==================================================================================================

add_executable(sgExe 
               ${sgName_SOURCE_FILES} 
               ${sgName_HEADER_FILES})

target_include_directories(sgExe PRIVATE ${sgName_INCLUDE_DIRS})
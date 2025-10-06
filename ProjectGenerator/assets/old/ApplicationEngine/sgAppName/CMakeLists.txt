# sgName/sgAppName

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	SGAPPNAME Project
# ==================================================================================================
project(
    SGAPPNAME
    VERSION 0.0.1
    DESCRIPTION "Description Here Please."
    LANGUAGES CXX
)


# ==================================================================================================
#   Collect SGAPPNAME project files
# ==================================================================================================

file(	GLOB_RECURSE SGAPPNAME_SOURCE_FILES
	${SGAPPNAME_SOURCE_DIR}/cpp/*.cpp
)

file(	GLOB_RECURSE SGAPPNAME_HEADER_FILES
    ${SGAPPNAME_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(sgAppName_INCLUDE_DIRS
    "${SGAPPNAME_SOURCE_DIR}/include"
    # "${SGAPPNAME_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Create and link generated executable
# ==================================================================================================

add_executable(sgExe 
               ${SGAPPNAME_SOURCE_FILES} 
               ${SGAPPNAME_HEADER_FILES})

target_include_directories(sgExe PRIVATE ${sgAppName_INCLUDE_DIRS})
target_link_libraries(sgExe PRIVATE sgLibName)
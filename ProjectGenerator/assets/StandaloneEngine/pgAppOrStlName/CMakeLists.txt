# pgName/pgAppOrStlName

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	PGAPPORSTLNAME Project
# ==================================================================================================
project(
    PGAPPORSTLNAME
    VERSION 0.0.1
    DESCRIPTION "Description Here Please."
    LANGUAGES CXX
)


# ==================================================================================================
#   Collect PGAPPORSTLNAME project files
# ==================================================================================================

file(	GLOB_RECURSE PGAPPORSTLNAME_SOURCE_FILES
	${PGAPPORSTLNAME_SOURCE_DIR}/cpp/*.cpp
)

file(	GLOB_RECURSE PGAPPORSTLNAME_HEADER_FILES
    ${PGAPPORSTLNAME_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(pgAppOrStlName_INCLUDE_DIRS
    "${PGAPPORSTLNAME_SOURCE_DIR}/include"
    # "${PGAPPORSTLNAME_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Create and link generated executable
# ==================================================================================================

add_executable(pgAppOrStl 
               ${PGAPPORSTLNAME_SOURCE_FILES} 
               ${PGAPPORSTLNAME_HEADER_FILES})

target_include_directories(pgAppOrStl PRIVATE ${pgAppOrStlName_INCLUDE_DIRS})
target_link_libraries(pgAppOrStl PRIVATE pgLibName)

target_compile_definitions(pgAppOrStl PRIVATE 
    $<$<CONFIG:Debug>:DEBUG_BUILD=1>
    $<$<CONFIG:Debug>:RELEASE_BUILD=0>
    $<$<CONFIG:Release>:DEBUG_BUILD=0>
    $<$<CONFIG:Release>:RELEASE_BUILD=1>
)
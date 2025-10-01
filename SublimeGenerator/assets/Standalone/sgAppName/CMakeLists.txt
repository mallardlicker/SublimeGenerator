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

# add function(s) to project
include(FetchContent)


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
#   Collect SGAPPNAME external library files
# ==================================================================================================

#   non-downloaded (FetchContent calls)

# GLFW (option 1)
# FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(glfw)


#   downloaded (physical libraries)

# call find_package for locally (user) installed packages like glfw or OpenGL
# GLFW (option 2)
# find_package(glfw3 3.3 REQUIRED)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${SGAPPNAME_SOURCE_DIR}/libraries/entt/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB SGAPPNAME_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${SGAPPNAME_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${SGAPPNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${SGAPPNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE SGAPPNAME_LIB_HEADER_FILES
    
    # eg:
    # ${SGAPPNAME_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(sgAppName_LINKED_LIBS 
    # glfw
    # EnTT::EnTT
    # "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" # frameworks for ImGui
)


# ==================================================================================================
#   Create and link generated executable
# ==================================================================================================

add_executable(sgExe 
               ${SGAPPNAME_SOURCE_FILES} 
               ${SGAPPNAME_HEADER_FILES}
               ${SGAPPNAME_LIB_SOURCE_FILES}
               ${SGAPPNAME_LIB_HEADER_FILES})

target_include_directories(sgExe PRIVATE ${sgAppName_INCLUDE_DIRS})
target_link_libraries(sgExe PRIVATE ${sgAppName_LINKED_LIBS})
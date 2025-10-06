# ProjectGenerator/ProjectGenerator

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	PROJECTGENERATOR Project
# ==================================================================================================
project(
    PROJECTGENERATOR
    VERSION 0.1.0
    DESCRIPTION "Description Here Please."
    LANGUAGES CXX
)

# add function(s) to project
include(FetchContent)


# ==================================================================================================
#   Collect ProjectGenerator project files
# ==================================================================================================

file(   GLOB_RECURSE PROJECTGENERATOR_SOURCE_FILES
    ${PROJECTGENERATOR_SOURCE_DIR}/cpp/*.cpp
)

file(   GLOB_RECURSE PROJECTGENERATOR_HEADER_FILES
    ${PROJECTGENERATOR_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(ProjectGenerator_INCLUDE_DIRS
    "${PROJECTGENERATOR_SOURCE_DIR}/include"
    # "${PROJECTGENERATOR_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Collect ProjectGenerator external library files
# ==================================================================================================

#   non-downloaded (FetchContent or find_package calls)

# CppUtils
FetchContent_Declare(CppUtils GIT_REPOSITORY https://github.com/mallardlicker/CppUtils.git GIT_TAG main)
FetchContent_MakeAvailable(CppUtils)

FetchContent_Declare(
	nlohmann_json 
    GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG v3.11.3 # or whatever the latest stable tag is
)
FetchContent_MakeAvailable(nlohmann_json)


#   downloaded (physical libraries)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${PROJECTGENERATOR_SOURCE_DIR}/libraries/entt/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB PROJECTGENERATOR_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${PROJECTGENERATOR_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${PROJECTGENERATOR_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${PROJECTGENERATOR_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE PROJECTGENERATOR_LIB_HEADER_FILES
    
    # eg:
    # ${PROJECTGENERATOR_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(ProjectGenerator_LINKED_LIBS
    CppUtils
    nlohmann_json::nlohmann_json
)


# ==================================================================================================
#   Create and link generated executable
# ==================================================================================================

add_executable(pg 
               ${PROJECTGENERATOR_SOURCE_FILES} 
               ${PROJECTGENERATOR_HEADER_FILES}
               ${PROJECTGENERATOR_LIB_SOURCE_FILES}
               ${PROJECTGENERATOR_LIB_HEADER_FILES})


target_include_directories(pg PRIVATE ${ProjectGenerator_INCLUDE_DIRS})
target_link_libraries(pg PRIVATE ${ProjectGenerator_LINKED_LIBS})

target_compile_definitions(pg PRIVATE 
    $<$<CONFIG:Debug>:DEBUG_BUILD=1>
    $<$<CONFIG:Debug>:RELEASE_BUILD=0>
    $<$<CONFIG:Release>:DEBUG_BUILD=0>
    $<$<CONFIG:Release>:RELEASE_BUILD=1>
)


# ==================================================================================================
#   Install configuration for Release builds
# ==================================================================================================

# Set default install prefix to /usr/local if not specified
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install prefix" FORCE)
endif()

# Install the executable to /usr/local/bin
install(TARGETS pg
		RUNTIME DESTINATION bin
		COMPONENT pg_only
		EXCLUDE_FROM_ALL)
		
# Install to workspace directory (uses different prefix)
# install(TARGETS pg
# 		RUNTIME DESTINATION .
# 		COMPONENT pg_workspace
# 		EXCLUDE_FROM_ALL)

# Create a custom target to install only pg to system location
add_custom_target(install_pg_system
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --component pg_only
    DEPENDS pg
    COMMENT "Installing pg executable to system bin")

# Create a custom target to install pg to workspace directory
# add_custom_target(install_pg_workspace
# 	COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --component pg_workspace --prefix ${CMAKE_SOURCE_DIR}
# 	DEPENDS pg
# 	COMMENT "Installing pg executable to workspace directory")
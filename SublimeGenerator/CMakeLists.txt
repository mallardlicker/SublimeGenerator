# SublimeGenerator/SublimeGenerator

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	SUBLIMEGENERATOR Project
# ==================================================================================================
project(
    SUBLIMEGENERATOR
    VERSION 0.0.1
    DESCRIPTION "Description Here Please."
    LANGUAGES CXX
)

# add function(s) to project
include(FetchContent)


# ==================================================================================================
#   Collect SublimeGenerator project files
# ==================================================================================================

file(   GLOB_RECURSE SUBLIMEGENERATOR_SOURCE_FILES
    ${SUBLIMEGENERATOR_SOURCE_DIR}/cpp/*.cpp
)

file(   GLOB_RECURSE SUBLIMEGENERATOR_HEADER_FILES
    ${SUBLIMEGENERATOR_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(SublimeGenerator_INCLUDE_DIRS
    "${SUBLIMEGENERATOR_SOURCE_DIR}/include"
    # "${SUBLIMEGENERATOR_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Collect TESTPRJ external library files
# ==================================================================================================

#   non-downloaded (FetchContent or find_package calls)

# CppUtils
FetchContent_Declare(CppUtils GIT_REPOSITORY https://github.com/mallardlicker/CppUtils.git GIT_TAG main)
FetchContent_MakeAvailable(CppUtils)


#   downloaded (physical libraries)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${TESTPRJ_SOURCE_DIR}/libraries/entt/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB TESTPRJ_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${TESTPRJ_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${TESTPRJ_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${TESTPRJ_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE TESTPRJ_LIB_HEADER_FILES
    
    # eg:
    # ${TESTPRJ_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(TestPrj_LINKED_LIBS 
    CppUtils
)


# ==================================================================================================
#   Create and link generated executable
# ==================================================================================================

add_executable(sg 
               ${SUBLIMEGENERATOR_SOURCE_FILES} 
               ${SUBLIMEGENERATOR_HEADER_FILES}
               ${SUBLIMEGENERATOR_LIB_SOURCE_FILES}
               ${SUBLIMEGENERATOR_LIB_HEADER_FILES})

target_include_directories(sg PRIVATE ${TestPrj_INCLUDE_DIRS})
target_link_libraries(sg PRIVATE ${TestPrj_LINKED_LIBS})

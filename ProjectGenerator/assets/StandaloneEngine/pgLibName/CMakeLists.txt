# pgName/pgLibName

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	PGLIBNAME Project
# ==================================================================================================
project(
    PGLIBNAME
    VERSION 0.0.1
    DESCRIPTION "Description here."
    LANGUAGES CXX
)

# settings
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(PGLIBNAME_INSTALL "Generate installation target" ON)

set(PGLIBNAME_LIBRARY_TYPE "${PGLIBNAME_LIBRARY_TYPE}" CACHE STRING
    "Library type override for PGLIBNAME (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

if (PGLIBNAME_LIBRARY_TYPE)
    if (PGLIBNAME_LIBRARY_TYPE STREQUAL "SHARED")
        set(PGLIBNAME_BUILD_SHARED_LIBRARY TRUE)
    else()
        set(PGLIBNAME_BUILD_SHARED_LIBRARY FALSE)
    endif()
else()
    set(PGLIBNAME_BUILD_SHARED_LIBRARY ${BUILD_SHARED_LIBS})
endif()

# add function(s) to project
include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)


# ==================================================================================================
#   Collect PGLIBNAME internal library files
# ==================================================================================================

file(   GLOB_RECURSE PGLIBNAME_SOURCE_FILES
    ${PGLIBNAME_SOURCE_DIR}/cpp/*.cpp
)

file(   GLOB_RECURSE PGLIBNAME_HEADER_FILES
    ${PGLIBNAME_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(pgLibName_INCLUDE_DIRS
    "${PGLIBNAME_SOURCE_DIR}/include"
    # "${PGLIBNAME_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Collect PGLIBNAME external library files
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
# WARNING: everything added using find_package MUST be added to the CMake/pgLibNameConfig.cmake.in
#  file's find_dependency list
# GLFW (option 2)
# find_package(glfw3 3.3 REQUIRED)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${PGLIBNAME_SOURCE_DIR}/libraries/entt/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB PGLIBNAME_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${PGLIBNAME_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${PGLIBNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${PGLIBNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE PGLIBNAME_LIB_HEADER_FILES
    
    # eg:
    # ${PGLIBNAME_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(pgLibName_LINKED_LIBS 
    # glfw
    # EnTT::EnTT
    # "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" # frameworks for ImGui
)


# ===================================================================================
#   Create additional generated files (that allow custom library)
# ===================================================================================
# include(CMakePackageConfigHelpers) --> in project def

set(PGLIBNAME_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/pgLibName")

configure_package_config_file(CMake/pgLibNameConfig.cmake.in
                              src/pgLibNameConfig.cmake
                              INSTALL_DESTINATION "${PGLIBNAME_CONFIG_PATH}"
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(src/pgLibNameConfigVersion.cmake
                                 VERSION ${PGLIBNAME_VERSION}
                                 COMPATIBILITY SameMajorVersion)


# ===================================================================================
#   Create generated library
# ===================================================================================

add_library(pgLibName ${PGLIBNAME_LIBRARY_TYPE}
                ${PGLIBNAME_SOURCE_FILES}
                ${PGLIBNAME_HEADER_FILES}
                ${PGLIBNAME_LIB_SOURCE_FILES}
                ${PGLIBNAME_LIB_HEADER_FILES}
)

set(PGLIBNAME_LIB_NAME pgLibName)
set(PGLIBNAME_LIB_NAME_SUFFIX "")

set_target_properties(pgLibName PROPERTIES
                      OUTPUT_NAME ${PGLIBNAME_LIB_NAME}
                      VERSION ${PGLIBNAME_VERSION_MAJOR}.${PGLIBNAME_VERSION_MINOR}
                      SOVERSION ${PGLIBNAME_VERSION_MAJOR}
                      POSITION_INDEPENDENT_CODE ON
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      DEFINE_SYMBOL _PGLIBNAME_BUILD_DLL
                      FOLDER "PGLIBNAME")


# ===================================================================================
#   Relist library include folders (for installation) and link
# ===================================================================================

target_include_directories(pgLibName PUBLIC
                            # this must contain all directories listed in pgLibName_INCLUDE_DIRS
                            "$<BUILD_INTERFACE:${PGLIBNAME_SOURCE_DIR}/include>"
                            # "$<BUILD_INTERFACE:${PGLIBNAME_SOURCE_DIR}/include/core>"
                            
                            # this must contain all directories listed in PGLIBNAME_LIB_SOURCE_FILES or PGLIBNAME_LIB_HEADER_FILES
                            # "$<BUILD_INTERFACE:${PGLIBNAME_SOURCE_DIR}/libraries/imgui>"
                            # "$<BUILD_INTERFACE:${PGLIBNAME_SOURCE_DIR}/libraries/imgui/backends>"
                            
                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_include_directories(pgLibName PRIVATE
                            # this must (again) contain all directories listed in pgLibName_INCLUDE_DIRS
                            "${PGLIBNAME_SOURCE_DIR}/include"
                            "${PGLIBNAME_BINARY_DIR}/include"
                            # "${PGLIBNAME_SOURCE_DIR}/include/core"
                            # "${PGLIBNAME_BINARY_DIR}/include/core"
                            
                            # this must (again) contain all directories listed in PGLIBNAME_LIB_SOURCE_FILES or PGLIBNAME_LIB_HEADER_FILES
                            # "${PGLIBNAME_SOURCE_DIR}/libraries/imgui"
                            # "${PGLIBNAME_BINARY_DIR}/libraries/imgui"
                            # "${PGLIBNAME_SOURCE_DIR}/libraries/imgui/backends"
                            # "${PGLIBNAME_BINARY_DIR}/libraries/imgui/backends"
)

target_link_libraries(pgLibName PUBLIC ${pgLibName_LINKED_LIBS})

target_compile_definitions(pgLibName PRIVATE 
    $<$<CONFIG:Debug>:DEBUG_BUILD=1>
    $<$<CONFIG:Debug>:RELEASE_BUILD=0>
    $<$<CONFIG:Release>:DEBUG_BUILD=0>
    $<$<CONFIG:Release>:RELEASE_BUILD=1>
)


# ===================================================================================
#   Generate and install library
# ===================================================================================

# if (PGLIBNAME_BUILD_SHARED_LIBRARY)
#   if (UNIX)
#       # Hide symbols not explicitly tagged for export from the shared library
#         target_compile_options(pgLibName PRIVATE "-fvisibility=hidden")
#   endif()
# endif()

foreach(arg ${pgLibName_PKG_DEPS})
    string(APPEND deps " ${arg}")
endforeach()
foreach(arg ${pgLibName_PKG_LIBS})
    string(APPEND libs " ${arg}")
endforeach()

set(PGLIBNAME_PKG_CONFIG_REQUIRES_PRIVATE "${deps}" CACHE INTERNAL
    "PGLIBNAME pkg-config Requires.private")
set(PGLIBNAME_PKG_CONFIG_LIBS_PRIVATE "${libs}" CACHE INTERNAL
    "PGLIBNAME pkg-config Libs.private")

configure_file("${PGLIBNAME_SOURCE_DIR}/CMake/pgLibName.pc.in" pgLibName.pc @ONLY)

if (PGLIBNAME_INSTALL)
    install(TARGETS pgLibName
            EXPORT pgLibNameTargets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

# ===================================================================================
#   Install files other than the library
# ===================================================================================
    
    install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.h") # install all header files

    install(FILES "${PGLIBNAME_BINARY_DIR}/src/pgLibNameConfig.cmake"
                  "${PGLIBNAME_BINARY_DIR}/src/pgLibNameConfigVersion.cmake"
            DESTINATION "${PGLIBNAME_CONFIG_PATH}")

    install(EXPORT pgLibNameTargets FILE pgLibNameTargets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
            DESTINATION "${PGLIBNAME_CONFIG_PATH}")
    install(FILES "${PGLIBNAME_BINARY_DIR}/src/pgLibName.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

endif()

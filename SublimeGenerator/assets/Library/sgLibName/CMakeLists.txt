# sgName/sgLibName

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	SGLIBNAME Project
# ==================================================================================================
project(
    SGLIBNAME
    VERSION 0.0.1
    DESCRIPTION "Description here."
    LANGUAGES CXX
)

# settings
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(SGLIBNAME_INSTALL "Generate installation target" ON)

set(SGLIBNAME_LIBRARY_TYPE "${SGLIBNAME_LIBRARY_TYPE}" CACHE STRING
    "Library type override for SGLIBNAME (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

if (SGLIBNAME_LIBRARY_TYPE)
    if (SGLIBNAME_LIBRARY_TYPE STREQUAL "SHARED")
        set(SGLIBNAME_BUILD_SHARED_LIBRARY TRUE)
    else()
        set(SGLIBNAME_BUILD_SHARED_LIBRARY FALSE)
    endif()
else()
    set(SGLIBNAME_BUILD_SHARED_LIBRARY ${BUILD_SHARED_LIBS})
endif()

# add function(s) to project
include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)


# ==================================================================================================
#   Collect SGLIBNAME internal library files
# ==================================================================================================

file(   GLOB_RECURSE SGLIBNAME_SOURCE_FILES
    ${SGLIBNAME_SOURCE_DIR}/cpp/*.cpp
)

file(   GLOB_RECURSE SGLIBNAME_HEADER_FILES
    ${SGLIBNAME_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(sgLibName_INCLUDE_DIRS
    "${SGLIBNAME_SOURCE_DIR}/include"
    # "${SGLIBNAME_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Collect SGLIBNAME external library files
# ==================================================================================================

#   non-downloaded (FetchContent or find_package calls)

# GLFW (option 1)
# FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(glfw)

# WARNING: everything added using find_package MUST be added to the CMake/sgLibNameConfig.cmake.in
#  file's find_dependency list
# GLFW (option 2)
# find_package(glfw3 3.3 REQUIRED)


#   downloaded (physical libraries)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${SGLIBNAME_SOURCE_DIR}/libraries/entt/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB SGLIBNAME_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${SGLIBNAME_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${SGLIBNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${SGLIBNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE SGLIBNAME_LIB_HEADER_FILES
    
    # eg:
    # ${SGLIBNAME_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(sgLibName_LINKED_LIBS 
    # glfw
    # EnTT::EnTT
    # "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" # frameworks for ImGui
)


# ===================================================================================
#   Create additional generated files (that allow custom library)
# ===================================================================================
# include(CMakePackageConfigHelpers) --> in project def

set(SGLIBNAME_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/sgLibName")

configure_package_config_file(CMake/sgLibNameConfig.cmake.in
                              src/sgLibNameConfig.cmake
                              INSTALL_DESTINATION "${SGLIBNAME_CONFIG_PATH}"
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(src/sgLibNameConfigVersion.cmake
                                 VERSION ${SGLIBNAME_VERSION}
                                 COMPATIBILITY SameMajorVersion)


# ===================================================================================
#   Create generated library
# ===================================================================================

add_library(sgLibName ${SGLIBNAME_LIBRARY_TYPE}
                ${SGLIBNAME_SOURCE_FILES}
                ${SGLIBNAME_HEADER_FILES}
                ${SGLIBNAME_LIB_SOURCE_FILES}
                ${SGLIBNAME_LIB_HEADER_FILES}
)

set(SGLIBNAME_LIB_NAME sgLibName)
set(SGLIBNAME_LIB_NAME_SUFFIX "")

set_target_properties(sgLibName PROPERTIES
                      OUTPUT_NAME ${SGLIBNAME_LIB_NAME}
                      VERSION ${SGLIBNAME_VERSION_MAJOR}.${SGLIBNAME_VERSION_MINOR}
                      SOVERSION ${SGLIBNAME_VERSION_MAJOR}
                      POSITION_INDEPENDENT_CODE ON
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      DEFINE_SYMBOL _SGLIBNAME_BUILD_DLL
                      FOLDER "SGLIBNAME")


# ===================================================================================
#   Relist library include folders (for installation) and link
# ===================================================================================

target_include_directories(sgLibName PUBLIC
                            # this must contain all directories listed in sgLibName_INCLUDE_DIRS
                            "$<BUILD_INTERFACE:${SGLIBNAME_SOURCE_DIR}/include>"
                            # "$<BUILD_INTERFACE:${SGLIBNAME_SOURCE_DIR}/include/core>"
                            
                            # this must contain all directories listed in SGLIBNAME_LIB_SOURCE_FILES or SGLIBNAME_LIB_HEADER_FILES
                            # "$<BUILD_INTERFACE:${SGLIBNAME_SOURCE_DIR}/libraries/imgui>"
                            # "$<BUILD_INTERFACE:${SGLIBNAME_SOURCE_DIR}/libraries/imgui/backends>"
                            
                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_include_directories(sgLibName PRIVATE
                            # this must (again) contain all directories listed in sgLibName_INCLUDE_DIRS
                            "${SGLIBNAME_SOURCE_DIR}/include"
                            "${SGLIBNAME_BINARY_DIR}/include"
                            # "${SGLIBNAME_SOURCE_DIR}/include/core"
                            # "${SGLIBNAME_BINARY_DIR}/include/core"
                            
                            # this must (again) contain all directories listed in SGLIBNAME_LIB_SOURCE_FILES or SGLIBNAME_LIB_HEADER_FILES
                            # "${SGLIBNAME_SOURCE_DIR}/libraries/imgui"
                            # "${SGLIBNAME_BINARY_DIR}/libraries/imgui"
                            # "${SGLIBNAME_SOURCE_DIR}/libraries/imgui/backends"
                            # "${SGLIBNAME_BINARY_DIR}/libraries/imgui/backends"
)

target_link_libraries(sgLibName PUBLIC ${sgLibName_LINKED_LIBS})


# ===================================================================================
#   Generate and install library
# ===================================================================================

# if (SGLIBNAME_BUILD_SHARED_LIBRARY)
#   if (UNIX)
#       # Hide symbols not explicitly tagged for export from the shared library
#         target_compile_options(sgLibName PRIVATE "-fvisibility=hidden")
#   endif()
# endif()

foreach(arg ${sgLibName_PKG_DEPS})
    string(APPEND deps " ${arg}")
endforeach()
foreach(arg ${sgLibName_PKG_LIBS})
    string(APPEND libs " ${arg}")
endforeach()

set(SGLIBNAME_PKG_CONFIG_REQUIRES_PRIVATE "${deps}" CACHE INTERNAL
    "SGLIBNAME pkg-config Requires.private")
set(SGLIBNAME_PKG_CONFIG_LIBS_PRIVATE "${libs}" CACHE INTERNAL
    "SGLIBNAME pkg-config Libs.private")

configure_file("${SGLIBNAME_SOURCE_DIR}/CMake/sgLibName.pc.in" sgLibName.pc @ONLY)

if (SGLIBNAME_INSTALL)
    install(TARGETS sgLibName
            EXPORT sgLibNameTargets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

# ===================================================================================
#   Install files other than the library
# ===================================================================================
    
    install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.h") # install all header files

    install(FILES "${SGLIBNAME_BINARY_DIR}/src/sgLibNameConfig.cmake"
                  "${SGLIBNAME_BINARY_DIR}/src/sgLibNameConfigVersion.cmake"
            DESTINATION "${SGLIBNAME_CONFIG_PATH}")

    install(EXPORT sgLibNameTargets FILE sgLibNameTargets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
            DESTINATION "${SGLIBNAME_CONFIG_PATH}")
    install(FILES "${SGLIBNAME_BINARY_DIR}/src/sgLibName.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

endif()
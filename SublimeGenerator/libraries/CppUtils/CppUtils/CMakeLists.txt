# CppUtils/CppUtils

cmake_minimum_required(VERSION 3.22)

# ==================================================================================================
#	CPPUTILS Project
# ==================================================================================================
project(
    CPPUTILS
    VERSION 0.0.1
    DESCRIPTION "Description here."
    LANGUAGES CXX
)

# settings
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(CPPUTILS_INSTALL "Generate installation target" ON)

set(CPPUTILS_LIBRARY_TYPE "${CPPUTILS_LIBRARY_TYPE}" CACHE STRING
    "Library type override for CPPUTILS (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

if (CPPUTILS_LIBRARY_TYPE)
    if (CPPUTILS_LIBRARY_TYPE STREQUAL "SHARED")
        set(CPPUTILS_BUILD_SHARED_LIBRARY TRUE)
    else()
        set(CPPUTILS_BUILD_SHARED_LIBRARY FALSE)
    endif()
else()
    set(CPPUTILS_BUILD_SHARED_LIBRARY ${BUILD_SHARED_LIBS})
endif()

# add function(s) to project
include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)


# ==================================================================================================
#   Collect CPPUTILS internal library files
# ==================================================================================================

file(   GLOB_RECURSE CPPUTILS_SOURCE_FILES
    ${CPPUTILS_SOURCE_DIR}/cpp/*.cpp
)

file(   GLOB_RECURSE CPPUTILS_HEADER_FILES
    ${CPPUTILS_SOURCE_DIR}/include/*.h
)

# must update the include directories manually so far
set(CppUtils_INCLUDE_DIRS
    "${CPPUTILS_SOURCE_DIR}/include"
    # "${CPPUTILS_SOURCE_DIR}/include/core"
)


# ==================================================================================================
#   Collect CPPUTILS external library files
# ==================================================================================================

#   non-downloaded (FetchContent or find_package calls)

# GLFW (option 1)
# FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(glfw)

# WARNING: everything added using find_package MUST be added to the CMake/CppUtilsConfig.cmake.in
#  file's find_dependency list
# GLFW (option 2)
# find_package(glfw3 3.3 REQUIRED)


#   downloaded (physical libraries)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${CPPUTILS_SOURCE_DIR}/libraries/entt/)

# NFD (native file dialog)
add_subdirectory(${CPPUTILS_SOURCE_DIR}/libraries/nativefiledialog-extended/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB CPPUTILS_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${CPPUTILS_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${CPPUTILS_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${CPPUTILS_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE CPPUTILS_LIB_HEADER_FILES
    
    # eg:
    # ${CPPUTILS_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(CppUtils_LINKED_LIBS 
    # glfw
    # EnTT::EnTT
    # "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" # frameworks for ImGui
    nfd "-framework AppKit" "-framework UniformTypeIdentifiers" # frameworks for NFD
)


# ===================================================================================
#   Create additional generated files (that allow custom library)
# ===================================================================================
# include(CMakePackageConfigHelpers) --> in project def

set(CPPUTILS_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/CppUtils")

configure_package_config_file(CMake/CppUtilsConfig.cmake.in
                              src/CppUtilsConfig.cmake
                              INSTALL_DESTINATION "${CPPUTILS_CONFIG_PATH}"
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(src/CppUtilsConfigVersion.cmake
                                 VERSION ${CPPUTILS_VERSION}
                                 COMPATIBILITY SameMajorVersion)


# ===================================================================================
#   Create generated library
# ===================================================================================

add_library(CppUtils ${CPPUTILS_LIBRARY_TYPE}
                ${CPPUTILS_SOURCE_FILES}
                ${CPPUTILS_HEADER_FILES}
                ${CPPUTILS_LIB_SOURCE_FILES}
                ${CPPUTILS_LIB_HEADER_FILES}
)

set(CPPUTILS_LIB_NAME CppUtils)
set(CPPUTILS_LIB_NAME_SUFFIX "")

set_target_properties(CppUtils PROPERTIES
                      OUTPUT_NAME ${CPPUTILS_LIB_NAME}
                      VERSION ${CPPUTILS_VERSION_MAJOR}.${CPPUTILS_VERSION_MINOR}
                      SOVERSION ${CPPUTILS_VERSION_MAJOR}
                      POSITION_INDEPENDENT_CODE ON
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      DEFINE_SYMBOL _CPPUTILS_BUILD_DLL
                      FOLDER "CPPUTILS")


# ===================================================================================
#   Relist library include folders (for installation) and link
# ===================================================================================

target_include_directories(CppUtils PUBLIC
                            # this must contain all directories listed in CppUtils_INCLUDE_DIRS
                            "$<BUILD_INTERFACE:${CPPUTILS_SOURCE_DIR}/include>"
                            # "$<BUILD_INTERFACE:${CPPUTILS_SOURCE_DIR}/include/core>"
                            
                            # this must contain all directories listed in CPPUTILS_LIB_SOURCE_FILES or CPPUTILS_LIB_HEADER_FILES
                            # "$<BUILD_INTERFACE:${CPPUTILS_SOURCE_DIR}/libraries/imgui>"
                            # "$<BUILD_INTERFACE:${CPPUTILS_SOURCE_DIR}/libraries/imgui/backends>"
                            
                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_include_directories(CppUtils PRIVATE
                            # this must (again) contain all directories listed in CppUtils_INCLUDE_DIRS
                            "${CPPUTILS_SOURCE_DIR}/include"
                            "${CPPUTILS_BINARY_DIR}/include"
                            # "${CPPUTILS_SOURCE_DIR}/include/core"
                            # "${CPPUTILS_BINARY_DIR}/include/core"
                            
                            # this must (again) contain all directories listed in CPPUTILS_LIB_SOURCE_FILES or CPPUTILS_LIB_HEADER_FILES
                            # "${CPPUTILS_SOURCE_DIR}/libraries/imgui"
                            # "${CPPUTILS_BINARY_DIR}/libraries/imgui"
                            # "${CPPUTILS_SOURCE_DIR}/libraries/imgui/backends"
                            # "${CPPUTILS_BINARY_DIR}/libraries/imgui/backends"
)

target_link_libraries(CppUtils PUBLIC ${CppUtils_LINKED_LIBS})


# ===================================================================================
#   Generate and install library
# ===================================================================================

# if (CPPUTILS_BUILD_SHARED_LIBRARY)
#   if (UNIX)
#       # Hide symbols not explicitly tagged for export from the shared library
#         target_compile_options(CppUtils PRIVATE "-fvisibility=hidden")
#   endif()
# endif()

foreach(arg ${CppUtils_PKG_DEPS})
    string(APPEND deps " ${arg}")
endforeach()
foreach(arg ${CppUtils_PKG_LIBS})
    string(APPEND libs " ${arg}")
endforeach()

set(CPPUTILS_PKG_CONFIG_REQUIRES_PRIVATE "${deps}" CACHE INTERNAL
    "CPPUTILS pkg-config Requires.private")
set(CPPUTILS_PKG_CONFIG_LIBS_PRIVATE "${libs}" CACHE INTERNAL
    "CPPUTILS pkg-config Libs.private")

configure_file("${CPPUTILS_SOURCE_DIR}/CMake/CppUtils.pc.in" CppUtils.pc @ONLY)

if (CPPUTILS_INSTALL)
    install(TARGETS CppUtils
            EXPORT CppUtilsTargets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

# ===================================================================================
#   Install files other than the library
# ===================================================================================
    
    install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.h") # install all header files

    install(FILES "${CPPUTILS_BINARY_DIR}/src/CppUtilsConfig.cmake"
                  "${CPPUTILS_BINARY_DIR}/src/CppUtilsConfigVersion.cmake"
            DESTINATION "${CPPUTILS_CONFIG_PATH}")

    install(EXPORT CppUtilsTargets FILE CppUtilsTargets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
            DESTINATION "${CPPUTILS_CONFIG_PATH}")
    install(FILES "${CPPUTILS_BINARY_DIR}/src/CppUtils.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

endif()

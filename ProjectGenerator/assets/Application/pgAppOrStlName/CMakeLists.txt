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

# add function(s) to project
include(FetchContent)


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
#   Collect PGAPPORSTLNAME external library files
# ==================================================================================================

#   non-downloaded (FetchContent calls)

# GLFW (option 1)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)


#   downloaded (physical libraries)

# call find_package for locally (user) installed packages like glfw or OpenGL
# GLFW (option 2)
# find_package(glfw3 3.3 REQUIRED)

# add_subdirectory for anything containing a CMakeLists.txt file (option 1)
# EnTT
# add_subdirectory(${PGAPPORSTLNAME_SOURCE_DIR}/libraries/entt/)

# all cpp files (not included via CMakeLists.txt, option 2a)
file(GLOB PGAPPORSTLNAME_LIB_SOURCE_FILES LIST_DIRECTORIES false
    
    # note that this is not recursive, and will only add files/directories specified eg:
    # ${PGAPPORSTLNAME_SOURCE_DIR}/libraries/imgui/*.cpp
    # ${PGAPPORSTLNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
    # ${PGAPPORSTLNAME_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
)

# all header files (not included via CMakeLists.txt, option 2b)
file(GLOB_RECURSE PGAPPORSTLNAME_LIB_HEADER_FILES
    
    # eg:
    # ${PGAPPORSTLNAME_SOURCE_DIR}/libraries/imgui/*.h
)


# define links and frameworks for the above as libs
set(pgAppOrStlName_LINKED_LIBS 
    glfw
    # EnTT::EnTT
    # "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" # frameworks for ImGui
)


# ==================================================================================================
#   Create and link generated executable
# ==================================================================================================

add_executable(pgAppOrStl 
               ${PGAPPORSTLNAME_SOURCE_FILES} 
               ${PGAPPORSTLNAME_HEADER_FILES}
               ${PGAPPORSTLNAME_LIB_SOURCE_FILES}
               ${PGAPPORSTLNAME_LIB_HEADER_FILES})

target_include_directories(pgAppOrStl PRIVATE ${pgAppOrStlName_INCLUDE_DIRS})
target_link_libraries(pgAppOrStl PRIVATE ${pgAppOrStlName_LINKED_LIBS})

target_compile_definitions(pgAppOrStl PRIVATE 
    $<$<CONFIG:Debug>:DEBUG_BUILD=1>
    $<$<CONFIG:Debug>:RELEASE_BUILD=0>
    $<$<CONFIG:Release>:DEBUG_BUILD=0>
    $<$<CONFIG:Release>:RELEASE_BUILD=1>
)


# ==================================================================================================
#   Platform-specific application bundle configuration
# ==================================================================================================

if(APPLE)
    # Create macOS .app bundle
    set_target_properties(pgAppOrStl PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.bunting.pgAppOrStl"
        MACOSX_BUNDLE_BUNDLE_NAME "pgAppOrStl"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Info.plist.in"
    )
    
    # Add app icon
    set(APP_ICON_MACOSX "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon/icon.icns")
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(pgAppOrStl PRIVATE ${APP_ICON_MACOSX})
	# To change app icon, use or modify the iconGen.sh script
    
elseif(WIN32) # TODO: test Windows .exe generation
    # Create Windows .exe with proper subsystem
    set_target_properties(pgAppOrStl PROPERTIES
        WIN32_EXECUTABLE TRUE
        # OUTPUT_NAME "pgAppOrStl"  # Optional: specify exact .exe name
    )
    
    # Add Windows resource file for icon/version info
    target_sources(pgAppOrStl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/CMake/resources.rc")
    
endif()
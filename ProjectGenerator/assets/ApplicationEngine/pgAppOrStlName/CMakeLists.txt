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
# SublimeGenerator/SUBLIMEGENERATOR

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


# collect files
file(	GLOB_RECURSE SUBLIMEGENERATOR_SOURCE_FILES
	${SUBLIMEGENERATOR_SOURCE_DIR}/cpp/*.cpp
)

file(	GLOB_RECURSE SUBLIMEGENERATOR_HEADER_FILES
    ${SUBLIMEGENERATOR_SOURCE_DIR}/include/*.h
    ${SUBLIMEGENERATOR_SOURCE_DIR}/libraries/CppUtils/*.hpp
)

# create executable and ensure that it includes appropriate directories
add_executable(sg 
               ${SUBLIMEGENERATOR_SOURCE_FILES} 
               ${SUBLIMEGENERATOR_HEADER_FILES})

add_subdirectory(${SUBLIMEGENERATOR_SOURCE_DIR}/libraries/CppUtils/)

target_link_libraries(sg PRIVATE 
                        CppUtils
                        # nfd "-framework AppKit" "-framework UniformTypeIdentifiers" # frameworks for NFD
                        # "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
                      )

target_include_directories(sg PRIVATE
                           "${SUBLIMEGENERATOR_SOURCE_DIR}/include"
                           )

# adds SublimeGenerator custom local library
# target_link_libraries(sg PRIVATE SublimeGeneratorLib)
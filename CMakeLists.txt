cmake_minimum_required(VERSION 3.30)

# voxlet project
project(voxlet
	VERSION 0.0.1
	LANGUAGES CXX
)

if (MSVC)
	message(FATAL_ERROR "MSVC is not supported")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake)

find_package(Catch2)


# include the different part of voxlet
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/doc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sandbox    EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests      EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmarks EXCLUDE_FROM_ALL)

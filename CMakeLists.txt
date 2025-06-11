cmake_minimum_required(VERSION 3.25)

# voxlet project
project(voxlet
	VERSION 0.0.1
	LANGUAGES CXX
)


# The required version of the C++ standard. This value is hard-coded
set(VOXLET_CPP_REQUIRED_STANDARD 23)
# The used version of the C++ standard. If this value isn't support, it will decay
# to older version since ${VOXLET_CPP_REQUIRED_STANDARD}
set(VOXLET_CPP_STANDARD 26 CACHE STRING "The version of the C++ standard to use")
set_property(CACHE VOXLET_CPP_STANDARD PROPERTY STRINGS 23 26)

# The C++ standard must be a number greater or equal than the required standard
if (NOT (${VOXLET_CPP_STANDARD} GREATER_EQUAL ${VOXLET_CPP_REQUIRED_STANDARD}))
	message(FATAL_ERROR "Invalid version of the C++ standard (C++${VOXLET_CPP_STANDARD})
		was provided (C++${VOXLET_CPP_REQUIRED_STANDARD} required)"
	)
endif()

option(VOXLET_AS_SHARED "Use the shared version of Voxlet" On)


# Setup some directory path
set(VOXLET_ENGINE_DIR        ${PROJECT_SOURCE_DIR}/engine)
set(VOXLET_SANDBOX_DIR       ${PROJECT_SOURCE_DIR}/sandbox)
set(VOXLET_TESTS_DIR         ${PROJECT_SOURCE_DIR}/tests)
set(VOXLET_BENCHMARKS_DIR    ${PROJECT_SOURCE_DIR}/benchmarks)
set(VOXLET_DOC_DIR           ${PROJECT_SOURCE_DIR}/doc)
set(VOXLET_DOC_GENERATED_DIR ${VOXLET_DOC_DIR}/generated)
set(VOXLET_DEPS_DIR          ${PROJECT_SOURCE_DIR}/deps)
set(VOXLET_CMAKE_DIR         ${PROJECT_SOURCE_DIR}/cmake)


# import flex
set(FLEX_BUILD_HEADER_ONLY true)
add_subdirectory(${VOXLET_DEPS_DIR}/flex)

# import turbolin
include(${VOXLET_CMAKE_DIR}/turbolin.cmake)

# import SDL3
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)


# include the different part of voxlet
add_subdirectory(${VOXLET_ENGINE_DIR})
add_subdirectory(${VOXLET_DOC_DIR})
add_subdirectory(${VOXLET_SANDBOX_DIR}    EXCLUDE_FROM_ALL)
add_subdirectory(${VOXLET_TESTS_DIR}      EXCLUDE_FROM_ALL)
add_subdirectory(${VOXLET_BENCHMARKS_DIR} EXCLUDE_FROM_ALL)

cmake_minimum_required(VERSION 3.25)

if (VOXLET_AS_SHARED)
	set(__VOXLET_ENGINE_TYPE_NAME shared)
else()
	set(__VOXLET_ENGINE_TYPE_NAME static)
endif()
message("Voxlet Engine will be compiled using C++${VOXLET_CPP_STANDARD} as a ${__VOXLET_ENGINE_TYPE_NAME} library")
unset(__VOXLET_ENGINE_TYPE_NAME)


# an interface library to share infos between the static and shared build
add_library(voxlet_header_engine INTERFACE)
target_include_directories(voxlet_header_engine INTERFACE
	${VOXLET_ENGINE_DIR}/include
)
target_link_libraries(voxlet_header_engine INTERFACE
	flex::flex_header_only
	SDL3::SDL3
	turbolin::turbolin
	${VOXLET_RENDERER_LIBRARIES}
)

target_compile_definitions(voxlet_header_engine INTERFACE
	VOXLET_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
	VOXLET_VERSION_MINOR=${PROJECT_VERSION_MINOR}
	VOXLET_VERSION_PATCH=${PROJECT_VERSION_PATCH}
	${VOXLET_RENDERER_DEFINES}
	VOXLET_RENDERER_ENABLED_COUNT=${VOXLET_RENDERER_ENABLED_COUNT}
)

if (PROJECT_IS_TOP_LEVEL AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	if (MSVC)
		target_compile_options(voxlet_header_engine INTERFACE /W4)
	else()
		target_compile_options(voxlet_header_engine INTERFACE -Wall -Wextra -Wpedantic)
	endif()
endif()

# source files of the engine
file(GLOB_RECURSE VOXLET_SOURCE_FILES ${VOXLET_ENGINE_DIR}/src/*.cpp)
list(REMOVE_ITEM VOXLET_SOURCE_FILES ${VOXLET_RENDERER_TO_REMOVE_SOURCE_FILES})


#####################
### static engine ###
#####################
add_library(voxlet_static_engine STATIC ${VOXLET_SOURCE_FILES})
target_link_libraries(voxlet_static_engine PUBLIC voxlet_header_engine)
target_compile_definitions(voxlet_static_engine
	PRIVATE VOXLET_BUILD_ENGINE
	PUBLIC VOXLET_STATIC_ENGINE
)
# set the C++ standard and required standard
set_property(TARGET voxlet_static_engine PROPERTY CXX_STANDARD ${VOXLET_CPP_STANDARD})
set_property(TARGET voxlet_static_engine PROPERTY CXX_STANDARD_REQUIRED true)


#####################
### shared engine ###
#####################
add_library(voxlet_shared_engine SHARED ${VOXLET_SOURCE_FILES})
target_link_libraries(voxlet_shared_engine PUBLIC voxlet_header_engine)
target_compile_definitions(voxlet_shared_engine
	PRIVATE VOXLET_BUILD_ENGINE
	PUBLIC VOXLET_SHARED_ENGINE
)
# set the C++ standard and required standard
set_property(TARGET voxlet_shared_engine PROPERTY CXX_STANDARD ${VOXLET_CPP_STANDARD})
set_property(TARGET voxlet_shared_engine PROPERTY CXX_STANDARD_REQUIRED true)


# add some aliases
add_library(voxlet::static_engine ALIAS voxlet_static_engine)
add_library(voxlet::shared_engine ALIAS voxlet_shared_engine)
if (VOXLET_AS_SHARED)
	add_library(voxlet::engine ALIAS voxlet_shared_engine)
	set_property(TARGET voxlet_static_engine PROPERTY EXCLUDE_FROM_ALL true)
else()
	add_library(voxlet::engine ALIAS voxlet_static_engine)
	set_property(TARGET voxlet_shared_engine PROPERTY EXCLUDE_FROM_ALL true)
endif()

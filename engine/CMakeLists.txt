include(GenerateExportHeader)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(engine SHARED ${SOURCES})
add_library(voxlet::engine ALIAS engine)

set(GENERATE_EXPORT_HEADERS_UNDEF_DEPRECATION "
#undef VOXLET_DEPRECATED
#undef VOXLET_DEPRECATED_EXPORT
#undef VOXLET_DEPRECATED_NO_EXPORT"
)
generate_export_header(engine
	EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/generated/include/voxlet/export.hpp
	BASE_NAME VOXLET
	EXPORT_MACRO_NAME VOXLET_EXPORT
	NO_EXPORT_MACRO_NAME VOXLET_NO_EXPORT
	STATIC_DEFINE VOXLET_STATIC_DEFINE
	CUSTOM_CONTENT_FROM_VARIABLE GENERATE_EXPORT_HEADERS_UNDEF_DEPRECATION
)


target_include_directories(engine
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)
target_compile_features(engine PUBLIC cxx_std_23)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(engine PUBLIC -mavx2)

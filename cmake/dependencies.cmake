include(FetchContent)

macro(postdownload_hook_Catch2)
	if (DEFINED catch2_SOURCE_DIR)
		list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
	endif()
endmacro()

set(DEPENDENCIES_Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.8.1 # or a later release
	DOWNLOAD_DIR   ${PROJECT_BINARY_DIR}/download/vendors/Catch2
	SOURCE_DIR     ${PROJECT_SOURCE_DIR}/vendors/Catch2
	BINARY_DIR     ${PROJECT_BINARY_DIR}/vendors/Catch2
)
set(POSTDOWNLOAD_HOOK_Catch2 postdownload_hook_Catch2)


macro(find_package PACKAGE)
	if (NOT DEFINED DEPENDENCIES_${PACKAGE})
		_find_package(${ARGV})
	else()
		set(_DEPENDENCIES_${PACKAGE} ${DEPENDENCIES_${PACKAGE}})
		unset(DEPENDENCIES_${PACKAGE})
		FetchContent_Declare(
			${PACKAGE}
			${_DEPENDENCIES_${PACKAGE}}
			FIND_PACKAGE_ARGS ${ARGN}
		)
		FetchContent_MakeAvailable(${PACKAGE})

		if (DEFINED POSTDOWNLOAD_HOOK_${PACKAGE})
			cmake_language(CALL ${POSTDOWNLOAD_HOOK_${PACKAGE}})
		endif()
	endif()
endmacro()

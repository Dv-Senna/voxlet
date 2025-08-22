include(CTest)
include(Catch)

set(TESTS "containers")

add_custom_target(voxlet-tests)

foreach(TEST IN LISTS TESTS)
	message(${TEST})
	file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}/src/*.cpp)

	set(TARGET_NAME voxlet-tests-${TEST})
	add_executable(${TARGET_NAME} ${SOURCES})
	catch_discover_tests(${TARGET_NAME})
	target_link_libraries(${TARGET_NAME} PRIVATE voxlet::engine Catch2::Catch2WithMain)
	target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
	if (VOXLET_ENABLE_ADDRESS_SANITIZER)
		target_compile_options(${TARGET_NAME} PRIVATE -fsanitize=address)
		target_link_options(${TARGET_NAME} PRIVATE -fsanitize=address)
	endif()

	add_dependencies(voxlet-tests ${TARGET_NAME})
endforeach()

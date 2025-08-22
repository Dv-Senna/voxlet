set(BENCHMARKS "containers")

find_package(Python3 COMPONENTS Interpreter)

add_custom_target(voxlet-build-benchmarks)
if (Python3_Interpreter_FOUND)
	add_custom_target(voxlet-benchmarks)
else()
	message(WARNING "Python3 interpreter not found: you won't be able to run benchmarks with CMake")
endif()
set(CMAKE_BUILD_TYPE Release)

foreach(BENCHMARK IN LISTS BENCHMARKS)
	file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}/src/*.cpp)

	set(TARGET_NAME voxlet-build-benchmarks-${BENCHMARK})
	add_executable(${TARGET_NAME} ${SOURCES})
	target_link_libraries(${TARGET_NAME} PRIVATE voxlet::engine Catch2::Catch2WithMain)
	target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -save-temps)
	add_dependencies(voxlet-build-benchmarks ${TARGET_NAME})

	if (Python3_Interpreter_FOUND)
		set(RUN_TARGET_NAME voxlet-benchmarks-${BENCHMARK})
		add_custom_target(${RUN_TARGET_NAME}
			Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/python/generate_graph_from_benchmark.py
			${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:${TARGET_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/results/${BENCHMARKS}
			DEPENDS ${TARGET_NAME}
		)
		add_dependencies(voxlet-benchmarks ${RUN_TARGET_NAME})
	endif()
endforeach()

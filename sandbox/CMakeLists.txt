file(GLOB_RECURSE VOXLET_SANDBOX_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(voxlet_sandbox_app ${VOXLET_SANDBOX_SOURCE_FILES})
target_link_libraries(voxlet_sandbox_app PRIVATE voxlet::engine)

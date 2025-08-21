file(GLOB_RECURSE VOXLET_SANDBOX_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(voxlet-sandbox-app ${VOXLET_SANDBOX_SOURCE_FILES})
target_link_libraries(voxlet-sandbox-app PRIVATE voxlet::engine)

cmake_minimum_required(VERSION 3.25)


file(GLOB_RECURSE VOXLET_SANDBOX_SOURCE_FILES ${VOXLET_SANDBOX_DIR}/src/*.cpp)

add_executable(voxlet_sandbox_app ${VOXLET_SANDBOX_SOURCE_FILES})
target_link_libraries(voxlet_sandbox_app PRIVATE voxlet::engine)

# set the C++ standard and required standard
set_property(TARGET voxlet_sandbox_app PROPERTY CXX_STANDARD ${VOXLET_CPP_STANDARD})
set_property(TARGET voxlet_sandbox_app PROPERTY CXX_STANDARD_REQUIRED true)

# add alias to the sandbox application
add_executable(voxlet::sandbox_app ALIAS voxlet_sandbox_app)

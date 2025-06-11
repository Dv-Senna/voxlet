cmake_minimum_required(VERSION 3.25)

add_library(turbolin-header INTERFACE)
target_include_directories(turbolin-header INTERFACE ${VOXLET_DEPS_DIR}/turbolin/include)
add_library(turbolin::turbolin ALIAS turbolin-header)

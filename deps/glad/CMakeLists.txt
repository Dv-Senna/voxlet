cmake_minimum_required(VERSION 3.25)

project(glad
	VERSION 0.1.36
	LANGUAGES C
)


add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(glad::glad ALIAS glad)

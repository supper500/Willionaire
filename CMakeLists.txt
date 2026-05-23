cmake_minimum_required(VERSION 3.14)

project( Willionaire )

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	6f14f4792a0cde5d0cf2c910506724d61cb95834
)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.4
)

FetchContent_MakeAvailable(glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

find_package(Freetype REQUIRED)

add_subdirectory( external/glad )
include_directories( external/stb )
include_directories( external/incbin )

include_directories( include )

add_subdirectory( src )
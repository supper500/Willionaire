set(FORCE_RECOMPILE_STAMP "${CMAKE_CURRENT_BINARY_DIR}/force_recompile.stamp")

add_custom_target(update_force_recompile ALL
    COMMAND ${CMAKE_COMMAND} -E touch "${FORCE_RECOMPILE_STAMP}"
    COMMENT "Touch stamp to force recompilation"
    VERBATIM
)

set_source_files_properties(loader.cpp
    PROPERTIES OBJECT_DEPENDS "${FORCE_RECOMPILE_STAMP}"
)

set_source_files_properties(src/main.c PROPERTIES
    OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../assets/simsun.ttc"
)

set(WILLIONAIRE-SRC
	main.cpp
	card.cpp
	client.cpp
	connection.cpp
	content.cpp
	game.cpp
	loader.cpp
	player.cpp
	round.cpp
	server.cpp
	ui.cpp
	uibasic.cpp
	uiinit.cpp
	window.cpp
	gl/image.cpp
	gl/shader.cpp
	gl/text.cpp
	gl/texture.cpp
)

add_executable(Willionaire ${WILLIONAIRE-SRC} )

target_include_directories(Willionaire PRIVATE
	${OPENGL_INCLUDE_DIRS}
	../external/stb
	../external/incbin
	../include
)

target_link_libraries(Willionaire PRIVATE
	${OPENGL_LIBRARIES}
	glfw
	glad
	glm::glm
	freetype
)

if(WIN32)
    target_link_libraries(Willionaire PRIVATE ws2_32)
endif()
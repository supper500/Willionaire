
set( WILLIONAIRE-SRC
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

add_executable( Willionaire WIN32 ${WILLIONAIRE-SRC} )

target_link_libraries( Willionaire PRIVATE
	${OPENGL_LIBRARIES} 
	glfw 
	glad
	glm::glm
	Freetype::Freetype
)
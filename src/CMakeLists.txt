set(FORCE_RECOMPILE_STAMP "${CMAKE_CURRENT_BINARY_DIR}/force_recompile.stamp")

add_custom_target(update_force_recompile ALL
    COMMAND ${CMAKE_COMMAND} -E touch "${FORCE_RECOMPILE_STAMP}"
    COMMENT "Touch stamp to force recompilation"
    VERBATIM
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

if(MSVC)
    set(INCBIN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/data.c)
	set(INCBIN_SOURCES
		loader.cpp
		gl/text.cpp
	)
    add_custom_command(
        OUTPUT ${INCBIN_OUTPUT}
        COMMAND incbin_generator
        ARGS
            ${INCBIN_SOURCES}
            -o ${INCBIN_OUTPUT}
			"-I${CMAKE_CURRENT_SOURCE_DIR}"
        DEPENDS incbin_generator ${INCBIN_SOURCES} ${FORCE_RECOMPILE_STAMP}
        COMMENT "Generating incbin data.c for MSVC"
        VERBATIM
    )
	target_sources(Willionaire PRIVATE ${INCBIN_OUTPUT})
	target_compile_options(Willionaire PRIVATE /utf-8 /std:c++17)
else()

set_source_files_properties(loader.cpp
    PROPERTIES OBJECT_DEPENDS "${FORCE_RECOMPILE_STAMP}"
)

set_source_files_properties(gl/text.cpp PROPERTIES
    OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../assets/simsun.ttc"
)

target_compile_options(Willionaire PRIVATE -std=c++17)

endif()

target_include_directories(Willionaire PRIVATE
	${OPENGL_INCLUDE_DIRS}
	../external/stb
	../external/incbin/include
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
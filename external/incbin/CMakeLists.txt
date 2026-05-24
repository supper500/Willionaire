if(MSVC)
	add_executable(incbin_generator "${CMAKE_CURRENT_SOURCE_DIR}/src/incbin.c")
	target_include_directories(incbin_generator PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/include/incbin.h")
endif()
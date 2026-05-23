#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

// #define TEXT_TEST

class Text{
public:
	Text()=delete;
	static void init(const std::string& s);
	/// @brief render text
	/// @param text text to render
	/// @param x pos of left-buttom corner of the first character
	/// @param y same
	/// @param scale character size
	/// @param n max characters each line for word wrap, 0 for disabling
	/// @param color character color
	static void render(
		const std::string& text,
		float x, float y,
		float scale, std::size_t n,
		const glm::vec3& color);
	#ifdef TEXT_TEST
	static void test();
	#endif
};
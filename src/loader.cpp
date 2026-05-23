#include "Loader.h"

#include <incbin.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::map<std::string, std::pair<const unsigned char*, unsigned>> Loader::rawMap;
std::map<std::string, char*> Loader::stringMap;
std::map<std::string, GLFWimage> Loader::imageMap;
INCBIN(CopyVert, "../assets/shader/copy.vert");
INCBIN(CopyFrag, "../assets/shader/copy.frag");
INCBIN(OutLineFrag, "../assets/shader/outLine.frag");
INCBIN(GreyFrag, "../assets/shader/grey.frag");
INCBIN(CubeVert, "../assets/shader/cube.vert");
INCBIN(CubeFrag, "../assets/shader/cube.frag");
INCBIN(TextVert, "../assets/shader/text.vert");
INCBIN(TextFrag, "../assets/shader/text.frag");
INCBIN(PureVert, "../assets/shader/pure.vert");
INCBIN(PureFrag, "../assets/shader/pure.frag");
INCBIN(ColorPickerFrag, "../assets/shader/colorPicker.frag");
INCBIN(CircleVert, "../assets/shader/circle.vert");
INCBIN(CircleGeom, "../assets/shader/circle.geom");
INCBIN(CircleFrag, "../assets/shader/circle.frag");
#if USE_TEST
INCBIN(TestFrag, "../assets/shader/test.frag");
#endif

INCBIN(LQH, "../assets/sprites/lqh.png");

INCBIN(Energy, "../assets/sprites/energy.png");
INCBIN(Null, "../assets/sprites/null.png");
INCBIN(About, "../assets/sprites/about.png");

#define INCDICE(x) INCBIN(Dice##x, "../assets/sprites/dice_"#x".png")
INCDICE(1);INCDICE(2);INCDICE(3);INCDICE(4);INCDICE(5);INCDICE(6);

#define INCCARD(x) INCBIN(Card##x, "../assets/sprites/card"#x".png")
INCCARD(0);INCCARD(1);INCCARD(2);INCCARD(3);INCCARD(4);INCCARD(5);
INCCARD(6);INCCARD(7);INCCARD(8);INCCARD(9);INCCARD(10);INCCARD(11);
INCCARD(12);INCCARD(13);INCCARD(14);INCCARD(15);INCCARD(16);INCCARD(17);
INCCARD(18);INCCARD(19);INCCARD(20);INCCARD(21);INCCARD(22);INCCARD(23);
INCCARD(24);INCCARD(25);INCCARD(26);INCCARD(27);INCCARD(28);INCCARD(29);
INCCARD(30);INCCARD(31);

#define INCCAREER(x) INCBIN(Career##x, "../assets/sprites/career"#x".png")
INCCAREER(0);INCCAREER(1);INCCAREER(2);INCCAREER(3);INCCAREER(4);INCCAREER(5);
INCCAREER(6);INCCAREER(7);INCCAREER(8);INCCAREER(9);INCCAREER(10);INCCAREER(11);

#define INCATTRI(x) INCBIN(Attri##x, "../assets/sprites/attri"#x".png")
INCATTRI(0);INCATTRI(1);INCATTRI(2);INCATTRI(3);INCATTRI(4);

#define INSERTMAP(NAME) rawMap[#NAME]=std::make_pair(g##NAME##Data,g##NAME##Size)
void Loader::init() {
	stbi_set_flip_vertically_on_load(true);
	INSERTMAP(CopyVert);
	INSERTMAP(CopyFrag);
	INSERTMAP(OutLineFrag);
	INSERTMAP(GreyFrag);
	INSERTMAP(CubeVert);
	INSERTMAP(CubeFrag);
	INSERTMAP(TextVert);
	INSERTMAP(TextFrag);
	INSERTMAP(PureVert);
	INSERTMAP(PureFrag);
	INSERTMAP(ColorPickerFrag);
	INSERTMAP(CircleVert);
	INSERTMAP(CircleGeom);
	INSERTMAP(CircleFrag);
	#if USE_TEST
	INSERTMAP(TestFrag);
	#endif

	INSERTMAP(LQH);
	INSERTMAP(Energy);
	INSERTMAP(Null);
	INSERTMAP(About);
INSERTMAP(Dice1);INSERTMAP(Dice2);INSERTMAP(Dice3);INSERTMAP(Dice4);INSERTMAP(Dice5);INSERTMAP(Dice6);
INSERTMAP(Card0);INSERTMAP(Card1);INSERTMAP(Card2);INSERTMAP(Card3);INSERTMAP(Card4);INSERTMAP(Card5);
INSERTMAP(Card6);INSERTMAP(Card7);INSERTMAP(Card8);INSERTMAP(Card9);INSERTMAP(Card10);INSERTMAP(Card11);
INSERTMAP(Card12);INSERTMAP(Card13);INSERTMAP(Card14);INSERTMAP(Card15);INSERTMAP(Card16);INSERTMAP(Card17);
INSERTMAP(Card18);INSERTMAP(Card19);INSERTMAP(Card20);INSERTMAP(Card21);INSERTMAP(Card22);INSERTMAP(Card23);
INSERTMAP(Card24);INSERTMAP(Card25);INSERTMAP(Card26);INSERTMAP(Card27);INSERTMAP(Card28);INSERTMAP(Card29);
INSERTMAP(Card30);INSERTMAP(Card31);
INSERTMAP(Career0);INSERTMAP(Career1);INSERTMAP(Career2);INSERTMAP(Career3);INSERTMAP(Career4);INSERTMAP(Career5);
INSERTMAP(Career6);INSERTMAP(Career7);INSERTMAP(Career8);INSERTMAP(Career9);INSERTMAP(Career10);INSERTMAP(Career11);
INSERTMAP(Attri0);INSERTMAP(Attri1);INSERTMAP(Attri2);INSERTMAP(Attri3);INSERTMAP(Attri4);
}
char* Loader::loadString(std::string name) {
	if (stringMap.find(name) != stringMap.end()) return stringMap[name];
	if (!rawMap.count(name)){
		std::cerr<<name<<" not found\n";
	}
	const auto& [s, size] = rawMap[name];
	char* ret = new char[size + 1];
	memcpy(ret, s, size);
	ret[size] = '\0';
	return stringMap[name] = ret;
}
GLFWimage Loader::loadGLFWimage(std::string name) {
	if (imageMap.find(name) != imageMap.end()) return imageMap[name];
	if (!rawMap.count(name)){
		std::cerr<<name<<" not found\n";
	}

	GLFWimage ret;
	static int nrChannels;
	ret.pixels = stbi_load_from_memory(rawMap[name].first, rawMap[name].second,
		&ret.width, &ret.height, &nrChannels, 0);
	return imageMap[name] = ret;
}
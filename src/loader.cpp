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
INCBIN(Cancel, "../assets/sprites/cancel.png");
INCBIN(Info, "../assets/sprites/info.png");
INCBIN(Button, "../assets/sprites/button.png");
INCBIN(BackGround, "../assets/sprites/bg.png");

INCBIN(Dice1, "../assets/sprites/dice_1.png");
INCBIN(Dice2, "../assets/sprites/dice_2.png");
INCBIN(Dice3, "../assets/sprites/dice_3.png");
INCBIN(Dice4, "../assets/sprites/dice_4.png");
INCBIN(Dice5, "../assets/sprites/dice_5.png");
INCBIN(Dice6, "../assets/sprites/dice_6.png");

INCBIN(Card0, "../assets/sprites/card0.png");
INCBIN(Card1, "../assets/sprites/card1.png");
INCBIN(Card2, "../assets/sprites/card2.png");
INCBIN(Card3, "../assets/sprites/card3.png");
INCBIN(Card4, "../assets/sprites/card4.png");
INCBIN(Card5, "../assets/sprites/card5.png");
INCBIN(Card6, "../assets/sprites/card6.png");
INCBIN(Card7, "../assets/sprites/card7.png");
INCBIN(Card8, "../assets/sprites/card8.png");
INCBIN(Card9, "../assets/sprites/card9.png");
INCBIN(Card10, "../assets/sprites/card10.png");
INCBIN(Card11, "../assets/sprites/card11.png");
INCBIN(Card12, "../assets/sprites/card12.png");
INCBIN(Card13, "../assets/sprites/card13.png");
INCBIN(Card14, "../assets/sprites/card14.png");
INCBIN(Card15, "../assets/sprites/card15.png");
INCBIN(Card16, "../assets/sprites/card16.png");
INCBIN(Card17, "../assets/sprites/card17.png");
INCBIN(Card18, "../assets/sprites/card18.png");
INCBIN(Card19, "../assets/sprites/card19.png");
INCBIN(Card20, "../assets/sprites/card20.png");
INCBIN(Card21, "../assets/sprites/card21.png");
INCBIN(Card22, "../assets/sprites/card22.png");
INCBIN(Card23, "../assets/sprites/card23.png");
INCBIN(Card24, "../assets/sprites/card24.png");
INCBIN(Card25, "../assets/sprites/card25.png");
INCBIN(Card26, "../assets/sprites/card26.png");
INCBIN(Card27, "../assets/sprites/card27.png");
INCBIN(Card28, "../assets/sprites/card28.png");
INCBIN(Card29, "../assets/sprites/card29.png");
INCBIN(Card30, "../assets/sprites/card30.png");
INCBIN(Card31, "../assets/sprites/card31.png");

INCBIN(Career0, "../assets/sprites/career0.png");
INCBIN(Career1, "../assets/sprites/career1.png");
INCBIN(Career2, "../assets/sprites/career2.png");
INCBIN(Career3, "../assets/sprites/career3.png");
INCBIN(Career4, "../assets/sprites/career4.png");
INCBIN(Career5, "../assets/sprites/career5.png");
INCBIN(Career6, "../assets/sprites/career6.png");
INCBIN(Career7, "../assets/sprites/career7.png");
INCBIN(Career8, "../assets/sprites/career8.png");
INCBIN(Career9, "../assets/sprites/career9.png");
INCBIN(Career10, "../assets/sprites/career10.png");
INCBIN(Career11, "../assets/sprites/career11.png");

INCBIN(Attri0, "../assets/sprites/attri0.png");
INCBIN(Attri1, "../assets/sprites/attri1.png");
INCBIN(Attri2, "../assets/sprites/attri2.png");
INCBIN(Attri3, "../assets/sprites/attri3.png");
INCBIN(Attri4, "../assets/sprites/attri4.png");

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
	INSERTMAP(Cancel);
	INSERTMAP(Info);
	INSERTMAP(Button);
	INSERTMAP(BackGround);
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
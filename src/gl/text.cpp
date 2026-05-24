#include "gl/text.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "gl/shader.h"
#include <map>
#include <vector>
#include <incbin.h>

#define FONT_SIZE 48.0f

INCBIN(Font, "../assets/simsun.ttc");

struct Character {
	glm::vec2 TexCoordTopLeft;
	glm::vec2 TexCoordBottomRight;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

static std::map<unsigned int, Character> Characters;

static Shader shdr;
static unsigned int atlasTexture;
static unsigned int VAO, VBO;
static unsigned int atlasWidth, atlasHeight;

static FT_Library ft;

static unsigned int utf8_decode(std::string::const_iterator& it,
	const std::string::const_iterator& end){
	unsigned char c = *it++;
	if (c < 0x80) return c;
	unsigned int cp = 0;
	int extra = 0;
	if ((c & 0xE0) == 0xC0)      { cp = c & 0x1F; extra = 1; }
	else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; extra = 2; }
	else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; extra = 3; }
	else return 0xFFFD;
	for (int i = 0; i < extra && it != end; ++i) {
		cp = (cp << 6) | (*it++ & 0x3F);
	}
	return cp;
}

static bool BuildAtlas(FT_Face face, unsigned int fontSize,
	unsigned int& outWidth, unsigned int& outHeight, std::string s){
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int totalWidth  = 0;
	// unsigned int maxWidth    = 0;
	unsigned int maxHeight   = 0;
	// unsigned int lastHeight  = 0;

	for(unsigned char c=32;c<128;c++) s+=c;

	for(auto it=s.cbegin(), end=s.cend();it!=end;){
		unsigned int c = utf8_decode(it, end);
		if(Characters.find(c)!=Characters.end()) continue;
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
			std::cout << "WARNING: Character " << c << "loading failed" << std::endl;
			continue;
		}
		Character ch{};
		ch.Size     = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		ch.Bearing  = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		ch.Advance  = static_cast<unsigned int>(face->glyph->advance.x);
		Characters.insert({c,ch});

		totalWidth += face->glyph->bitmap.width + 1;
		// if(totalWidth>1024){
		// 	maxWidth=1024;
		// 	totalWidth=
		// }
		if(face->glyph->bitmap.rows > maxHeight)
			maxHeight = face->glyph->bitmap.rows;
	}

	totalWidth = (totalWidth + 3) & ~3;
	outWidth   = totalWidth;
	outHeight  = maxHeight;
	// std::cout<<"atlas width:"<<outWidth<<" height:"<<outHeight
	// 	<<" count:"<<Characters.size()<<std::endl;

	unsigned char *atlasBuffer=reinterpret_cast<unsigned char*>(calloc(outWidth * outHeight, 1));

	unsigned int xOffset = 0;
	for(auto it=s.cbegin(), end=s.cend();it!=end;){
		unsigned int c = utf8_decode(it, end);
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

		auto& ch=Characters[c];
		if(ch.TexCoordBottomRight.y) continue;

		FT_Bitmap& bitmap = face->glyph->bitmap;
		unsigned int w = bitmap.width;
		unsigned int h = bitmap.rows;

		for(unsigned int row = 0; row < h; row++)
			memcpy(&atlasBuffer[row*outWidth+xOffset],
				&bitmap.buffer[row*bitmap.pitch], w);

		ch.TexCoordTopLeft     = glm::vec2((float)xOffset / outWidth, 0.0f);
		ch.TexCoordBottomRight = glm::vec2((float)(xOffset + w) / outWidth, (float)h / outHeight);

		xOffset += w + 1;
	}

	glGenTextures(1, &atlasTexture);
	glBindTexture(GL_TEXTURE_2D, atlasTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, outWidth, outHeight, 0,
		GL_RED, GL_UNSIGNED_BYTE, atlasBuffer);

	free(atlasBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

static void GLInit(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

	shdr.compile("TextVert", "TextFrag");
	shdr.use();
	shdr.set("atlasTexture", 0);
}

static void _render(
	const std::string& text,
	float x, float y,
	float scale,
	const glm::vec3& color){

	std::vector<float> vertices;

	float startX = x;
	float curX = x, curY = y;

	for(auto it=text.cbegin(), end=text.cend();it!=end;){
		unsigned int c = utf8_decode(it, end);

		if(c == '\n'){
			curY -= (FONT_SIZE * scale);
			curX = startX;
			continue;
		}

		if (Characters.find(c) == Characters.end()) continue;

		const auto& ch = Characters[c];

		float xpos = curX + ch.Bearing.x * scale * 9/16;
		float ypos = curY - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale * 9/16;
		float h = ch.Size.y * scale;

		float u0 = ch.TexCoordTopLeft.x;
		float v0 = ch.TexCoordTopLeft.y;
		float u1 = ch.TexCoordBottomRight.x;
		float v1 = ch.TexCoordBottomRight.y;

		#define I(x) vertices.emplace_back(x)
		I(xpos);     I(ypos + h); I(u0); I(v0);
		I(xpos);     I(ypos);     I(u0); I(v1);
		I(xpos + w); I(ypos);     I(u1); I(v1);
		I(xpos + w); I(ypos);     I(u1); I(v1);
		I(xpos + w); I(ypos + h); I(u1); I(v0);
		I(xpos);     I(ypos + h); I(u0); I(v0);

		curX += (ch.Advance >> 6) * scale * 9/16;
	}
	if(vertices.empty()) return;

	shdr.use();
	shdr.set("textColor", color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlasTexture);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
		vertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size()>>2);
}

void Text::render(
	const std::string& text,
	float x, float y,
	float scale, std::size_t n,
	const glm::vec3& color){
	if(!n){
		_render(text, x, y, scale/FONT_SIZE, color);
		return;
	}
	std::string _t;
	std::size_t i=0;
	for(auto it=text.cbegin(), end=text.cend();it!=end;){
		auto oit=it;
		unsigned int c = utf8_decode(it, end);
		for(;oit<it;oit++) _t+=*oit;
		i=(c=='\n'?0:i+1);
		if(i==n) _t+='\n',i=0;
	}
	_render(_t, x, y, scale/FONT_SIZE, color);
}

void Text::init(const std::string& s){
	assert(!FT_Init_FreeType(&ft));

	FT_Face face;
	assert(!FT_New_Memory_Face(ft, gFontData, gFontSize, 0, &face));

	GLInit();
	assert(BuildAtlas(face, static_cast<unsigned>(FONT_SIZE), atlasWidth, atlasHeight, s));

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

#ifdef TEXT_TEST
#include "image.h"
void Text::test(){
	static Image ima;
	if(!ima.ID) ima.ID=atlasTexture;
	ima.draw();
}
#endif
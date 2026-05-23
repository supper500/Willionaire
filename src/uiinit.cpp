#define UI_IMPLEMENTATION
#include "ui.hpp"
#include "loader.h"
#include "gl/text.h"
#include "buff.hpp"
#include "career.hpp"

void UI::init(){
	Loader::init();
	Image::initAll();
	std::string characters;
	for(const auto& b:Game::buffs) characters+=b->characters();
	for(const auto& c:Game::cards) characters+=c->characters();
	for(const auto& c:Game::careers) characters+=c->characters();
	for(const auto& n:Card::typeName) characters+=n;
	characters+=Card::title;
	Text::init(characters);
	ColorPicker::init();
	outLine.compile("CopyVert","OutLineFrag");
	outLine.use();
	outLine.set("Texture",0);
	grey.compile("CopyVert","GreyFrag");
	grey.use();
	grey.set("Texture",0);
	diceDraw.compile("CubeVert","CubeFrag");
	diceDraw.use();
	diceDraw.set("textureArray",0);
	{
		GLFWimage t=Loader::loadGLFWimage("Dice1");
		glGenTextures(1,&diceTexture);
		glBindTexture(GL_TEXTURE_2D_ARRAY,diceTexture);
		glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RGBA,t.width,t.height,
			6,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
		for(int i=0;i<6;i++){
			char s[6]="Dice1";
			s[4]=i+'1';
			GLFWimage ima=Loader::loadGLFWimage(s);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,i,t.width,t.height,
				1,GL_RGBA,GL_UNSIGNED_BYTE,ima.pixels);
		}
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

		glGenVertexArrays(1,&diceVAO);
		glGenBuffers(1,&diceVBO);
		glBindVertexArray(diceVAO);
		glBindBuffer(GL_ARRAY_BUFFER,diceVBO);
		static const float vb[]={
			-0.5f,-0.5f,-0.5f,0.0f,0.0f,0.0f,
			 0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,
			 0.5f, 0.5f,-0.5f,1.0f,1.0f,0.0f,
			 0.5f, 0.5f,-0.5f,1.0f,1.0f,0.0f,
			-0.5f, 0.5f,-0.5f,0.0f,1.0f,0.0f,
			-0.5f,-0.5f,-0.5f,0.0f,0.0f,0.0f,

			-0.5f,-0.5f, 0.5f,0.0f,0.0f,5.0f,
			 0.5f,-0.5f, 0.5f,1.0f,0.0f,5.0f,
			 0.5f, 0.5f, 0.5f,1.0f,1.0f,5.0f,
			 0.5f, 0.5f, 0.5f,1.0f,1.0f,5.0f,
			-0.5f, 0.5f, 0.5f,0.0f,1.0f,5.0f,
			-0.5f,-0.5f, 0.5f,0.0f,0.0f,5.0f,

			-0.5f,-0.5f, 0.5f,0.0f,0.0f,1.0f,
			-0.5f, 0.5f, 0.5f,1.0f,0.0f,1.0f,
			-0.5f, 0.5f,-0.5f,1.0f,1.0f,1.0f,
			-0.5f, 0.5f,-0.5f,1.0f,1.0f,1.0f,
			-0.5f,-0.5f,-0.5f,0.0f,1.0f,1.0f,
			-0.5f,-0.5f, 0.5f,0.0f,0.0f,1.0f,

			 0.5f,-0.5f, 0.5f,0.0f,0.0f,4.0f,
			 0.5f, 0.5f, 0.5f,1.0f,0.0f,4.0f,
			 0.5f, 0.5f,-0.5f,1.0f,1.0f,4.0f,
			 0.5f, 0.5f,-0.5f,1.0f,1.0f,4.0f,
			 0.5f,-0.5f,-0.5f,0.0f,1.0f,4.0f,
			 0.5f,-0.5f, 0.5f,0.0f,0.0f,4.0f,

			-0.5f,-0.5f, 0.5f,0.0f,0.0f,2.0f,
			 0.5f,-0.5f, 0.5f,1.0f,0.0f,2.0f,
			 0.5f,-0.5f,-0.5f,1.0f,1.0f,2.0f,
			 0.5f,-0.5f,-0.5f,1.0f,1.0f,2.0f,
			-0.5f,-0.5f,-0.5f,0.0f,1.0f,2.0f,
			-0.5f,-0.5f, 0.5f,0.0f,0.0f,2.0f,

			-0.5f, 0.5f,-0.5f,0.0f,1.0f,3.0f,
			 0.5f, 0.5f,-0.5f,1.0f,1.0f,3.0f,
			 0.5f, 0.5f, 0.5f,1.0f,0.0f,3.0f,
			 0.5f, 0.5f, 0.5f,1.0f,0.0f,3.0f,
			-0.5f, 0.5f, 0.5f,0.0f,0.0f,3.0f,
			-0.5f, 0.5f,-0.5f,0.0f,1.0f,3.0f
		};
		glBufferData(GL_ARRAY_BUFFER,sizeof(vb),vb,GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(5*sizeof(float)));
	}
	pureColor.compile("PureVert","PureFrag");
	{
		glGenVertexArrays(1, &pureVAO);
		glGenBuffers(1, &pureVBO);
		glBindVertexArray(pureVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pureVBO);
		static const float vb[8]={
			1.0f, 1.0f,
			1.0f, -1.0f,
			-1.0f, -1.0f,
			-1.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vb), vb, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
	}
	circle.compile("CircleVert","CircleFrag","CircleGeom");

	{//textures
	Image tmp;
	#define ld(x,y) tmp.init(y),sprites[x]=tmp
	ld(NULL,"LQH");
	ld(SP_ATTRI_(0),"Attri0");
	ld(SP_ATTRI_(1),"Attri1");
	ld(SP_ATTRI_(2),"Attri2");
	ld(SP_ATTRI_(3),"Attri3");
	ld(SP_ATTRI_(4),"Attri4");
	ld(SP_ENERGY,"Energy");
	ld(SP_ENERGY_ZERO,"Null");
	ld(SP_NONE,"Null");
	ld(SP_MENU_BACKGROUND,"LQH");
	ld(SP_MENU_CLIENT,"LQH");
	ld(SP_MENU_SERVER,"LQH");
	ld(SP_MENU_QUIT,"LQH");
	ld(SP_GAME_BACKGROUND,"LQH");
	ld(SP_CLIENT_BACKGROUND,"LQH");
	ld(SP_CLIENT_ENTER,"LQH");
	ld(SP_HELP,"About");
	ld(SP_HELP_CLOSE,"Null");

	#define ldCareers(x) ld(Game::careers[x],"Career"#x);
ldCareers(0);ldCareers(1);ldCareers(2);ldCareers(3);ldCareers(4);ldCareers(5);
ldCareers(6);ldCareers(7);ldCareers(8);ldCareers(9);ldCareers(10);ldCareers(11);

	#define ldCards(x) ld(Game::cards[x],"Card"#x)
ldCards(0);ldCards(1);ldCards(2);ldCards(3);ldCards(4);ldCards(5);
ldCards(6);ldCards(7);ldCards(8);ldCards(9);ldCards(10);ldCards(11);
ldCards(12);ldCards(13);ldCards(14);ldCards(15);ldCards(16);ldCards(17);
ldCards(18);ldCards(19);ldCards(20);ldCards(21);ldCards(22);ldCards(23);
ldCards(24);ldCards(25);ldCards(26);ldCards(27);ldCards(28);ldCards(29);
ldCards(30);ldCards(31);

	ld(Game::buffs[0],"LQH");
	ld(Game::buffs[1],"LQH");
	ld(Game::buffs[2],"LQH");
	ld(Game::buffs[3],"LQH");
	ld(Game::buffs[4],"LQH");
	ld(Game::buffs[5],"LQH");
	#undef ld
	}

	#define SP_(x) (&sprites[reinterpret_cast<void*>(x)])
	f1.close=new Item(&sprites[SP_HELP_CLOSE],glm::vec4(),0.9578f,0.925f,0.0422f,0.075f);
	f1.column.resize(7);
	for(std::size_t i=0;i<5;i++)
		f1.column[i].name=Card::typeName[i]+Card::title;
	for(const auto&c:Game::cards)
		f1.column[static_cast<std::size_t>(c->type)].page.
			push_back({c->name,SP_(c),c->description});
	f1.column[5].name=Career::title;
	for(const auto&c:Game::careers)
		f1.column[5].page.push_back({c->name,SP_(c),c->description});
	f1.column[6].name="Buff";
	for(const auto&b:Game::buffs)
		f1.column[6].page.push_back({b->name(),SP_(b),b->description()});

	about.src=&sprites[SP_HELP];
	about.color=glm::vec4(0x40/255.0f, 0x70/255.0f, 0xa4/255.0f, 0.6f);

	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
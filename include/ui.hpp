#pragma once

#include "gl/image.h"
#include "game.hpp"
#include <utility>
#include <array>

class Cube;

class UI{
	static Shader outLine;
	static Shader grey;
	static Shader diceDraw;
	static unsigned diceTexture;
	static unsigned diceVAO;
	static unsigned diceVBO;
	static Shader pureColor;
	static unsigned pureVAO;
	static unsigned pureVBO;
	static Shader circle;

	struct Quad{
		float x,y,rx,ry,tx,ty;
		Quad()=default;
		Quad(float x,float y,float rx,float ry):x(x),y(y),rx(rx),ry(ry),tx(x),ty(y){}
		Quad(float x,float y,float rx,float ry,float tx,float ty):
			x(x),y(y),rx(rx),ry(ry),tx(tx),ty(ty){}
		glm::vec2 relativePos(glm::vec2 ndc){return glm::vec2((ndc.x-x)/rx,(ndc.y-y)/ry);}
		virtual void move(float tx,float ty){x=tx,y=ty;}
		bool hover() const;
		void moveTo(){
			if(fabs(tx-x)<0.01 && fabs(ty-y)<0.01) move(tx,ty);
			else move(x+(tx-x)/10,y+(ty-y)/10);
		}
		virtual ~Quad(){}
	};
	struct Item:Quad{
		Image *src;
		glm::vec4 color;
		Item()=default;
		Item(Image *src,glm::vec4 color,float x,float y,float rx,float ry):
			Quad(x,y,rx,ry),src(src),color(color){}
		virtual ~Item(){}
		void anim(){moveTo();}
	};
	struct Choice:Item{
		bool selected;
		Choice(Image *src,glm::vec4 color,float x,float y,float rx,float ry):
			Item(src,color,x,y,rx,ry),selected(false){}
	};
	static void addHighlightable(Item* i){
		units[1].emplace_back(i,i->color,&_draw_copy);
		if(i->hover()) units[0].emplace_back(i,i->color,&_draw_highlight);
	}
	typedef void(*PEN)(Item*,glm::vec4);
	typedef std::tuple<Item*,glm::vec4,PEN> UNIT;
	static void _draw_copy(Item* i,glm::vec4);
	static void _draw_shade(Item* i,glm::vec4 color);
	static void _draw_highlight(Item* i,glm::vec4 color);
	static void _draw_pulse(Item* i,glm::vec4 color);
	static void _draw_grey(Item* i,glm::vec4);
	template<std::size_t N>
	class Bar:Quad{
		glm::vec4 clr1,clr2;
		float dx,dy;
		unsigned amount;
		std::array<Item,N> item;
		template<std::size_t... Is>
		Bar(Image *src,glm::vec4 clr1,glm::vec4 clr2,float x,float y,float rx,float ry,
			float dx,float dy,std::index_sequence<Is...>):Quad(x,y,0.0f,0.0f),clr1(clr1),
			clr2(clr2),dx(dx),dy(dy),amount(0),item{Item(src,clr1,x+dx*Is,y+dy*Is,rx,ry)...}{}
	public:
		Bar(Image *src,glm::vec4 clr1,glm::vec4 clr2,float x,float y,float rx,float ry,float dx,float dy):
			Bar(src,clr1,clr2,x,y,rx,ry,dx,dy,std::make_index_sequence<N>{}){}
		const float X() const {return x;}
		const float Y() const {return y;}
		auto begin(){return item.begin();}
		auto end(){return item.end();}
		auto begin() const{return item.cbegin();}
		auto end() const{return item.cend();}
		void updateAmount(unsigned amount){this->amount=amount;}
		int getAmount(){return amount;}
		void updateColor1(glm::vec4 color){clr1=color;}
		void updateColor2(glm::vec4 color){clr2=color;}
		int hover(){
			int ans=0;
			for(unsigned i=0;i<amount;i++) if(item[i].hover()) ans=i+1;
			return ans;
		}
		void draw(){
			for(unsigned i=0;i<N;i++)
				units[1].emplace_back(&item[i],i<amount?clr1:clr2,
					i<amount?&_draw_copy:&_draw_shade);
			for(unsigned i=hover();i--;)
				units[0].emplace_back(&item[i],item[i].color,&_draw_highlight);
		}
		void move(float tx,float ty){
			for(unsigned i=0;i<N;i++) item[i].move(tx+i*dx,ty+i*dy);
			x=tx,y=ty;
		}
		void anim(){moveTo();}
	};
	class Player:Quad{
	protected:
		int pid;
		bool left;
		std::array<Bar<10>,5> attri;
		std::vector<Item> buff;
		template<std::size_t... Is>
		Player(float x,float y,bool left,::Player* p,std::index_sequence<Is...>);
		void updateBuff();
	public:
		Player(float x,float y,bool left,::Player* p):
			Player(x,y,left,p,std::make_index_sequence<5>{}){}
		virtual void move(float tx,float ty){
			for(auto &a:attri) a.move(tx-x+a.X(),ty-y+a.Y());
			for(auto &b:buff) b.move(tx-x+b.x,ty-y+b.y);
			x+=tx,y+=ty;
		}
		void turn(bool newLeft){left=newLeft;}
		void draw();
		virtual void update();
		void anim(){moveTo();}
	};
	class Self:public Player{
	public:
		Bar<11> energy;
		Item energyZero;
		std::array<Item,3> career;
	private:
		template<std::size_t... ls>
		Self(::Player* p,std::index_sequence<ls...>);
	public:
		Self(::Player* p):Self(p,std::make_index_sequence<3>{}){}
		void draw(){
			Player::draw();
			energy.draw();
			if(Game::state!=Game::ChooseCareer)
				for(auto &c:career)
					addHighlightable(&c);
			units[2].emplace_back(&energyZero,energyZero.color,&_draw_copy);
		}
		virtual void update();
		void anim(){
			energy.anim();
			energyZero.anim();
			for(auto &c:career) c.anim();
		}
	};
	struct Scene{
		int thisID;
		Self self;
		std::vector<Player> other;
		std::array<Item,3> cards;
		/// @param thisID the id of the player,not the id in the vector
		Scene(int thisID);
		void update();
		void draw();
		void anim();
	};
	class ColorPicker:public Quad{
		static Shader shader;
		static unsigned PVAO;
		static unsigned PVBO;
		glm::vec2 pos;
	public:
		static void init();
		#define dis() static_cast<float>(rand()) / (RAND_MAX+1) * 2 - 1.0f
		ColorPicker(float x=0.5f,float y=0.35f,float rx=0.225f,float ry=0.4f):
			Quad(x,y,rx,ry),pos(dis(),dis()){}
		#undef dis
		void draw() const;
		void pick(glm::vec2 newPos){pos=newPos;}
		glm::vec4 color() const;
		virtual ~ColorPicker();
	};
	struct F1{
		struct Page{
			std::string name;
			Image* icon;
			std::string content;
		};
		struct Column{
			std::string name;
			std::vector<Page> page;
		};
		std::vector<Column> column;
		bool show;
		std::size_t i, j;
		Item* close;
		F1():column(),show(false),i(0),j(0),close(NULL){}
		void draw();
		void click(double x, double y);
	};

	static Scene* scene;
	static std::map<void*,Image> sprites;
	static std::vector<UNIT> units[3];
	static bool _requireEnergy;
	static int _requireChoice;
	static std::vector<Choice> choice;
	static bool _acceptChoice;
	static void (*_callback)();
	static bool _keepChoice;
	static Cube* cube;
	static std::string addr;
	static bool addrError;
	static double hoverX,hoverY;
	static ColorPicker* picker;
	static int _self_color;
	static F1 f1;
	static Item about;
	static std::vector<glm::vec4> careerColor;
	static void hoverInit();
	static void drawInGame();
	static void drawMenu();
	static void drawClient();
	static void enterIP();
public:
	UI()=delete;
	static void init();
	static void start(int id);
	static void update();
	static void draw();
	static void press(GLFWwindow*, int key, int, int action, int);
	static void click(GLFWwindow*, int button, int action, int);
	static void requireEnergy(){_requireEnergy=true;}
	static void addCareer(const char *src);
	static void chooseCard();
	static void displayCard();
	static void FWHK();
	static void dice();
	static void win(int id);
	static int selfColor(){return _self_color;}
};

#ifdef UI_IMPLEMENTATION

#define SP_ATTRI reinterpret_cast<void*>(3ull<<62)
#define SP_ATTRI_(I) reinterpret_cast<void*>(3ull<<62|(I))

#define SP_ENERGY reinterpret_cast<void*>(3ull<<62|1<<8)
#define SP_ENERGY_ZERO reinterpret_cast<void*>(3ull<<62|1<<8|1)

#define SP_NONE reinterpret_cast<void*>(3ull<<62|2<<8)

#define SP_MENU_BACKGROUND reinterpret_cast<void*>(3ull<<62|4<<8|1)
#define SP_MENU_BUTTON reinterpret_cast<void*>(3ull<<62|4<<8|2)
// #define SP_MENU_SERVER reinterpret_cast<void*>(3ull<<62|4<<8|3)
// #define SP_MENU_QUIT reinterpret_cast<void*>(3ull<<62|4<<8|4)
#define SP_GAME_BACKGROUND reinterpret_cast<void*>(3ull<<62|4<<8|5)
#define SP_CLIENT_BACKGROUND reinterpret_cast<void*>(3ull<<62|4<<8|6)
#define SP_CLIENT_ENTER reinterpret_cast<void*>(3ull<<62|4<<8|7)
// #define SP_CLIENT_ADDR_ERROR reinterpret_cast<void*>(3ull<<62|4<<8|8)

#define SP_HELP reinterpret_cast<void*>(3ull<<62|5<<8)
#define SP_HELP_CLOSE reinterpret_cast<void*>(3ull<<62|5<<8|1)

extern const glm::vec4 LQH_R,LQH_B,attriClr[5],cardTypeClr[5];

#endif
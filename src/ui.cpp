#define UI_IMPLEMENTATION
#include "ui.hpp"
#include "connection.hpp"
#include "cube.hpp"
#include "gl/text.h"
#include "window.hpp"
#include "card.hpp"
#include <chrono>
#include <random>

int read_s(const char* &s);

std::map<void*,Image> UI::sprites;
std::vector<UI::UNIT> UI::units[3];
bool UI::_requireEnergy;
int UI::_requireChoice;
std::vector<UI::Choice> UI::choice;
bool UI::_acceptChoice;
void (*UI::_callback)();
bool UI::_keepChoice;
Cube* UI::cube;
std::string UI::addr;
bool UI::addrError;
UI::Item UI::about;
std::vector<glm::vec4> UI::careerColor;
const glm::vec4 LQH_R(1.0,0.0,0.0,1.0),LQH_B(0.0,0.0,1.0,1.0);
const glm::vec4 attriClr[5]={
	glm::vec4(0x5c/255.0f, 0xba/255.0f, 0xff/255.0f, 1.0f),
	glm::vec4(0xff/255.0f, 0xd1/255.0f, 0x35/255.0f, 1.0f),
	glm::vec4(0xff/255.0f, 0x95/255.0f, 0xc4/255.0f, 1.0f),
	glm::vec4(0x87/255.0f, 0xff/255.0f, 0xae/255.0f, 1.0f),
	glm::vec4(0x98/255.0f, 0x92/255.0f, 0xff/255.0f, 1.0f)
};
const glm::vec4 cardTypeClr[5]={
	glm::vec4(135/255.0f, 206/255.0f, 250/255.0f, 1.0f),
	glm::vec4(255/255.0f, 205/255.0f, 140/255.0f, 1.0f),
	glm::vec4(210/255.0f, 180/255.0f, 250/255.0f, 1.0f),
	glm::vec4(255/255.0f, 185/255.0f, 200/255.0f, 1.0f),
	glm::vec4(242/255.0f, 134/255.0f, 136/255.0f, 1.0f)
};

void UI::addCareer(const char* src){
	choice.clear();
	for(int i=0;i<5;i++){
		auto x=read_s(src);
		choice.emplace_back(
			&sprites[Game::careers[x]],
			careerColor[x],0.25*i-0.5,0.3,0.108,0.256
		);
	}
	_requireChoice=3;
	_acceptChoice=true;
	_callback=[]{
		for(int i=0,j=0;i<5;i++)
			if(choice[i].selected){
				scene->self.career[j].x=0.25*i-0.5;
				scene->self.career[j++].y=0.3;
			}
	};
}
void UI::chooseCard(){
	choice.clear();
	choice.emplace_back(&sprites[SP_NONE],LQH_R,0,-0.24,0.09,0.16);
	for(int i=0;i<3;i++)
		choice.emplace_back(
			&sprites[Game::choice[i]],
			cardTypeClr[Game::choice[i]->type],
			(i-1)*0.3,0.3,0.135,0.32
		);
	_requireChoice=1;
	_acceptChoice=true;
	_callback=[]{
		for(int i=0;i<3;i++)
			scene->cards[i].x=(i-1)*0.25,
			scene->cards[i].y=0.3;
	};
}
void UI::displayCard(){
	//yes, nothing here
}
void UI::FWHK(){
	choice.clear();
	choice.emplace_back(
		&sprites[SP_ATTRI_(1)],
		attriClr[1],-0.125,0.3,0.1,0.2
	);
	choice.emplace_back(
		&sprites[SP_ATTRI_(4)],
		attriClr[4],0.125,0.3,0.1,0.2
	);
	_requireChoice=1;
	_acceptChoice=true;
}
void UI::dice(){
	using namespace std::chrono;
	static std::mt19937 randGen(system_clock::now().time_since_epoch().count());
	static auto dis=std::uniform_real_distribution<double>(-1,1);
	static auto rnd=[]{return dis(randGen);};
	cube=new Cube{glm::vec3(0,6.0f,0),
		glm::normalize(glm::quat(1,rnd()*10,rnd()*10,rnd()*10)),
		glm::vec3(0,rnd()+7,0),
		glm::vec3(rnd()*5,rnd()*5,rnd()*5),
		0.7f,0.5f
	};
}
void UI::win(int id){
	void clientClose();
	clientClose();
}

void UI::enterIP(){
	bool clientInit(const std::string& addr);
	if(!clientInit(addr)){
		addrError=true;
		return;
	}
	_keepChoice=false;
	choice.clear();
	_requireChoice=0;
	_acceptChoice=false;
	delete picker;
	picker=NULL;
	Game::state=Game::Uninited;
	UI::scene=NULL;
	UI::update();
}
void UI::press(GLFWwindow*, int key, int, int action, int){
	if(action!=GLFW_PRESS) return;
	if(key==GLFW_KEY_F1 || key==GLFW_KEY_ESCAPE){
		f1.i=f1.j=0;
		f1.show^=(key==GLFW_KEY_F1?1:f1.show);
	}
	if(Game::state!=Game::Client) return;
	switch(key){
		case '.': case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': case ';':
			putchar(key);
			addr.push_back(key==';'?':':key);
			addrError=false;
			break;
		case GLFW_KEY_BACKSPACE:
			putchar('-');
			if(addr.size()) addr.pop_back();
			addrError=false;
			break;
		case GLFW_KEY_ENTER:
			putchar('/');
			putchar('\n');
			enterIP();
			break;
	}
}
void UI::click(GLFWwindow*, int button, int action, int){
	if(button!=GLFW_MOUSE_BUTTON_LEFT || action!=GLFW_PRESS) return;
	hoverInit();
	if(f1.show){
		f1.click(hoverX, hoverY);
		return;
	}
	if(about.hover()){
		f1.show=true;
		return;
	}
	if(scene && _requireEnergy){
		int h=scene->self.energy.hover();
		if(!h) return;
		_requireEnergy=false;
		char tmp[10];
		sprintf(tmp,"/%d\n",h-1);
		Connection::back_web(tmp);
	}
	if(picker && picker->hover())
		picker->pick( picker->relativePos( glm::vec2(hoverX,hoverY) ) );
	int sum=0;
	for(auto &c:choice){
		if(c.hover() && _acceptChoice)
			c.selected^=1;
		sum+=c.selected;
	}
	if(sum && sum==_requireChoice){
		if(Connection::connected){
			char buf[20],*p=buf;
			p+=sprintf(p,"/");
			for(unsigned i=0;i<choice.size();i++)
				if(choice[i].selected)
					p+=sprintf(p,"%d ",i);
			*(p-1)='\n';
			Connection::back_web(buf);
		}
		if(_callback){
			_callback();
			if(_keepChoice) return;
			_callback=NULL;
		}
		if(_keepChoice) return;
		choice.clear();
		_requireChoice=0;
		_acceptChoice=false;
	}
}

extern std::string _menu_string0, _menu_string1, _menu_string2;
void UI::drawMenu(){
	for(int i=0;i<3;i++) units[i].clear();
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	sprites[SP_MENU_BACKGROUND].draw();
	for(auto &c:choice){
		c.anim();
		addHighlightable(&c);
	}
	units[1].emplace_back(&about,glm::vec4(),_draw_copy);
	for(int i=0;i<3;i++)
		for(auto t:units[i])
			std::get<2>(t)(std::get<0>(t),std::get<1>(t));
	Text::render(_menu_string0,-0.7,0.565f,0.08f,0,glm::vec3(0.0f));
	Text::render(_menu_string1,-0.7,0.265f,0.08f,0,glm::vec3(0.0f));
	Text::render(_menu_string2,-0.7,-0.035f,0.08f,0,glm::vec3(0.0f));
}
extern std::string _client_string0, _client_string1, _client_string2;
void UI::drawClient(){
	for(int i=0;i<3;i++) units[i].clear();
	glClearColor(0.0,0.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	sprites[SP_CLIENT_BACKGROUND].draw();
	picker->draw();
	for(auto &c:choice){
		c.anim();
		addHighlightable(&c);
	}
	units[1].emplace_back(&about,glm::vec4(),_draw_copy);
	for(int i=0;i<3;i++)
		for(auto t:units[i])
			std::get<2>(t)(std::get<0>(t),std::get<1>(t));
	Text::render(_client_string0, 0.45, -0.535, 0.08, 0, glm::vec3(0.0f));
	Text::render(_client_string2+addr, -0.8, 0.2, 0.08, 30, glm::vec3(0.0f));
	if(addrError)
		Text::render(_client_string1, -0.4, 0.4, 0.08, 0, glm::vec3(1.0f,0.0f,0.0f));
}
void UI::drawInGame(){
	for(int i=0;i<3;i++) units[i].clear();

	glClearColor(0.0,1.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sprites[SP_GAME_BACKGROUND].draw();
	if(scene){
		scene->anim();
		scene->draw();
	}
	for(auto &c:choice){
		c.anim();
		addHighlightable(&c);
		if(c.selected)
			units[0].emplace_back(&c,c.color,_draw_pulse);
	}
	units[1].emplace_back(&about,glm::vec4(),_draw_copy);
	for(int i=0;i<3;i++)
		for(auto t:units[i])
			std::get<2>(t)(std::get<0>(t),std::get<1>(t));
	if(cube){
		glEnable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY,diceTexture);
		diceDraw.use();
		static glm::mat4 mat=glm::perspective(
			glm::radians(45.0f),
			16.0f/9.0f,
			0.1f,100.0f
		)*glm::lookAt(
			glm::vec3(11.0f,9.0f,0.0f),
			glm::vec3(0.0f,1.0f,0.0f),
			glm::vec3(0.0f,1.0f,0.0)
		);
		diceDraw.set("Mat",mat*cube->getTransformMatrix());
		glBindVertexArray(diceVAO);
		glDrawArrays(GL_TRIANGLES,0,36);
		glDisable(GL_DEPTH_TEST);
		static int still=0;
		cube->update(0.009);
		cube->update(0.009);
		if(cube->still()) still++;
		else still=0;
		if(still>60){
			char tmp[10];
			sprintf(tmp,"/%d",cube->getTopFace()+1);
			Connection::back_web(tmp);
			still=0;
			delete cube;
			cube=NULL;
		}
	}
}
void UI::draw(){
	#ifdef TEXT_TEST
	Text::test();
	return;
	#endif
	hoverInit();
	if(f1.show){
		glClearColor(0.0f,1.0f,1.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		f1.draw();
		return;
	}
	switch(Game::state){
		case Game::Menu:
			drawMenu();
			break;
		case Game::Client:
			drawClient();
			break;
		default:
			drawInGame();
			break;
	}
}
void UI::start(int id){
	if(scene) delete scene;
	scene=new Scene(id);
}
void UI::update(){
	const auto buttonC=glm::vec4(245/255.0f,245/255.0f,245/255.0f,1.0f);
	switch(Game::state){
		case Game::Menu:
			choice.clear();
			choice.emplace_back(
				&sprites[SP_MENU_BUTTON],
				buttonC,-0.6f,0.6f,0.2f,0.1f
			);
			choice.emplace_back(
				&sprites[SP_MENU_BUTTON],
				buttonC,-0.6f,0.3f,0.2f,0.1f
			);
			choice.emplace_back(
				&sprites[SP_MENU_BUTTON],
				buttonC,-0.6f,0.0f,0.2f,0.1f
			);
			_requireChoice=1;
			_acceptChoice=true;
			_callback=[]{
				if(choice[0].selected){
					Game::state=Game::Client;
					UI::update();
				}else{
					if(choice[1].selected)
						Game::state=Game::Server;
					glfwSetWindowShouldClose(Window::window,GLFW_TRUE);
				}
			};
			about.move(-0.9f,-0.8f);
			break;
		case Game::Client:
			picker=new ColorPicker;
			addrError=false;
			choice.clear();
			choice.emplace_back(
				&sprites[SP_CLIENT_ENTER],
				buttonC,0.5,-0.5,0.2,0.1
			);
			_requireChoice=1;
			_acceptChoice=true;
			_keepChoice=true;
			_callback=&enterIP;
			about.move(-0.9f,-0.8f);
			break;
		default:
			about.move(0.3f,0.8f);
			if(scene) scene->update();
			break;
	}
}
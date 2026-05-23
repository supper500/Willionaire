#define UI_IMPLEMENTATION
#include "ui.hpp"
#include "buff.hpp"
#include "window.hpp"
#include "gl/text.h"
#include <chrono>

Shader UI::outLine;
Shader UI::grey;
Shader UI::diceDraw;
unsigned UI::diceTexture;
unsigned UI::diceVAO;
unsigned UI::diceVBO;
Shader UI::pureColor;
unsigned UI::pureVAO;
unsigned UI::pureVBO;
Shader UI::circle;
UI::Scene* UI::scene;
double UI::hoverX,UI::hoverY;
UI::ColorPicker* UI::picker;
int UI::_self_color;
UI::F1 UI::f1;

void UI::hoverInit(){
	int lx,ly;
	glfwGetCursorPos(Window::window,&hoverX,&hoverY);
	glfwGetWindowSize(Window::window,&lx,&ly);
	hoverX=hoverX/lx*2-1,hoverY=1-hoverY/ly*2;
}
bool UI::Quad::hover() const{
	return hoverX>x-rx && hoverX<x+rx && hoverY>y-ry && hoverY<y+ry;
}
void UI::_draw_copy(Item* i,glm::vec4){
	i->src->updateLinear(glm::mat2(i->rx,0,0,i->ry));
	i->src->updateTranslation(glm::vec2(i->x,i->y));
	i->src->draw();
}
void UI::_draw_shade(Item* i,glm::vec4 color){
	outLine.use();
	outLine.set("Color",color);
	i->src->updateLinear(glm::mat2(i->rx,0,0,i->ry));
	i->src->updateTranslation(glm::vec2(i->x,i->y));
	i->src->draw(&outLine);
}
void UI::_draw_highlight(Item* i,glm::vec4 color){
	outLine.use();
	outLine.set("Color",color);
	i->src->updateLinear(glm::mat2(i->rx*1.15,0,0,i->ry*1.15));
	i->src->updateTranslation(glm::vec2(i->x,i->y));
	i->src->draw(&outLine);
}
void UI::_draw_pulse(Item* i,glm::vec4 color){
	outLine.use();
	using namespace std::chrono;
	auto t=system_clock::now().time_since_epoch();
	auto ms=duration_cast<milliseconds>(t);
	color.w*=(sin(ms.count()*asin(1.0)*4/1000)+1.0)/2;
	outLine.set("Color",color);
	i->src->updateLinear(glm::mat2(i->rx*1.4,0,0,i->ry*1.4));
	i->src->updateTranslation(glm::vec2(i->x,i->y));
	i->src->draw(&outLine);
}
void UI::_draw_grey(Item* i,glm::vec4){
	i->src->updateLinear(glm::mat2(i->rx,0,0,i->ry));
	i->src->updateTranslation(glm::vec2(i->x,i->y));
	i->src->draw(&grey);
}
template<std::size_t... Is>
UI::Player::Player(float x,float y,bool left,::Player* p,std::index_sequence<Is...>):
	Quad(x,y,0.0f,0.0f),pid(p->id),left(left),
	attri{Bar<10>(
		&sprites[SP_ATTRI_(Is)],attriClr[Is],glm::vec4(0.0,0.0,0.0,1.0),
		x,y+0.1-0.05*Is,0.012,0.02,left?-0.03:0.03,0
	)...}{update();}
void UI::Player::updateBuff(){
	::Player *p=&Game::players[pid];
	buff.clear();
	for(unsigned i=0;i<p->buff.size();i++)
		buff.emplace_back(
			&sprites[Game::buffs[p->buff[i]->id]],
			LQH_R,
			left?x-((i>>2)*0.09+0.31):x+((i>>2)*0.09+0.31),
			y+0.135-(i&3)*0.09,
			0.0225,0.04
		);
}
void UI::Player::update(){
	::Player *p=&Game::players[pid];
	for(int i=0,maxi=p->attri.maxi();i<5;i++){
		if(i==maxi){
			attri[i].updateAmount(p->attri[i]);
			attri[i].updateColor2(glm::vec4(0.0,0.0,0.0,1.0));
		}else{
			attri[i].updateAmount(0);
			attri[i].updateColor1(glm::vec4(0.2,0.2,0.2,0.7));
		}
	}
	updateBuff();
}
void UI::Player::draw(){
	pureColor.use();
	pureColor.set("linear",glm::mat2(0.16f,0.0f,0.0f,0.15f));
	pureColor.set("translation",glm::vec2(x+(left?-0.14f:0.14f),y));
	int c=Game::players[pid].color;
	pureColor.set("aColor",glm::vec4((c&255)/256.0f,(c>>8&255)/256.0f,(c>>16&255)/256.0f,0.7f));
	glBindVertexArray(pureVAO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	for(auto &a:attri) a.draw();
	for(auto &b:buff)
		addHighlightable(&b);
}
#define GEOM -0.35,-0.92,0.012,0.02
template<std::size_t... Is>
UI::Self::Self(::Player* p,std::index_sequence<Is...>):
	Player(0.05,-0.78,false,p),
	energy(&sprites[SP_ENERGY],LQH_R,glm::vec4(0.0,0.0,0.0,1.0),GEOM,0,0.042),
	energyZero(&sprites[SP_ENERGY_ZERO],LQH_R,GEOM),
	career{Item(&sprites[p->career[Is]],LQH_B,Is*0.1-0.25,-0.85,0.045,0.15)...}{}
void UI::Self::update(){
	::Player *p=&Game::players[pid];
	for(int i=0;i<5;i++)
		attri[i].updateAmount(p->attri[i]);
	energy.updateAmount(p->energy+1);
	for(int i=0;i<3;i++)
		career[i].src=&sprites[p->career[i]];
	updateBuff();
}
#undef GEOM

#define P Game::players
UI::Scene::Scene(int thisID):thisID(thisID),self(&P[thisID]){
	for(unsigned i=0;P[i].id!=thisID;i++)
		other.emplace_back(-0.98,0.8-i*0.4,false,&P[i]);
	for(unsigned i=P.size()-1;P[i].id!=thisID;i--)
		other.emplace_back(0.98,0.8-(P.size()-i-1)*0.4,true,&P[i]);
}
#undef P
void UI::Scene::update(){
	self.update();
	for(auto &p:other) p.update();
	if(Game::state==Game::Update || Game::state==Game::ChooseCard)
		for(int i=0;i<3;i++)
			cards[i]=Item(&sprites[Game::choice[i]],
				cardTypeClr[Game::choice[i]->type],(i-1)*0.13,0.8,0.054,0.128);
}
void UI::Scene::draw(){
	self.draw();
	for(auto &p:other)
		p.draw();
	if(Game::state==Game::Update || Game::state==Game::ChooseCard)
		for(int i=0;i<3;i++)
			addHighlightable(&cards[i]);
	if(_requireEnergy)
		for(auto &i:self.energy)
			units[0].emplace_back(&i,i.color,_draw_pulse);
}
void UI::Scene::anim(){
	self.anim();
	for(auto &p:other)
		p.anim();
	if(Game::state==Game::Update)
		for(int i=0;i<3;i++)
			cards[i].anim();
}

Shader UI::ColorPicker::shader;
unsigned UI::ColorPicker::PVAO;
unsigned UI::ColorPicker::PVBO;
void UI::ColorPicker::init(){
	shader.compile("CopyVert","ColorPickerFrag");
	glGenVertexArrays(1,&PVAO);
	glGenBuffers(1,&PVBO);
	glBindVertexArray(PVAO);
	glBindBuffer(GL_ARRAY_BUFFER,PVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(2*sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(4*sizeof(float)));
}
void UI::ColorPicker::draw() const{
	shader.use();
	shader.set("linear",glm::mat2(rx,0.0f,0.0f,ry));
	shader.set("translation",glm::vec2(x,y));
	glBindVertexArray(Image::VAO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	circle.use();
	glBindVertexArray(PVAO);
	glBindBuffer(GL_ARRAY_BUFFER,PVBO);
	float buf[8]={rx*pos.x+x,ry*pos.y+y,rx/10,ry/10};
	auto clr=glm::vec4(1.0f,1.0f,1.0f,2.0f)-color();
	memcpy(buf+4,&clr[0],4*sizeof(float));
	glBufferData(GL_ARRAY_BUFFER,8*sizeof(float),buf,GL_STREAM_DRAW);
	glDrawArrays(GL_POINTS,0,1);

	buf[2]*=0.8,buf[3]*=0.8;
	clr=glm::vec4(1.0f,1.0f,1.0f,2.0f)-clr;
	memcpy(buf+4,&clr[0],4*sizeof(float));
	glBufferData(GL_ARRAY_BUFFER,8*sizeof(float),buf,GL_STREAM_DRAW);
	glDrawArrays(GL_POINTS,0,1);
}
glm::vec4 UI::ColorPicker::color() const{
	glm::vec4 clr(0.0f,0.0f,0.0f,1.0f);
	float tx=(pos.x+1)*3;
	if(tx<1.0 || tx>5.0) clr.x=1.0;
	else if(tx<2.0) clr.x=2.0-tx;
	else if(tx>4.0) clr.x=tx-4.0;
	if(tx>1.0 && tx<3.0) clr.y=1.0;
	else if(tx<1.1) clr.y=tx;
	else if(tx<4.0) clr.y=4.0-tx;
	if(tx>3.0 && tx<5.0) clr.z=1.0;
	else if(tx>4.9) clr.z=6.0-tx;
	else if(tx>2.0) clr.z=tx-2.0;

	if(pos.y>0) clr*=(1.0f-pos.y);
	else clr=glm::vec4(-pos.y)+clr*(pos.y+1.0f);
	clr.w=1.0f;
	return clr;
}
UI::ColorPicker::~ColorPicker(){
	auto c=color();
	#define I(t) (static_cast<int>(t*256.0f)&255)
	_self_color=I(c.x)|I(c.y)<<8|I(c.z)<<16;
	#undef I
}

void UI::F1::draw(){
	const auto& p=column[i].page;
	glBindVertexArray(pureVAO);
	pureColor.use();
	pureColor.set("linear",glm::mat2(0.925f/column.size(),0.0f,0.0f,0.075f));
	pureColor.set("translation",glm::vec2(0.925f/column.size()*(i<<1|1)-1.0f,0.925f));
	pureColor.set("aColor",glm::vec4(0.0f,0.0f,0.0f,0.4f));
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	pureColor.set("linear",glm::mat2(0.25f,0.0f,0.0f,0.925f/p.size()));
	pureColor.set("translation",glm::vec2(-0.75f,0.85f-0.925f/p.size()*(j<<1|1)));
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	for(std::size_t i=0, size=column.size();i<size;i++)
		Text::render(column[i].name,1.85f*i/size-1.0f,0.88f,0.08f,0,glm::vec3(0.0f));
	for(std::size_t j=0, size=p.size();j<size;j++){
		p[j].icon->updateLinear(glm::mat2(0.03f,0.0f,0.0f,0.07f));
		p[j].icon->updateTranslation(glm::vec2(-0.955f,0.85f-0.925f/p.size()*(j<<1|1)));
		p[j].icon->draw();
		Text::render(p[j].name,-0.9,0.81f-0.925f/p.size()*(j<<1|1),0.08f,0,glm::vec3(0.0f));
	}
	Text::render(p[j].content,-0.5f,0.75f,0.08f,35,glm::vec3(0.0f));
	UI::_draw_copy(close,glm::vec4());
}
void UI::F1::click(double x, double y){
	if(x>-0.6 && y<0.85) return;
	if(y<0.85) j=(0.85-y)/(1.85/column[i].page.size());
	else if(x<0.85) i=(x+1)/(1.85/column.size()),j=0;
	else show=false;
}
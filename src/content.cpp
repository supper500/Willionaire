#include "attri.hpp"
const std::string Attri::name[5]={"智慧","力量","魅力","幸运","才华"};

#include "buff.hpp"
#define BUFF_INFO(x,y,z) \
std::string Buff##x::name() const{return y;}\
std::string Buff##x::description() const{return z;}
BUFF_INFO(Double,"上课摸鱼","下次获得的能力值翻倍")
BUFF_INFO(Coffee,"喝杯小咖啡","每次学习时额外获得1智慧")
BUFF_INFO(Prio,"外出旅游","下回合优先进行决策选择（如果有不止一个人则进行d6判定）")
BUFF_INFO(Sleep,"睡了12h","获得所有能力各1点，但下2回合无法行动")
BUFF_INFO(Daydream,"上课发呆","智慧不再增长，但每回合体力改为+3")
BUFF_INFO(LieFlat,"躺平摆烂","全能力-1，但体力一直为6")
#undef BUFF_INFO

#include "game.hpp"
#include "card.hpp"
#include "career.hpp"
std::string Career::title;
std::string _menu_string0, _menu_string1, _menu_string2;
std::string _client_string0, _client_string1, _client_string2;
void Game::init(){
	Card::typeName[0]="学习类";
	Card::typeName[1]="实践类";
	Card::typeName[2]="机遇类";
	Card::typeName[3]="休息类";
	Card::typeName[4]="比赛类";
	Card::title="卡牌";
	Career::title="职业";
	_menu_string0="客户端";
	_menu_string1="服务端";
	_menu_string2="退出";
	_client_string0="连接";
	_client_string1="连接失败";
	_client_string2="服务器地址：";
	#define a p.attri
	#define his(x) p.history[p.history.size()-x]->type==Card::Study
	careers={
		new Career("公务员",[](const Player &p){return a.i>=7 && a.t>=7;},"智慧>=7,才华>=7"),
		new Career("教师",[](const Player &p){return a.i>=7 && a.t>=4 && a.c>=4;},"智慧>=7,才华>=4,魅力>=4"),
		new Career("科研",[](const Player &p){return a.i>=9 && a.f>=5;},"智慧>=9,幸运>=5"),
		new Career("自由职业",[](const Player &p){return a.sum()>=25;},"全能力总和>=25"),
		new Career("运动员",[](const Player &p){return a.s==10;},"力量=10"),
		new Career("艺术家",[](const Player &p){return a.t>=8 && a.f>=6;},"才华>=8,幸运>=6"),
		new Career("赌神",[](const Player &p){return a.f==10;},"幸运=10"),
		new Career("股神",[](const Player &p){return a.i+a.f>=16;},"智慧+幸运>=16"),
		new Career("军人",[](const Player &p){return a.i>=7 && a.s>=7;},"智慧>=7,力量>=7"),
		new Career("演员",[](const Player &p){return a.c+a.i>=16;},"魅力+才华>=16"),
		new Career("无业游民",[](const Player &p){return Game::rounds>=9 && a.sum()<=20;},"在第10回合开始时全能力总和<=20"),
		new Career("专家",[](const Player &p){return p.history.size()>=3 && his(1) && his(2) && his(3);},"连续3回合学习")
	};
	#undef his
	#undef a
	cards={
		new CardStudy("图书馆自习",1,0,0,0,0,"智慧+1"),
		new CardStudy("认真听课",1,0,0,0,0,"智慧+1"),
		new CardStudy("作诗一首",0,0,0,0,1,"才华+1"),
		new CardFeiWenHuaKe("非文化课","（体育）力量+1/（美术）才华+1"),
		new CardStudy("博雅技艺",0,0,1,0,0,"魅力+1"),
		new CardPractice("公益活动",0,0,0,2,0,"幸运+2"),
		new CardPractice("企业参观",2,0,0,0,0,"智慧+2"),
		new CardPractice("参观博物馆",1,0,1,0,0,"魅力+1 智慧+1"),
		new CardPractice("学术交流",1,0,0,0,1,"才华+1 智慧+1"),
		new CardPractice("模拟长征",0,2,0,0,0,"力量+2"),
		new CardPractice("感受浙大",1,1,1,1,1,"全能力+1"),
		new CardChance("项目报告",0,0,0,0,2,4,">=4:才华+2"),
		new CardChance("行业论坛发言",2,0,0,0,0,5,">=5：智慧+2"),
		new CardChance("户外拓展领队",0,2,0,0,0,6,">=6：力量+2"),
		new CardChance("校园晚会主持",0,0,2,0,0,7,">=7：魅力+2"),
		new CardChance("校园才艺展演",0,0,0,0,2,6,">=6：才华+2"),
		new CardChance("跨专业合作项目",2,0,0,0,1,9,">=9：智慧+2 才华+1"),
		new CardChance("公益形象大使选拔",0,0,1,1,0,8,">=8：魅力+1 幸运+1"),
		new CardChance("学术论文投稿",2,0,0,0,1,10,">=10：智慧+2 才华+1"),
		new CardRest<BuffDouble>("上课摸鱼",0,0,0,0,0,"下次获得的能力值翻倍"),
		new CardRest<BuffCoffee>("喝杯小咖啡",0,0,0,0,0,"每次学习时额外获得1智慧"),
		new CardRest<BuffPrio>("外出旅游",0,0,0,0,0,"下回合优先进行决策选择（如果有不止一个人则进行d6判定）"),
		new CardRest<BuffSleep>("睡了12h",1,1,1,1,1,"获得所有能力各1点，但下2回合无法行动"),
		new CardYouLiQiZhenHu("游历启真湖","能力值最高的和最低的互换"),
		new CardRest<BuffDaydream>("上课发呆",0,0,0,0,0,"智慧不再增长，但每回合体力改为+3"),
		new CardRest<BuffLieFlat>("躺平摆烂",-1,-1,-1,-1,-1,"全能力-1，但体力一直为6"),
		new CardCompetition("校园歌手大赛",0,0,2,0,0,2,4,"魅力+才华最高者魅力+2"),
		new CardCompetition("大学生运动会",0,2,0,0,0,1,5,"力量最高者力量+2"),
		new CardCompetition("大学生演讲比赛",0,0,0,0,2,4,5,"才华最高者才华+2"),
		new CardCompetition("激烈辩论赛",0,0,2,0,0,0,4,"智慧+才华最高者魅力+2"),
		new CardCompetition("知识竞赛",2,0,0,0,0,0,5,"智慧最高者智慧+2"),
		new CardCompetition("校园棋牌赛",0,0,0,2,0,3,5,"运气最高者运气+2")
	};
	buffs={
		new BuffDouble,
		new BuffCoffee,
		new BuffPrio,
		new BuffSleep,
		new BuffDaydream,
		new BuffLieFlat
	};
	idMap[NULL]=0;
	#define INIT_MAP(x) for(unsigned i=x.size();i--;) idMap[x[i]]=i
	INIT_MAP(careers);
	INIT_MAP(cards);
	INIT_MAP(buffs);
	#undef INIT_MAP
}
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
using namespace std;

#define MAXBUFSIZE 1024 

class Pet{
	private:			/*精灵属性包括：*/ 
		int type;		/*精灵种类*/ 
						/*力量型：1，防御型：2，敏捷型：3，肉盾型：4*/ 
		int attack;		/*能力属性：攻击*/ 
		int defence;	/*能力属性：防御*/ 
		int speed;		/*能力属性：攻击间隔*/ 
		int power;		/*能力属性：生命值*/ 
		char name[20];	/*名称*/ 
		int level;		/*等级（大于等于1，小于等于15）*/ 
		int exp;		/*经验值*/ 
		/*以下属性为额外添加*/
		int id;					/*精灵序号*/ 
								/*序号是精灵的特性，一旦序号确定，其名称、种类、种族值和攻击方式唯一确定*/ 
		int sp_attack;			/*种族值*/ 
		int sp_defence;			/*精灵的能力属性由种族值和等级决定*/ 
		int sp_speed;
		int sp_power;
		char skill_name[20];	/*攻击方式名称*/ 
	public:
		void init_p(int, int);
		virtual void skill(){	/*攻击方式（本程序中亦称为“技能”）*/ 
			cout << "【" << name << "】使用了【"<< skill_name << "】" << endl;
		}
		void lvup(int, int);	/*升级*/ 
		const void show();			/*展示精灵信息*/ 
		const void showname();		/*输出精灵名称*/ 
		const int getinfo(int);		/*获取精灵的某项属性*/ 
		int addexp(int);			/*精灵加经验*/ 
};

void Pet::init_p(int pid,int lv){
								/*精灵属性初始化*/ 
	ifstream fp("input.txt",ios::binary);
	int ty,sp_at,sp_de,sp_sp,sp_po;
	char na[20];
	char sk_na[20];
	int pet_max,tempid;
	fp >> pet_max;
	for(int i = 0; i <= pid; i++){
		fp >> tempid >> ty >> sp_at >> sp_de >> sp_sp >> sp_po >> na >> sk_na;
	}
	fp.close();
	
	id = pid;
	sp_attack = sp_at;
	sp_defence = sp_de;
	sp_speed = sp_sp;
	sp_power = sp_po;
	type = ty;
	
	if(lv < 1){
		lv = 1;
	}
	level = lv; 
	exp = 0; 
	
	attack = (sp_attack * 2 + 31) * (7.07 * level - 6.07) / 100 + 5; 
	defence = (sp_defence * 2 + 31) * (7.07 * level - 6.07) / 100 + 5;
	speed = (sp_speed * 2 + 31) * (7.07 * level - 6.07) / 100 + 5;
	power = (sp_power * 2 + 31) * (7.07 * level - 6.07) / 100 + 10 + (7.07 * level - 6.07);
	
	strcpy(name,na); 
	strcpy(skill_name,sk_na); 
}

void Pet::lvup(int dlv, int out){	/*精灵升级，参数为提升的等级；超过15级时改为提升至15级*/ 
	int flv = level;
	int fat = attack;
	int fde = defence;
	int fsp = speed;
	int fpo = power;
	
	level += dlv;
	if(level > 15){
		level = 15;
	}
	else if(level <= 0){
		level = 1;
	}

	attack = (sp_attack * 2 + 31) * (7.07 * level - 6.07) / 100 + 5;
	defence = (sp_defence * 2 + 31) * (7.07 * level - 6.07) / 100 + 5;
	speed = (sp_speed * 2 + 31) * (7.07 * level - 6.07) / 100 + 5;
	power = (sp_power * 2 + 31) * (7.07 * level - 6.07) / 100 + 10 + (7.07 * level - 6.07);
	
	if(out == 1){
		cout << "【" << name << "】升级了！";
		cout << "等级：" << flv << " + " << level - flv << " -> " << level << endl;
		cout << "攻击：" << fat << " + " << attack - fat << " -> " << attack << endl;
		cout << "防御：" << fde << " + " << defence - fde << " -> " << defence << endl;
		cout << "敏捷：" << fsp << " + " << speed - fsp << " -> " << speed << endl;
		cout << "生命：" << fpo << " + " << power - fpo << " -> " << power << endl;
	}
	
}

const void Pet::show(){	/*展示精灵信息*/ 
	cout << "序号：" << id << endl; 
	cout << "精灵名：【" << name << "】" << endl;
	cout << "等级：" << level << endl;
	cout << "攻击：" << attack << endl;
	cout << "防御：" << defence << endl;
	cout << "敏捷：" << speed << endl;
	cout << "生命：" << power << endl;
}

const void Pet::showname(){ /*输出精灵名称*/ 
	cout << "【" << name << "】" ;
}

const int Pet::getinfo(int flag){ 	/*获取精灵的某项信息*/ 
	if(flag == 1){					/*参数为1，获取精灵序号*/ 
		return id;
	}
	else if(flag == 2){				/*参数为2，获取精灵等级*/ 
		return level;
	} 
	else if(flag == 3){				/*参数为3，获取精灵攻击*/ 
		return attack;
	} 
	else if(flag == 4){				/*参数为4，获取精灵防御*/ 
		return defence;
	} 
	else if(flag == 5){				/*参数为5，获取精灵敏捷*/ 
		return speed;
	} 
	else if(flag == 6){				/*参数为6，获取精灵生命值*/ 
		return power;
	} 
	else if(flag == 7){				/*参数为7，获取精灵经验值*/ 
		return exp;
	} 
}

int Pet::addexp(int dexp){	/*精灵加经验*/ 
							/*经验机制：当精灵经验达到等级的平方乘10时，触发精灵升级，升级后经验值重新累计*/ 
	exp += dexp;
	int dlv = 0;
	int temp_lv = level;
	while(temp_lv < 15 && exp >= temp_lv * temp_lv * 10){
		exp -= temp_lv * temp_lv * 10;
		dlv++;
		temp_lv++;
	}
	if(dlv > 0){
		lvup(dlv, 1);
		return dlv;
	}
	return 0;
}

class Users{
	private:
		char username[20];		/*用户名*/ 
		int online;				/*在线状态，在线为1，不在线为0，未注册为-1*/ 
		int pet_tot;			/*用户精灵总数*/ 
		int pet_tot15;			/*15级精灵总数*/ 
		Pet* petp;				/*精灵指针*/ 
		char password[20];		/*密码*/ 
		int win;				/*胜场*/ 
		int lose; 				/*负场*/ 
		int badge;				/*宠物个数徽章，未拥有为0，铜1，银2，金3*/
		int badge15;			/*高级宠物徽章*/ 
	public:
		Users();
		void signin(char* ,char*);			/*注册*/	 
		void log(int);				 		/*登录和登出*/ 
		int checkstatus();					/*检测是否注册、登录状态*/ 
		const void showpet();				/*查看拥有的精灵*/ 
		const void showusername(char*);		/*查看用户名*/ 
		int newpet(int, int);				/*增加一只精灵*/ 
		const int usntaken(char*);			/*检测用户名是否匹配*/
		const int pswtaken(char*);			/*检测密码是否匹配*/ 
		const void getusername(char*);		/*获取用户名*/ 
		const int getpet(char*);			/*获取用户的精灵（序号）及精灵总数*/ 
		void badge_upd();					/*更新用户的徽章*/
		const int getuserinfo(int); 		/*获取用户胜负场、徽章信息*/ 
		void userwin(int);					/*记录用户胜/负场+1*/
		int addexp_pet(int, int);			/*给用户的一只指定精灵增加经验值*/ 
		const void getpet_one(char* ,int);	/*获取用户的指定精灵序号和等级*/ 
		void losepet(int);					/*失去一只精灵*/ 
};

Users::Users(){
	online = -1;
	for(int i = 0; i < 20; i++){
		username[i] = '\0';
		password[i] = '\0';
	}
}

void Users::signin(char* usern, char* userp){//注册，参数为用户名和密码 
	memcpy(username, usern, 20);
	memcpy(password, userp, 20);
	online = 0;
	pet_tot = 0;
	pet_tot15 = 0;
	petp = NULL;
	win = 0;
	lose = 0;
	badge = 0;
	badge15 = 0;	
}

void Users::log(int s){//参数为1表示登录，参数为0表示登出 
	online = s;
}

int Users::checkstatus(){//返回用户的在线状态 
	return online;
}

int Users::newpet(int num,int lv){//用户获得新精灵，参数为精灵序号,返回该用户新的精灵总数 
	pet_tot += 1;
	Pet* temp = new Pet[pet_tot];
	for(int i = 0; i < pet_tot - 1; i++){
		temp[i] = petp[i];
	}
	delete(petp);
	petp = temp;
	temp = NULL;
	if(lv < 1){
		lv = 1;
	}
	petp[pet_tot - 1].init_p(num, lv); 
	
	badge_upd();
	 
	return pet_tot;
}

void Users::losepet(int num){//用户失去精灵，参数为失去第几只精灵 
	pet_tot -= 1;
	Pet* temp = new Pet[pet_tot];
	for(int i = 0, j = 0; i < pet_tot; i++, j++){
		if(j == num){
			if(petp[j].getinfo(2) == 15){
				pet_tot15--;
			}
			j++;
		}
		temp[i] = petp[j];
	}
	delete(petp);
	petp = temp;
	temp = NULL;
	
	badge_upd();
}

const int Users::usntaken(char* in){/*检测用户名是否匹配*/
	if(memcmp(in,username,20) == 0){
		return 1;
	}
	else{
		return 0;
	}
}

const int Users::pswtaken(char* in){/*检测密码是否匹配*/
	if(memcmp(in,password,20) == 0){
		return 1;
	}
	else{
		return 0;
	}
}

const void Users::getusername(char* usn){/*获取用户名*/
	memcpy(usn, username, 20);
}

const int Users::getpet(char* buf){/*获取用户的精灵（序号）及精灵总数*/
	for(int i = 0; i < pet_tot; i++){
		*(buf + i * 2) = petp[i].getinfo(1);
		*(buf + i * 2 + 1) = petp[i].getinfo(2);
	}
	return pet_tot;
}

const int Users::getuserinfo(int flag){	/*获取用户胜负场、徽章信息*/
	if(flag == 1){						/*参数为1，获取胜场数*/ 
		return win;
	}
	else if(flag == 2){					/*参数为2，获取负场数*/ 
		return lose;
	} 
	else if(flag == 3){					/*参数为3，获取宠物个数徽章*/ 
		badge_upd();
		return badge;
	} 
	else if(flag == 4){					/*参数为4，获取高级宠物徽章*/ 
		badge_upd();
		return badge15;
	}
}

void Users::userwin(int i){	/*记录用户胜/负场数+1*/ 
	if(i == 1){
		win++;
	}
	else if(i == 2){
		lose++;
	}
}

int Users::addexp_pet(int petnum, int dexp){	/*给用户的第petnum只精灵增加dexp经验值*/ 
	int dlv = petp[petnum].addexp(dexp);
	if(dlv + petp[petnum].getinfo(2) >= 15){
		pet_tot15++;
		badge_upd();
	}
	
	return dlv;
}

const void Users::getpet_one(char* buf,int num){	/*获取用户第num只精灵的属性*/ 
	for(int i = 0; i < 6; i++){
		*(buf + i) = petp[num].getinfo(i + 1);
	}	
}

void Users::badge_upd(){	/*更新用户的两种徽章*/ 
	if(pet_tot >= 15){
		badge = 3;
	} 
	else if(pet_tot >= 10){
		badge = 2;
	}
	else if(pet_tot >= 5){
		badge = 1;
	}
	else{
		badge = 0;
	}
	
	if(pet_tot15 >= 15){
		badge15 = 3;
	} 
	else if(pet_tot15 >= 10){
		badge15 = 2;
	}
	else if(pet_tot15 >= 5){
		badge15 = 1;
	}
	else{
		badge15 = 0;
	}
}

class Battle{		 
	public:
		Pet userpet;				/*我方出战精灵*/ 
		Pet syspet;					/*对方出战精灵*/ 
		int userhp;					/*我方当前血量*/ 
		int syshp;					/*对方当前血量*/ 
		Battle(int, int, int, int);
		int battle();				/*开始对战*/ 
		const int dmg(int);			/*造成伤害*/ 
};

Battle::Battle(int usr_id, int usr_lv, int sys_id, int sys_lv){	/*对战双方信息初始化*/ 
	userpet.init_p(usr_id, usr_lv);
	//userpet.lvup(usr_lv - 1, 0);
	syspet.init_p(sys_id, sys_lv);
	//syspet.lvup(sys_lv - 1, 0);
}

int Battle::battle(){	/*双方开始对战，当一方当前生命值降为0时对战结束*/ 
	int userhp = userpet.getinfo(6);
	int userhpmax = userpet.getinfo(6);
	int syshp = syspet.getinfo(6);
	int syshpmax = syspet.getinfo(6);
	userpet.showname();
	cout << "生命值：" << userhp << " / " << userhpmax << endl;
	syspet.showname();
	cout << "生命值：" << syshp << " / " << syshpmax << endl;
	cout << "战斗开始！" << endl;
	srand(time(NULL));
	for(int i = 0; ; i++){
		Sleep(1000);
		//cout << userpet.getinfo(5) << " " << syspet.getinfo(5) << endl;
		if(userpet.getinfo(5) >= syspet.getinfo(5)){
			userpet.skill();
			syshp -= dmg(1);
			cout << "生命值：" << syshp << " / " << syshpmax << endl << endl;
			if(syshp <= 0){
				cout << "恭喜您获得了胜利！" << endl;
				return 1;
			}
			syspet.skill();
			userhp -= dmg(2);
			cout << "生命值：" << userhp << " / " << userhpmax << endl << endl;
			if(userhp <= 0){
				cout << "很遗憾，您失败了！" << endl;
				return 2; 
			}
		}
		else{
			syspet.skill();
			userhp -= dmg(2);
			cout << "生命值：" << userhp << " / " << userhpmax << endl << endl;
			if(userhp <= 0){
				cout << "很遗憾，您失败了！" << endl; 
				return 2;
			}
			userpet.skill();
			syshp -= dmg(1);
			cout << "生命值：" << syshp << " / " << syshpmax << endl << endl;
			if(syshp <= 0){
				cout << "恭喜您获得了胜利！" << endl;
				return 1;
			}
		}
	}
	return 0;
}

const int Battle::dmg(int i){	/*造成伤害，参数为1表示我方造成伤害，为2表示对方造成伤害*/ 
	float at_lv, at_at, de_de;
	if(i == 1){
		at_lv = userpet.getinfo(2);
		at_at = userpet.getinfo(3);
		de_de = syspet.getinfo(4);				
	}
	else if(i == 2){
		at_lv = syspet.getinfo(2);
		at_at = syspet.getinfo(3);
		de_de = userpet.getinfo(4);		
	}
	int times = 1;				/*伤害采用随机机制：随机系数为1表示正常，2表示暴击（伤害*2），0表示闪避（伤害为0）*/ 
	switch(rand() % 16){
		case 1:
			times = 0;
			cout << "Miss！"<< endl;
			break;
		case 6:
		case 12:
			times = 2;
			cout << "打出了致命一击" << endl;
			break;
		default:
			break;
	}
	srand(time(NULL));
	int damage = ((at_lv * 0.4 + 2) * (at_lv * 10 + 10) * at_at / de_de / 50 + 2) * 1.5 * 1 * (255 - (rand() % 30)) / 255;
	damage *= times;
	if(times > 0){
		if(i == 1){
			syspet.showname();
		}
		else if(i == 2){
			userpet.showname();
		}
		cout << "受到了 " << damage << " 点伤害 " ;
	}
	 
	return damage;
}

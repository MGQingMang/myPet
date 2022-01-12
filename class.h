#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
using namespace std;

#define MAXBUFSIZE 1024 

class Pet{
	private:			/*�������԰�����*/ 
		int type;		/*��������*/ 
						/*�����ͣ�1�������ͣ�2�������ͣ�3������ͣ�4*/ 
		int attack;		/*�������ԣ�����*/ 
		int defence;	/*�������ԣ�����*/ 
		int speed;		/*�������ԣ��������*/ 
		int power;		/*�������ԣ�����ֵ*/ 
		char name[20];	/*����*/ 
		int level;		/*�ȼ������ڵ���1��С�ڵ���15��*/ 
		int exp;		/*����ֵ*/ 
		/*��������Ϊ�������*/
		int id;					/*�������*/ 
								/*����Ǿ�������ԣ�һ�����ȷ���������ơ����ࡢ����ֵ�͹�����ʽΨһȷ��*/ 
		int sp_attack;			/*����ֵ*/ 
		int sp_defence;			/*�������������������ֵ�͵ȼ�����*/ 
		int sp_speed;
		int sp_power;
		char skill_name[20];	/*������ʽ����*/ 
	public:
		void init_p(int, int);
		virtual void skill(){	/*������ʽ�������������Ϊ�����ܡ���*/ 
			cout << "��" << name << "��ʹ���ˡ�"<< skill_name << "��" << endl;
		}
		void lvup(int, int);	/*����*/ 
		const void show();			/*չʾ������Ϣ*/ 
		const void showname();		/*�����������*/ 
		const int getinfo(int);		/*��ȡ�����ĳ������*/ 
		int addexp(int);			/*����Ӿ���*/ 
};

void Pet::init_p(int pid,int lv){
								/*�������Գ�ʼ��*/ 
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

void Pet::lvup(int dlv, int out){	/*��������������Ϊ�����ĵȼ�������15��ʱ��Ϊ������15��*/ 
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
		cout << "��" << name << "�������ˣ�";
		cout << "�ȼ���" << flv << " + " << level - flv << " -> " << level << endl;
		cout << "������" << fat << " + " << attack - fat << " -> " << attack << endl;
		cout << "������" << fde << " + " << defence - fde << " -> " << defence << endl;
		cout << "���ݣ�" << fsp << " + " << speed - fsp << " -> " << speed << endl;
		cout << "������" << fpo << " + " << power - fpo << " -> " << power << endl;
	}
	
}

const void Pet::show(){	/*չʾ������Ϣ*/ 
	cout << "��ţ�" << id << endl; 
	cout << "����������" << name << "��" << endl;
	cout << "�ȼ���" << level << endl;
	cout << "������" << attack << endl;
	cout << "������" << defence << endl;
	cout << "���ݣ�" << speed << endl;
	cout << "������" << power << endl;
}

const void Pet::showname(){ /*�����������*/ 
	cout << "��" << name << "��" ;
}

const int Pet::getinfo(int flag){ 	/*��ȡ�����ĳ����Ϣ*/ 
	if(flag == 1){					/*����Ϊ1����ȡ�������*/ 
		return id;
	}
	else if(flag == 2){				/*����Ϊ2����ȡ����ȼ�*/ 
		return level;
	} 
	else if(flag == 3){				/*����Ϊ3����ȡ���鹥��*/ 
		return attack;
	} 
	else if(flag == 4){				/*����Ϊ4����ȡ�������*/ 
		return defence;
	} 
	else if(flag == 5){				/*����Ϊ5����ȡ��������*/ 
		return speed;
	} 
	else if(flag == 6){				/*����Ϊ6����ȡ��������ֵ*/ 
		return power;
	} 
	else if(flag == 7){				/*����Ϊ7����ȡ���龭��ֵ*/ 
		return exp;
	} 
}

int Pet::addexp(int dexp){	/*����Ӿ���*/ 
							/*������ƣ������龭��ﵽ�ȼ���ƽ����10ʱ������������������������ֵ�����ۼ�*/ 
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
		char username[20];		/*�û���*/ 
		int online;				/*����״̬������Ϊ1��������Ϊ0��δע��Ϊ-1*/ 
		int pet_tot;			/*�û���������*/ 
		int pet_tot15;			/*15����������*/ 
		Pet* petp;				/*����ָ��*/ 
		char password[20];		/*����*/ 
		int win;				/*ʤ��*/ 
		int lose; 				/*����*/ 
		int badge;				/*����������£�δӵ��Ϊ0��ͭ1����2����3*/
		int badge15;			/*�߼��������*/ 
	public:
		Users();
		void signin(char* ,char*);			/*ע��*/	 
		void log(int);				 		/*��¼�͵ǳ�*/ 
		int checkstatus();					/*����Ƿ�ע�ᡢ��¼״̬*/ 
		const void showpet();				/*�鿴ӵ�еľ���*/ 
		const void showusername(char*);		/*�鿴�û���*/ 
		int newpet(int, int);				/*����һֻ����*/ 
		const int usntaken(char*);			/*����û����Ƿ�ƥ��*/
		const int pswtaken(char*);			/*��������Ƿ�ƥ��*/ 
		const void getusername(char*);		/*��ȡ�û���*/ 
		const int getpet(char*);			/*��ȡ�û��ľ��飨��ţ�����������*/ 
		void badge_upd();					/*�����û��Ļ���*/
		const int getuserinfo(int); 		/*��ȡ�û�ʤ������������Ϣ*/ 
		void userwin(int);					/*��¼�û�ʤ/����+1*/
		int addexp_pet(int, int);			/*���û���һָֻ���������Ӿ���ֵ*/ 
		const void getpet_one(char* ,int);	/*��ȡ�û���ָ��������ź͵ȼ�*/ 
		void losepet(int);					/*ʧȥһֻ����*/ 
};

Users::Users(){
	online = -1;
	for(int i = 0; i < 20; i++){
		username[i] = '\0';
		password[i] = '\0';
	}
}

void Users::signin(char* usern, char* userp){//ע�ᣬ����Ϊ�û��������� 
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

void Users::log(int s){//����Ϊ1��ʾ��¼������Ϊ0��ʾ�ǳ� 
	online = s;
}

int Users::checkstatus(){//�����û�������״̬ 
	return online;
}

int Users::newpet(int num,int lv){//�û�����¾��飬����Ϊ�������,���ظ��û��µľ������� 
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

void Users::losepet(int num){//�û�ʧȥ���飬����Ϊʧȥ�ڼ�ֻ���� 
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

const int Users::usntaken(char* in){/*����û����Ƿ�ƥ��*/
	if(memcmp(in,username,20) == 0){
		return 1;
	}
	else{
		return 0;
	}
}

const int Users::pswtaken(char* in){/*��������Ƿ�ƥ��*/
	if(memcmp(in,password,20) == 0){
		return 1;
	}
	else{
		return 0;
	}
}

const void Users::getusername(char* usn){/*��ȡ�û���*/
	memcpy(usn, username, 20);
}

const int Users::getpet(char* buf){/*��ȡ�û��ľ��飨��ţ�����������*/
	for(int i = 0; i < pet_tot; i++){
		*(buf + i * 2) = petp[i].getinfo(1);
		*(buf + i * 2 + 1) = petp[i].getinfo(2);
	}
	return pet_tot;
}

const int Users::getuserinfo(int flag){	/*��ȡ�û�ʤ������������Ϣ*/
	if(flag == 1){						/*����Ϊ1����ȡʤ����*/ 
		return win;
	}
	else if(flag == 2){					/*����Ϊ2����ȡ������*/ 
		return lose;
	} 
	else if(flag == 3){					/*����Ϊ3����ȡ�����������*/ 
		badge_upd();
		return badge;
	} 
	else if(flag == 4){					/*����Ϊ4����ȡ�߼��������*/ 
		badge_upd();
		return badge15;
	}
}

void Users::userwin(int i){	/*��¼�û�ʤ/������+1*/ 
	if(i == 1){
		win++;
	}
	else if(i == 2){
		lose++;
	}
}

int Users::addexp_pet(int petnum, int dexp){	/*���û��ĵ�petnumֻ��������dexp����ֵ*/ 
	int dlv = petp[petnum].addexp(dexp);
	if(dlv + petp[petnum].getinfo(2) >= 15){
		pet_tot15++;
		badge_upd();
	}
	
	return dlv;
}

const void Users::getpet_one(char* buf,int num){	/*��ȡ�û���numֻ���������*/ 
	for(int i = 0; i < 6; i++){
		*(buf + i) = petp[num].getinfo(i + 1);
	}	
}

void Users::badge_upd(){	/*�����û������ֻ���*/ 
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
		Pet userpet;				/*�ҷ���ս����*/ 
		Pet syspet;					/*�Է���ս����*/ 
		int userhp;					/*�ҷ���ǰѪ��*/ 
		int syshp;					/*�Է���ǰѪ��*/ 
		Battle(int, int, int, int);
		int battle();				/*��ʼ��ս*/ 
		const int dmg(int);			/*����˺�*/ 
};

Battle::Battle(int usr_id, int usr_lv, int sys_id, int sys_lv){	/*��ս˫����Ϣ��ʼ��*/ 
	userpet.init_p(usr_id, usr_lv);
	//userpet.lvup(usr_lv - 1, 0);
	syspet.init_p(sys_id, sys_lv);
	//syspet.lvup(sys_lv - 1, 0);
}

int Battle::battle(){	/*˫����ʼ��ս����һ����ǰ����ֵ��Ϊ0ʱ��ս����*/ 
	int userhp = userpet.getinfo(6);
	int userhpmax = userpet.getinfo(6);
	int syshp = syspet.getinfo(6);
	int syshpmax = syspet.getinfo(6);
	userpet.showname();
	cout << "����ֵ��" << userhp << " / " << userhpmax << endl;
	syspet.showname();
	cout << "����ֵ��" << syshp << " / " << syshpmax << endl;
	cout << "ս����ʼ��" << endl;
	srand(time(NULL));
	for(int i = 0; ; i++){
		Sleep(1000);
		//cout << userpet.getinfo(5) << " " << syspet.getinfo(5) << endl;
		if(userpet.getinfo(5) >= syspet.getinfo(5)){
			userpet.skill();
			syshp -= dmg(1);
			cout << "����ֵ��" << syshp << " / " << syshpmax << endl << endl;
			if(syshp <= 0){
				cout << "��ϲ�������ʤ����" << endl;
				return 1;
			}
			syspet.skill();
			userhp -= dmg(2);
			cout << "����ֵ��" << userhp << " / " << userhpmax << endl << endl;
			if(userhp <= 0){
				cout << "���ź�����ʧ���ˣ�" << endl;
				return 2; 
			}
		}
		else{
			syspet.skill();
			userhp -= dmg(2);
			cout << "����ֵ��" << userhp << " / " << userhpmax << endl << endl;
			if(userhp <= 0){
				cout << "���ź�����ʧ���ˣ�" << endl; 
				return 2;
			}
			userpet.skill();
			syshp -= dmg(1);
			cout << "����ֵ��" << syshp << " / " << syshpmax << endl << endl;
			if(syshp <= 0){
				cout << "��ϲ�������ʤ����" << endl;
				return 1;
			}
		}
	}
	return 0;
}

const int Battle::dmg(int i){	/*����˺�������Ϊ1��ʾ�ҷ�����˺���Ϊ2��ʾ�Է�����˺�*/ 
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
	int times = 1;				/*�˺�����������ƣ����ϵ��Ϊ1��ʾ������2��ʾ�������˺�*2����0��ʾ���ܣ��˺�Ϊ0��*/ 
	switch(rand() % 16){
		case 1:
			times = 0;
			cout << "Miss��"<< endl;
			break;
		case 6:
		case 12:
			times = 2;
			cout << "���������һ��" << endl;
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
		cout << "�ܵ��� " << damage << " ���˺� " ;
	}
	 
	return damage;
}

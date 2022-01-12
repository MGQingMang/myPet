/*Server*/ 

#include <iostream>
#include <winsock2.h>
#include "class.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;

Users user[256];
DWORD WINAPI ServerThread(LPVOID lpParameter);
const int PORT = 8000;
int user_max = 0;			/*已注册用户总数*/ 
int lock = 0; 			
int pet_max = 0;			
int user_online = 0;		/*当前在线用户数*/ 

class Func{
	public:
		void mullock();	
		void unlock();
		void sort(char*, int);
		int findusernum(char*);		/*根据用户名查找其序号*/ 
};
Func func;


int main(){
	ifstream fp("input.txt",ios::binary);
	fp >> pet_max;
	fp.close();
	
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ListenAddr;
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	ListenAddr.sin_port = htons(PORT);
	
	int n;
	n = bind(ListenSocket, (LPSOCKADDR)&ListenAddr, sizeof(ListenAddr));
	if(n == SOCKET_ERROR){
		cout << "端口绑定失败！" << endl;
		return -1;
	}
	else{
		cout << "端口绑定成功：" << PORT << endl;
	}
	
	int l = listen(ListenSocket, 20);
	cout << "服务器准备就绪，等待连接请求" << endl;
	
	while(1){
		SOCKET* ClientSocket = new SOCKET;
		ClientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		int SockAddlen = sizeof(sockaddr);
		*ClientSocket = accept(ListenSocket, 0, 0);
		cout << "一个客户端已连接到服务器，socket是：" << *ClientSocket << endl;
		CreateThread(NULL, 0, &ServerThread, ClientSocket, 0, NULL);		 
	}
	
	closesocket(ListenSocket);
	WSACleanup();
	
	return 0; 
} 

DWORD WINAPI ServerThread(LPVOID lpParameter){
	SOCKET* ClientSocket = (SOCKET*)lpParameter;
	
	Pet pet[pet_max];
	for(int i = 0; i < pet_max; i++){
		pet[i].init_p(i, i - 1);
		//pet[i].lvup(i - 1, 0);
	}
	int rbyte = 0;
	char sbuf[MAXBUFSIZE];
	char rbuf[MAXBUFSIZE];
	sbuf[0] = 2;
	char usn[20];
	char psw[20];
	int taken = -1; 
	int s_online = 0;
	char online_usn[20]; 
	for(int i = 0; i < 20; i++){
		online_usn[i] = '\0';
	}
	int online_usernum = -1;
	
	while(1){
		rbyte = recv(*ClientSocket, rbuf, sizeof(rbuf),0);
		cout << "————————————————" << endl;
		cout << "客户端："  << *ClientSocket << endl;
		cout << "状态：";
		if(s_online == 1){
			cout << "在线，在线用户：" << online_usn << "，序号：" << online_usernum << endl;
		} 
		else{
			cout << "离线" << endl;
		}
		if(rbyte > 0){
			if(rbuf[0] == 1){
				for(int i = 0; i < 20; i++){
					usn[i] = '\0';
					psw[i] = '\0';
				}
				taken = -1;
				if(rbuf[1] == 1){//接收到注册请求 
					sbuf[1] = 1;
					memcpy(usn, &rbuf[2], 20);
					memcpy(psw, &rbuf[22], 20);				
					cout << "请求注册，用户名：" << usn << "，密码：" << psw << endl;
					taken = func.findusernum(usn);
					if(taken == -1){//用户名不重复，注册成功 
						func.mullock();
						user[user_max].signin(usn, psw);
						func.unlock();					
						sbuf[2] = 1;
						cout << "注册成功，用户序号：" << user_max << endl;
						srand(time(NULL));
						int randpet;
						cout << "随机发放3只新精灵："; 
						for(int i = 0; i < 3; i++){
							randpet = rand() % pet_max;
							user[user_max].newpet(randpet, 1);
							sbuf[3 + i] = randpet;
							pet[randpet].showname();
							cout << "（" << randpet << "）；" ;
						}
						cout << endl;
						user_max++;
					}
					else if(taken >= 0){//用户名重复，注册失败 
						sbuf[2] = 0;
						cout << "注册失败，与用户" << taken << "的用户名重复" << endl;
					}				 
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 2){//接收到登录请求 
					sbuf[1] = 2;
					memcpy(usn, &rbuf[2], 20);
					memcpy(psw, &rbuf[22], 20);
					cout << "请求登录，输入的用户名：" << usn << "，密码：" << psw << endl;
					taken = func.findusernum(usn);
					if(taken == -1){//找不到用户名，登录失败 					
						sbuf[2] = 0;
						cout << "该用户名未注册，登录失败" << endl;
					}
					else if(taken >= 0){//找到用户名
						func.mullock(); 
						if(user[taken].usntaken(usn) == 1 && user[taken].pswtaken(psw) == 1){
							if(user[taken].checkstatus() == 0){
								user[taken].log(1); 
								sbuf[2] = 2;
								sbuf[3] = taken;
								user_online++;							
								cout << "登录成功，用户序号：" << taken << endl;
								s_online = 1;
								online_usernum = taken;
								memcpy(online_usn, usn, 20);
							}
							else if(user[taken].checkstatus() == 1){
								sbuf[2] = 3;
								cout << "登录失败，该用户已经在线，用户序号：" << taken << endl;
							}
						}
						else if(user[taken].usntaken(usn) == 1 && user[taken].pswtaken(psw) != 1){
							sbuf[2] = 1;
							cout << "密码错误，登录失败，用户序号：" << taken << endl;
						}
						func.unlock();
					}				 
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 3){//接收到登出请求
					sbuf[1] = 3;				 
					memcpy(usn, &rbuf[2], 20);
					taken = func.findusernum(usn);
					cout << "请求登出，用户名：" << usn << "，用户序号：" << taken << endl;
					func.mullock();
					if(user[taken].checkstatus() == -1){
						sbuf[2] = 2;
						cout << "error:该用户未注册" << endl; 
					}
					else if(user[taken].checkstatus() == 0){
						sbuf[2] = 0;
						cout << "登出失败，该用户不在线" << endl;
					}
					else if(user[taken].checkstatus() == 1){
						sbuf[2] = 1;
						user[taken].log(0);
						user_online--;
						cout << "登出成功" << endl;
						s_online = 0;
						for(int i = 0; i < 20; i++){
							online_usn[i] = '\0';
						}
					}
					func.unlock();
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 4){
					sbuf[1] = 4;
					cout << "请求查看已注册用户的所有精灵" << endl;
					cout << "查询结果：" << endl;
					sbuf[2] = user_max;
					cout << "已注册用户共 " << user_max << "位：" << endl;
					int j = 3;
					func.mullock();
					for(int i = 0; i < user_max; i++){
						int offset = 0,templv = 0, tempwin = 0, templose = 0, tempbadge = 0, tempbadge15 = 0;
						float tempwinrate = 0;
						user[i].getusername(usn);
						memcpy(&sbuf[j], usn, 20);
						cout << "用户" << i << "，用户名：" << usn << endl; 
						j += 20;
						offset = user[i].getpet(&sbuf[j + 1]);
						sbuf[j] = offset;
						j++;
						cout << "精灵共 " << offset << " 只：" << endl;

						for(int k = 0; k < offset; k++){
							pet[sbuf[j + k * 2]].showname();
							templv = sbuf[j + k * 2 + 1];
							cout << "等级：" << templv << endl;
						} 
						j += offset * 2;
						tempwin = user[i].getuserinfo(1);
						sbuf[j] = tempwin;
						j++;
						templose = user[i].getuserinfo(2);
						sbuf[j] = templose;
						j++;
						if(tempwin + templose == 0){
							tempwinrate = 0;
						}
						else{
							tempwinrate = ((float)tempwin / (tempwin + templose));
						}
						cout << "胜率：" << tempwinrate * 100 << "%（" << tempwin << "/" << templose << "）" << endl;
						tempbadge =  user[i].getuserinfo(3);
						sbuf[j] = tempbadge;
						j++;
						tempbadge15 = user[i].getuserinfo(4);
						sbuf[j] = tempbadge15;
						j++;
						cout << "宠物个数徽章：" ;
						if(tempbadge == 0){
							cout << "未获得" << endl; 
						}
						else if(tempbadge == 1){
							cout << "铜" << endl; 
						}
						else if(tempbadge == 2){
							cout << "银" << endl; 
						}
						else if(tempbadge == 3){
							cout << "金" << endl; 
						}
						cout << "高级宠物徽章：" ;
						if(tempbadge15 == 0){
							cout << "未获得" << endl; 
						}
						else if(tempbadge15 == 1){
							cout << "铜" << endl; 
						}
						else if(tempbadge15 == 2){
							cout << "银" << endl; 
						}
						else if(tempbadge15 == 3){
							cout << "金" << endl; 
						}
						cout << endl;
					}					
					func.unlock();
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 5){
					sbuf[1] = 5;
					cout << "请求查看当前在线用户" << endl;
					cout << "查询结果：当前共 " << user_online << " 名用户在线" << endl;
					sbuf[2] = user_online;
					int j = 3;
					func.mullock();
					for(int i = 0; i < user_max; i++){
						if(user[i].checkstatus() == 1){
							user[i].getusername(usn);
							memcpy(&sbuf[j], usn, 20);
							cout << "用户" << i << "，用户名：" << usn <<endl; 
							j += 20;
						}
					} 
					func.unlock();
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 6){
					sbuf[1] = 6;
					cout << "请求对战" << endl; 
					int user_pet_tot = user[online_usernum].getpet(&sbuf[3]);
					sbuf[2] = user_pet_tot;
					int j = 3;
					cout << "用户精灵共 " << user_pet_tot << " 只：" << endl; 
					for(int i = 0; i < user_pet_tot; i++){
						cout << i << " - ";
						cout << "序号：" << pet[sbuf[j]].getinfo(1);
						cout << "名称：";
						pet[sbuf[j]].showname();
						j++;
						cout << "等级：" << pet[sbuf[j]].getinfo(2) << endl;
						j++;
					}
					cout << "系统虚拟精灵共 " << pet_max << " 只：" << endl;
					sbuf[j] = pet_max;
					j++;
					for(int i = 0; i < pet_max; i++){
						cout << i << " - ";
						sbuf[j] = pet[i].getinfo(1);
						cout << "序号：" << pet[sbuf[j]].getinfo(1);
						cout << "名称：";
						pet[sbuf[j]].showname();
						j++;
						sbuf[j] = pet[i].getinfo(2);
						cout << "等级：" << pet[i].getinfo(2) << endl;
						j++;
					}
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
					
					rbyte = recv(*ClientSocket, rbuf, sizeof(rbuf),0);
					int battle_type = 0, user_choose = -1, sys_choose = -1, battle_win = 0;
					battle_type = rbuf[2];
					user_choose = rbuf[3];
					sys_choose = rbuf[4];
					battle_win = rbuf[5];
					cout << "用户选择的对战类型：";
					if(battle_type == 1){
						cout << "升级赛" << endl;
					} 
					else if(battle_type == 2){
						cout << "决斗赛" << endl;
					}
					cout << "用户出战精灵：" << user_choose << endl;
					cout << "系统出战精灵：" << sys_choose << endl;
					cout << "对战结果：";
					if(battle_win == 1){//胜利 
						cout << "用户胜利" << endl; 
						user[online_usernum].userwin(1);
						if(battle_type == 1){//升级赛胜利 
							cout << "获得经验值：" << pet[sys_choose].getinfo(2) * 20 << endl;
							sbuf[2] = user_choose;
							user[online_usernum].getpet_one(&sbuf[3], user_choose);
							int dlv = user[online_usernum].addexp_pet(user_choose, pet[sys_choose].getinfo(2) * 20);							
							sbuf[9] = dlv;
							user[online_usernum].getpet_one(&sbuf[10], user_choose);
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
						}
						else if(battle_type = 2){//决斗赛胜利 
							int newpet_id = pet[sys_choose].getinfo(1);
							int newpet_lv = pet[sys_choose].getinfo(2);
							user[online_usernum].newpet(newpet_id, newpet_lv);
							sbuf[2] = newpet_id;
							sbuf[3] = newpet_lv;
							cout << "用户获得新精灵：" ;
							pet[newpet_id].showname();
							cout << "，等级：" << newpet_lv << endl; 
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
						}
					} 
					else if(battle_win == 2){//失败 
						cout << "用户失败" << endl;
						user[online_usernum].userwin(2); 
						if(battle_type == 2){
							int lose_max = 3;
							
							srand(time(NULL));
							if(user_pet_tot < 3){
								lose_max = user_pet_tot;
							}
							char losepet[lose_max];
							srand(time(NULL));
							for(int i = 0; i < lose_max; i++){
								losepet[i] = rand() % user_pet_tot;
								for(int j = 0; j < i; j++){
									if(losepet[j] == losepet[i]){
										i--;
										break;
									}
								}
							}
							func.sort(losepet, lose_max);
							sbuf[2] = lose_max;
							cout << "用户需送出 " << lose_max << " 只精灵中的一只，序号：" ;
							for(int i = 0; i < lose_max; i++){
								user[online_usernum].getpet_one(&sbuf[3 + 2 * i], losepet[i]);
								cout << (int)losepet[i] << " ";
							}							 
							cout << endl;
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
							rbyte = recv(*ClientSocket, rbuf, sizeof(rbuf),0);
							int lose = losepet[rbuf[2]];
							cout << "用户选择失去精灵的序号：" << lose << endl;
							sbuf[2] = lose;
							user[online_usernum].getpet_one(&sbuf[3], lose);
							user[online_usernum].losepet(lose);
							sbuf[5] = -1;
							if(lose_max <= 1){
								srand(time(NULL));
								int randpet = rand() % pet_max;
								user[online_usernum].newpet(randpet, 1);
								cout << "用户获得1只新精灵："; 
								pet[randpet].showname();
								cout << "（" << randpet << "）" << endl;
								sbuf[5] = randpet;
								sbuf[6] = 1;
							}
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
						}
					} 
				}
			}
			else{
				cout << "error " << (int)rbuf[0] << endl;
			}
		}		
		else{
			break;
		}
		memset(rbuf, 0, sizeof(rbuf));
	}	
	
	cout << "一个客户端已断开连接，socket是：" << *ClientSocket << endl;
	closesocket(*ClientSocket);
	free(ClientSocket);
	
	return 0;
}

void Func::mullock(){
	while(lock != 0){
	}
	lock = 1;
}
void Func::unlock(){
	lock = 0;
}

int Func::findusernum(char* usn){
	int tkn = -1;
	for(int i = 0; i < user_max; i++){
		func.mullock();
		if(user[i].usntaken(usn) == 1){
			tkn = i;
			//break;
		}
		func.unlock();
	}
	return tkn;
}

void Func::sort(char* left, int size){
	char* Ptr = NULL;
	int loc, temp;
	for(loc = size-1; loc >= 1; loc--){
		for(Ptr = left; Ptr < (left+loc); Ptr++){
			if(*Ptr > *(Ptr+1)){
				temp = *Ptr;
				*Ptr = *(Ptr+1);
				*(Ptr+1) = temp;
			}			
		} 
	}
}
	
